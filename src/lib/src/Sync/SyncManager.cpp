#include "libtimeit/sync/SyncManager.h"

#include <libtimeit/Utils.h>
#include <iostream>
#include <libtimeit/sync/Json.h>
#include <libtimeit/db/DefaultValues.h>
#include <sstream>
#include <libintl.h>

#include <libtimeit/internal/gettext.h>

namespace libtimeit
{

using namespace std;

const int ONE_DAY = 60 * 60 * 24;

SyncManager::SyncManager(
		IDatabase &database,
		INetwork  &op_network,
		Notifier& notifier,
		Timer&    timer
		):
		timer_(timer),
		notifier_(notifier),
		network(op_network)
{
	int len = 1;
	auto text = unique_ptr<char[]>(new char[len]);

	taskAccessor = database.getTaskAccessor();
	timeAccessor = database.getTimeAccessor();
	settingsAccessor = database.getSettingsAccessor();

	timer_.attach(this);
}

SyncManager::~SyncManager()
{
	timer_.detach(this);
}

SyncState SyncManager::status()
{
	return state;
}

void SyncManager::on_signal_1_second()
{
	switch (state)
	{
		case SyncState::IDLE:
		{
			time_t now = libtimeit::now();
			if (isActive() && now > nextFullSync)
			{
				nextFullSync = now + ONE_DAY;
				lastSync = 0;
			}
			if (isActive() && now > nextSync)
			{
				state = SyncState::TASK_REQUEST;
				currentSync = now;
				nextSync = getNextSync(currentSync);
			}
		}
			break;
		case SyncState::TASK_REQUEST:
			outstandingRequest = requestTasks(lastSync);
			state = SyncState::WAIT;
			following_state = SyncState::TASK_STORE;
			break;
		case SyncState::WAIT:
			if ( requestDone() )
			{
				auto response = outstandingRequest->futureResponse.get();
				if (response.statusOK() && response.httpCode() == 200)
				{
					state = following_state;
				}
				else
				{
					state = SyncState::FAIL;
				}
			}
			break;
		case SyncState::TASK_STORE:
			syncTasksToDatabase();
			state = SyncState::TIME_REQUEST;
		case SyncState::TIME_REQUEST:
			outstandingRequest = requestTimes(lastSync);
			state = SyncState::WAIT;
			following_state = SyncState::TIME_STORE;
			break;
		case SyncState::TIME_STORE:
			syncTimesToDatabase();
			state = SyncState::IDLE;
			nextSync = getNextSync(currentSync);
			lastSync = currentSync;
			break;
		case SyncState::FAIL:
			manageNetworkProblems();
			state = SyncState::IDLE;
			nextSync = getNextSync(currentSync);
			break;
		default:
			state = SyncState::IDLE;
			break;
	}

}

bool SyncManager::requestDone()
{
	auto future = outstandingRequest->futureResponse;
	if(future.valid())
	{
		return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	else
	{
		return false;
	}
}

time_t SyncManager::getNextSync( time_t referencePoint )
{

	int syncInterval = settingsAccessor->GetIntByName("SyncInterval", DEFAULT_SYNC_INTERVAL);
	int secondBetweenSyncs = syncInterval * 60;
	return referencePoint + secondBetweenSyncs;
}

bool SyncManager::isActive()
{
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	if (baseUrl.length() > 0 && username.length() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void SyncManager::syncTasksToDatabase()
{
	auto result = outstandingRequest->futureResponse.get();
	shared_ptr<std::vector<Task> > tasks = toTasks(result.response());

	shared_ptr<std::vector<Task> > tasksToUpdate = shared_ptr<vector<Task>>(new vector<Task>);
	for (Task task : *tasks)
	{
		int64_t id = taskAccessor->uuidToId(task.getUUID());
		auto parentUUID = task.parentUUID();
		bool completed = task.completed();
		int64_t parent = 0;
		time_t lastChanged = task.lastChanged();
		string name = task.name();
		auto uuid = task.getUUID();
		bool deleted = task.deleted();
		if (parentUUID)
		{
			parent = taskAccessor->uuidToId(*parentUUID);
			if (parent == 0)
			{
				tasksToUpdate->push_back(task);
			}
		}
		Task tempTask(name, parent, uuid, completed, id, lastChanged, parentUUID, deleted);
		if (id > 0)
		{
			taskAccessor->updateTask(tempTask);
		}
		else
		{
			taskAccessor->newTask(tempTask);
		}
	}
//Update tasks that had missing data earlier
	for (Task task : *tasksToUpdate)
	{
		auto parentUUID = task.parentUUID();
		if (parentUUID)
		{
			int64_t id = taskAccessor->uuidToId(task.getUUID());
			int64_t parent = taskAccessor->uuidToId(*parentUUID);
			bool completed = task.completed();
			time_t lastChanged = task.lastChanged();
			string name = task.name();
			auto uuid = task.getUUID();
			bool deleted = task.deleted();
			Task tempTask(name, parent, uuid, completed, id, lastChanged, parentUUID, deleted);
			taskAccessor->updateTask(tempTask);
		}
	}
	taskAccessor->enableNotifications(true);
}

void SyncManager::syncTimesToDatabase()
{
	auto result = outstandingRequest->futureResponse.get();
	TimeList times = toTimes(result.response());

	taskAccessor->enableNotifications(false);

	for (TimeEntry item : times)
	{
		auto taskUUID = item.taskUUID();
		int64_t taskID = taskAccessor->uuidToId(*taskUUID);
		auto uuid = item.getUUID();
		int id = timeAccessor->uuidToId(uuid);
		time_t changed = item.changed();
		bool deleted = item.deleted();
		time_t start = item.start();
		time_t stop = item.stop();
		bool running = false;
		if (id > 0)
		{
			auto originalItem = timeAccessor->getByID(id);
			if(originalItem)
			{
				running = originalItem->running();
			}
		}

		TimeEntry te(id, uuid, taskID, taskUUID, start, stop, deleted, running, changed);
		if (id > 0)
		{
			timeAccessor->update(te);
		}
		else
		{
			timeAccessor->newEntry(te);
		}
	}
	taskAccessor->enableNotifications(true);
}

shared_ptr <asyncHTTPResponse> SyncManager::requestTasks(time_t sincePointInTime)
{
	std::shared_ptr<std::vector<Task> > tasks = taskAccessor->getTasksChangedSince(sincePointInTime);
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	string password = settingsAccessor->GetStringByName("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/tasks/" + username + "/" + std::to_string(sincePointInTime);
	bool ignoreCertError = settingsAccessor->GetBoolByName("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	std::string jsonString = toJson(tasks, username);
	return network.request(url, jsonString, username, password, ignoreCertError);
}

shared_ptr <asyncHTTPResponse> SyncManager::requestTimes(time_t sincePointInTime)
{
	TimeList times = timeAccessor->getTimesChangedSince(sincePointInTime);
	std::string jsonString = toJson(times);
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	string password = settingsAccessor->GetStringByName("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/times/" + username + "/" + std::to_string(sincePointInTime);
	bool ignoreCertError = settingsAccessor->GetBoolByName("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);

	return network.request(url, jsonString, username, password, ignoreCertError);
}

void SyncManager::manageNetworkProblems()
{
	auto result = outstandingRequest->futureResponse.get();
	if (result.statusOK() == false || result.httpCode() != 200)
	{
		std::stringstream text;
		// %s is replaced with the URI on which the connection failed
		text << libtimeit::string_printf(_("Failed connection to %s:\n"), result.url().c_str());

		text << _("HTTP error ") << result.httpCode() << " ";
		if (result.httpCode() == 401)
		{
			text << _("Username or password is wrong.");
		}
		else
		{
			text << result.errorMessage();
		}

		notifier_.send(EventType::ERROR_MESSAGE, _("Network error"), text.str() );
	}
}

void SyncManager::reset()
{
	nextFullSync = 0;
	nextSync = 0;
}


}
