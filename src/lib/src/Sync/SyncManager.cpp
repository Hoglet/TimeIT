#include "libtimeit/sync/SyncManager.h"

#include <libtimeit/Utils.h>
#include <iostream>
#include <libtimeit/sync/Json.h>
#include <libtimeit/db/default_values.h>
#include <sstream>
#include <libintl.h>

#include <libtimeit/internal/gettext.h>

namespace libtimeit
{

using namespace std;

const int ONE_DAY = 60 * 60 * 24;

SyncManager::SyncManager(
		Database &database,
		INetwork  &op_network,
		Notifier& notifier,
		Timer&    timer
		):
		timer_(timer),
		notifier_(notifier),
		network(op_network),
		taskAccessor(database),
		timeAccessor(database),
		settingsAccessor(database)

{
	int len = 1;
	auto text = unique_ptr<char[]>(new char[len]);
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

	int syncInterval = settingsAccessor.get_int("SyncInterval", DEFAULT_SYNC_INTERVAL);
	int secondBetweenSyncs = syncInterval * 60;
	return referencePoint + secondBetweenSyncs;
}

bool SyncManager::isActive()
{
	string baseUrl = settingsAccessor.get_string("URL", DEFAULT_URL);
	string username = settingsAccessor.get_string("Username", DEFAULT_USER);
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
	vector<Task> tasks = toTasks(result.response());

	vector<Task> tasksToUpdate;
	for (Task task : tasks)
	{
		int64_t id = taskAccessor.ID(task.get_UUID());
		auto parentUUID = task.parent_UUID();
		bool completed = task.completed();
		int64_t parent = 0;
		time_t lastChanged = task.last_changed();
		string name = task.name();
		auto uuid = task.get_UUID();
		bool deleted = task.deleted();
		if (parentUUID)
		{
			parent = taskAccessor.ID(*parentUUID);
			if (parent == 0)
			{
				tasksToUpdate.push_back(task);
			}
		}
		Task tempTask(name, parent, uuid, completed, id, lastChanged, parentUUID, deleted);
		if (id > 0)
		{
			taskAccessor.update(tempTask);
		}
		else
		{
			taskAccessor.create(tempTask);
		}
	}
//Update tasks that had missing data earlier
	for (Task task : tasksToUpdate)
	{
		auto parentUUID = task.parent_UUID();
		if (parentUUID)
		{
			int64_t id = taskAccessor.ID(task.get_UUID());
			int64_t parent = taskAccessor.ID(*parentUUID);
			bool completed = task.completed();
			time_t lastChanged = task.last_changed();
			string name = task.name();
			auto uuid = task.get_UUID();
			bool deleted = task.deleted();
			Task tempTask(name, parent, uuid, completed, id, lastChanged, parentUUID, deleted);
			taskAccessor.update(tempTask);
		}
	}
	taskAccessor.enable_notifications(true);
}

void SyncManager::syncTimesToDatabase()
{
	auto result = outstandingRequest->futureResponse.get();
	Time_list times = toTimes(result.response());

	taskAccessor.enable_notifications(false);

	for (Time_entry item : times)
	{
		auto taskUUID = item.task_UUID();
		int64_t taskID = taskAccessor.ID(*taskUUID);
		auto uuid = item.get_UUID();
		int id = timeAccessor.uuid_to_id(uuid);
		time_t changed = item.changed();
		bool deleted = item.deleted();
		time_t start = item.start();
		time_t stop = item.stop();
		bool running = false;
		if (id > 0)
		{
			auto originalItem = timeAccessor.by_ID(id);
			if(originalItem)
			{
				running = originalItem->running();
			}
		}

		Time_entry te(id, uuid, taskID, taskUUID, start, stop, deleted, running, changed);
		if (id > 0)
		{
			timeAccessor.update(te);
		}
		else
		{
			timeAccessor.create(te);
		}
	}
	taskAccessor.enable_notifications(true);
}

shared_ptr <asyncHTTPResponse> SyncManager::requestTasks(time_t sincePointInTime)
{
	vector<Task> tasks = taskAccessor.changed_since(sincePointInTime);
	string baseUrl = settingsAccessor.get_string("URL", DEFAULT_URL);
	string username = settingsAccessor.get_string("Username", DEFAULT_USER);
	string password = settingsAccessor.get_string("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/tasks/" + username + "/" + std::to_string(sincePointInTime);
	bool ignoreCertError = settingsAccessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	string jsonString = toJson(tasks, username);
	return network.request(url, jsonString, username, password, ignoreCertError);
}

shared_ptr <asyncHTTPResponse> SyncManager::requestTimes(time_t sincePointInTime)
{
	Time_list times = timeAccessor.times_changed_since(sincePointInTime);
	std::string jsonString = toJson(times);
	string baseUrl = settingsAccessor.get_string("URL", DEFAULT_URL);
	string username = settingsAccessor.get_string("Username", DEFAULT_USER);
	string password = settingsAccessor.get_string("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/times/" + username + "/" + std::to_string(sincePointInTime);
	bool ignoreCertError = settingsAccessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);

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
