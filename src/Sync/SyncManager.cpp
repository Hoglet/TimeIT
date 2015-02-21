#include "SyncManager.h"

#include <Utils.h>
#include <glibmm.h>
#include <iostream>
#include "Json.h"
#include "DefaultValues.h"
#include <glibmm/i18n.h>
#include <Utils.h>


namespace syncing
{

using std::shared_ptr;
using std::cout;
using std::string;
using std::vector;
using DB::IDatabase;
using DB::Task;
using DB::TimeEntry;

SyncManager::SyncManager(shared_ptr<IDatabase>& database, shared_ptr<INetwork>& op_network, std::shared_ptr<Utils::MessageCenter> messageCenter)
{
	int len=1;
	auto text = std::unique_ptr<char[]>(new char[len]);

	thread = 0;
	inited = false;
	running = false;
	this->messageCenter = messageCenter;
	if(Glib::thread_supported() && database->isThreadSafe())
	{
		db = database;
		taskAccessor = database->getTaskAccessor();
		timeAccessor = database->getTimeAccessor();
		settingsAccessor = database->getSettingsAccessor();
		network = op_network;
		inited=true;
	}
}

//LCOV_EXCL_START
SyncManager::SyncManager()
{
	thread = 0;
	inited = false;
	running = false;
}
//LCOV_EXCL_STOP

SyncManager::~SyncManager()
{
	stop();
}

void SyncManager::start()
{
	if(inited)
	{
		running=true;
		thread = Glib::Thread::create(sigc::mem_fun(*this, &SyncManager::worker), true);
	}
}
void SyncManager::stop()
{
	if(inited && thread!=nullptr)
	{
		running = false;
		thread->join();
		thread=0;
	}
}

void SyncManager::worker()
{
	time_t nextFullSync = 0;
	while(running)
	{
		if(isActive())
		{
			time_t now=Utils::now();
			if(nextFullSync < now)
			{
				if(doSync(0))
				{
					nextFullSync = now + ONE_DAY;
				}
			}
			else
			{
				time_t	pointInTime = now-ONE_DAY;
				doSync(pointInTime);
			}
		}
		int syncInterval = settingsAccessor->GetIntByName("SyncInterval", DEFAULT_SYNC_INTERVAL);
		int secondBetweenSyncs = syncInterval * 60;
		for(int q=0; q<secondBetweenSyncs; q++)
		{
			Glib::usleep(1000*1000); //One second;
			if(!running)
			{
				break;
			}
		}
	}
}

bool SyncManager::isActive()
{
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	if(baseUrl.length()>0 && username.length()>0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
int SyncManager::syncTaskToDatabase(string result)
{
	uint32_t start = Utils::millisecondsSinceEpoch();
	std::shared_ptr<std::vector<Task> > tasks = json.toTasks(result);
	uint32_t json_conversion_done = Utils::millisecondsSinceEpoch();

	std::shared_ptr<std::vector<Task> > tasksToUpdate = shared_ptr<vector<Task>>(new vector<Task>);
	for (Task task : *tasks)
	{
		int64_t id = taskAccessor->uuidToId(task.getUUID());
		string parentUUID = task.getParentUUID();
		bool completed = task.getCompleted();
		int64_t parent = 0;
		time_t lastChanged = task.getLastChanged();
		string name = task.getName();
		string uuid = task.getUUID();
		bool deleted = task.getDeleted();
		if (parentUUID.size() > 0)
		{
			parent = taskAccessor->uuidToId(parentUUID);
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
	uint32_t stage1_done = Utils::millisecondsSinceEpoch();
//Update tasks that had missing data earlier
	for (Task task : *tasksToUpdate)
	{
		string parentUUID = task.getParentUUID();
		if (parentUUID.size() > 0)
		{
			int64_t id = taskAccessor->uuidToId(task.getUUID());
			int64_t parent = taskAccessor->uuidToId(parentUUID);
			bool completed = task.getCompleted();
			time_t lastChanged = task.getLastChanged();
			string name = task.getName();
			string uuid = task.getUUID();
			bool deleted = task.getDeleted();
			Task tempTask(name, parent, uuid, completed, id, lastChanged, parentUUID, deleted);
			taskAccessor->updateTask(tempTask);
		}
	}
	uint32_t stage2_done = Utils::millisecondsSinceEpoch();
	cout << "Task sync\n";
	cout << "Json conversion: " << json_conversion_done - start << " ms.\n";
	cout << "stage1: " << stage1_done - json_conversion_done << " ms.\n";
	cout << "stage2: " << stage2_done - stage1_done << " ms.\n";
	cout << "*****\n";
	return tasks->size();

}

int SyncManager::syncTimesToDatabase(string result)
{
	uint32_t start = Utils::millisecondsSinceEpoch();
	std::shared_ptr<std::vector<TimeEntry> > times = json.toTimes(result);
	uint32_t json_conversion_done = Utils::millisecondsSinceEpoch();

	for (TimeEntry item : *times)
	{
		std::string taskUUID = item.getTaskUUID();
		int64_t taskID = taskAccessor->uuidToId(taskUUID);
		std::string uuid = item.getUUID();
		int id = timeAccessor->uuidToId(uuid);
		time_t changed = item.getLastChanged();
		bool deleted = item.getDeleted();
		time_t start = item.getStart();
		time_t stop = item.getStop();
		bool running = false;
		if (id > 0)
		{
			TimeEntry originalItem = timeAccessor->getByID(id);
			running = originalItem.getRunning();
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
	uint32_t stage1_done = Utils::millisecondsSinceEpoch();

	cout << "Time sync\n";
	cout << "Json conversion: " << json_conversion_done - start << " ms.\n";
	cout << "stage1: " << stage1_done - json_conversion_done << " ms.\n";
	cout << "******\n";
	return times->size();
}

bool SyncManager::syncTasks(time_t sincePointInTime)
{
	std::shared_ptr<std::vector<Task> > tasks = taskAccessor->getTasksChangedSince(sincePointInTime);
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	string password = settingsAccessor->GetStringByName("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/tasks/" + username + "/" + std::to_string(sincePointInTime);
	bool ignoreCertError = settingsAccessor->GetBoolByName("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	std::string jsonString = json.toJson(tasks, username);
	NetworkResponse result= network->request(url, jsonString, username, password, ignoreCertError);
	if (result.statusOK && result.httpCode == 200)
	{
		syncTaskToDatabase(result.response);
		return true;
	}
	else
	{
		manageNetworkProblems(result);
		return false;
	}
}

bool SyncManager::syncTimes(time_t pointInTime)
{
	std::shared_ptr<std::vector<TimeEntry> > times = timeAccessor->getTimesChangedSince(pointInTime);
	std::string jsonString = json.toJson(times);
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	string password = settingsAccessor->GetStringByName("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/times/" + username + "/" + std::to_string(pointInTime);
	bool ignoreCertError = settingsAccessor->GetBoolByName("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);

	uint32_t start = Utils::millisecondsSinceEpoch();
	NetworkResponse result = network->request(url, jsonString, username, password, ignoreCertError);
	uint32_t end = Utils::millisecondsSinceEpoch();
	cout << "Server request " << end - start << " ms.\n";

	if (result.statusOK && result.httpCode == 200)
	{
		syncTimesToDatabase(result.response);
		return true;
	}
	else
	{
		manageNetworkProblems(result);
		return false;
	}
}

bool SyncManager::doSync(time_t pointInTime)
{
	bool success = false;
	uint32_t start = Utils::millisecondsSinceEpoch();
	db->beginTransaction();
	taskAccessor->enableNotifications(false);
	uint32_t tasks_done;
	if(syncTasks(pointInTime))
	{
		tasks_done = Utils::millisecondsSinceEpoch();
		if( syncTimes(pointInTime))
		{
			success = true;
		}
	}

	uint32_t times_done = Utils::millisecondsSinceEpoch();
	db->endTransaction();
	taskAccessor->enableNotifications(true);

	uint32_t totally_finished = Utils::millisecondsSinceEpoch();
	cout << "Processed the tasks in " << tasks_done - start << " ms.\n";
	cout << "Processed the times in " << times_done - tasks_done << " ms.\n";
	cout << "Update UI: " << totally_finished - times_done << "\n";

	return success;
}

void SyncManager::manageNetworkProblems(NetworkResponse result)
{
	if (result.statusOK == false || result.httpCode != 200 )
	{
		std::stringstream text;
		text << _("Failed connection to ");
		text << result.url << ":\n";
		text << _("HTTP error ") << result.httpCode << " ";
		if (result.httpCode == 401)
		{
			text << _("Username or password is wrong.");
		}
		text << result.errorMessage;
		Utils::Message message(Utils::ERROR_MESSAGE, _("Network error"), text.str());
		messageCenter->sendMessage(message);
	}
}

}
