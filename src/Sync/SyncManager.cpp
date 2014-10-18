#include "SyncManager.h"

#include <Utils.h>
#include <glibmm.h>
#include <iostream>
#include "Json.h"
#include "DefaultValues.h"

namespace syncing
{

using std::shared_ptr;
using std::cout;
using std::string;
using std::vector;
using DB::IDatabase;
using DB::Task;
using DB::TimeEntry;

SyncManager::SyncManager(shared_ptr<IDatabase>& database, shared_ptr<INetwork>& op_network)
{
	int len=1;
	auto text = std::unique_ptr<char[]>(new char[len]);

	thread = 0;
	inited = false;
	running = false;
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
	while(running)
	{
		if(isActive())
		{
			completeSync();
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
	uint32_t start = Utils::currentTime();
	std::shared_ptr<std::vector<Task> > tasks = json.toTasks(result);
	uint32_t json_conversion_done = Utils::currentTime();

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
	uint32_t stage1_done = Utils::currentTime();
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
	uint32_t stage2_done = Utils::currentTime();
	cout << "Task sync\n";
	cout << "Json conversion: " << json_conversion_done - start << " ms.\n";
	cout << "stage1: " << stage1_done - json_conversion_done << " ms.\n";
	cout << "stage2: " << stage2_done - stage1_done << " ms.\n";
	cout << "*****\n";
	return tasks->size();

}

int SyncManager::syncTimesToDatabase(string result)
{
	uint32_t start = Utils::currentTime();
	std::shared_ptr<std::vector<TimeEntry> > times = json.toTimes(result);
	uint32_t json_conversion_done = Utils::currentTime();

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
	uint32_t stage1_done = Utils::currentTime();

	cout << "Time sync\n";
	cout << "Json conversion: " << json_conversion_done - start << " ms.\n";
	cout << "stage1: " << stage1_done - json_conversion_done << " ms.\n";
	cout << "******\n";
	return times->size();
}

int SyncManager::syncTasks()
{
	int itemsProcessed = 0;
	std::shared_ptr<std::vector<Task> > tasks = taskAccessor->getTasksChangedSince();
	std::string jsonString = json.toJson(tasks);
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	string password = settingsAccessor->GetStringByName("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/tasks/" + username + "/";
	bool ignoreCertError = settingsAccessor->GetBoolByName("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	std::string result = network->request(url, jsonString, username, password, ignoreCertError);
	if (result.size() > 0)
	{
		itemsProcessed = syncTaskToDatabase(result);
	}
	return itemsProcessed;
}

int SyncManager::syncTimes()
{
	int itemsProcessed = 0;
	std::shared_ptr<std::vector<TimeEntry> > times = timeAccessor->getTimesChangedSince();
	std::string jsonString = json.toJson(times);
	string baseUrl = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	string username = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	string password = settingsAccessor->GetStringByName("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/times/" + username + "/";
	bool ignoreCertError = settingsAccessor->GetBoolByName("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	std::string result = network->request(url, jsonString, username, password, ignoreCertError);

	if (result.size() > 0)
	{
		itemsProcessed = syncTimesToDatabase(result);
	}
	return itemsProcessed;

}

void SyncManager::completeSync()
{
	uint32_t start = Utils::currentTime();
	db->beginTransaction();
	taskAccessor->enableNotifications(false);
	int tasksProcessed = syncTasks();
	uint32_t tasks_done = Utils::currentTime();
	int timesProcessed = 0;
	if(tasksProcessed>0)
	{
		timesProcessed = syncTimes();
	}
	uint32_t times_done = Utils::currentTime();
	db->endTransaction();
	taskAccessor->enableNotifications(true);

	uint32_t totally_finished = Utils::currentTime();
	cout << "Processed " << tasksProcessed << " tasks in " << tasks_done - start << " ms.\n";
	cout << "Processed " << timesProcessed << " times in " << times_done - tasks_done << " ms.\n";
	cout << "Update UI: " << totally_finished - times_done << "\n";

}

}
