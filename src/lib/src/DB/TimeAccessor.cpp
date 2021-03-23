#include <iostream>
#include <sstream>
#include <string>
#include <libtimeit/db/TimeAccessor.h>

using namespace std;
namespace libtimeit
{

TimeAccessor::TimeAccessor(Database &op_database) : database(op_database)
{
}

TimeAccessor::~TimeAccessor()
= default;

void TimeAccessor::stopAllRunning()
{
	database.exe("UPDATE times SET running = 0");
}

int64_t TimeAccessor::newTime(int64_t taskID, time_t start, time_t stop)
{
	time_t now = time(nullptr);
	TimeEntry te(0, UUID(), taskID, {}, start, stop, false, false, now);
	return newEntry(te);
}

void TimeAccessor::updateTime(int64_t timeID, time_t startTime, time_t stopTime)
{
	auto te = getByID(timeID);
	if(te)
	{
		time_t now = time(nullptr);
		stringstream statement;
		statement << "UPDATE times SET start = " << startTime << ", stop=" << stopTime;
		statement << ", changed=" << now;
		statement << " WHERE id=" << timeID;
		database.exe(statement.str());
		database.sendNotification(TASK_UPDATED, te->taskID());
	}
}

void TimeAccessor::remove(int64_t id)
{
	auto te = getByID(id);
	if(te)
	{
		update(te->withDeleted(true));
	}
}

std::optional<TimeEntry> TimeAccessor::getByID(int64_t id)
{
	Statement statement = database.prepare(
			"SELECT taskID, start, stop, running, changed, deleted, uuid, taskUUID FROM v_times WHERE id = ?");
	statement.bindValue(1, id);

	std::string taskUUID;
	time_t start = 0;
	time_t stop = 0;
	bool running = false;
	int64_t changed = 0;
	bool deleted = false;
	std::string uuid;

	QueryResult rows = statement.execute();
	if (rows.size() > 0)
	{
		vector<DataCell> row = rows.at(0);
		int     taskID   = row[0].getInt();
		time_t  start    = row[1].getInt();
		time_t  stop     = row[2].getInt();
		time_t  running  = row[3].getBool();
		int64_t changed  = row[4].getInt();
		bool    deleted  = row[5].getBool();
		auto    uuid     = toUuid(row[6].getString());
		auto    taskUUID = toUuid(row[7].getString());
		if( uuid && taskUUID )
		{

			return TimeEntry(id, *uuid, taskID, *taskUUID, start, stop, deleted, running, changed);
		}
	}
	return {};
}

int TimeAccessor::getTime(int64_t taskID, time_t start, time_t stop)
{
	int time = getTimeCompletelyWithinLimits(taskID, start, stop);
	if (stop > 0)
	{
		time += getTimePassingEndLimit(taskID, start, stop);
		time += getTimePassingStartLimit(taskID, start, stop);
	}
	return time;
}

int TimeAccessor::getTimeCompletelyWithinLimits(int64_t &taskID, time_t &start, time_t &stop)
{
	int time = 0;
	QueryResult rows;
	if (stop > 0)
	{
		Statement statement_timeCompletelyWithinLimits = database.prepare("SELECT SUM(stop-start) AS time "
				" FROM times  WHERE taskID = ? AND start>=? AND stop<=? and deleted=0;");
		statement_timeCompletelyWithinLimits.bindValue(1, taskID);
		statement_timeCompletelyWithinLimits.bindValue(2, start);
		statement_timeCompletelyWithinLimits.bindValue(3, stop);
		rows = statement_timeCompletelyWithinLimits.execute();
	}
	else
	{
		Statement statement_getTime = database.prepare("SELECT SUM(stop-start) AS time  FROM times WHERE taskID = ? AND deleted=0;");
		statement_getTime.bindValue(1, taskID);
		rows = statement_getTime.execute();
	}
	if (rows.size() == 1)
	{
		vector<DataCell> row = rows.at(0);
		if (row[0].hasValue())
		{
			time = row[0].getInt();
		}
	}

	return time;
}

int TimeAccessor::getTimePassingEndLimit(int64_t &taskID, time_t &start, time_t &stop)
{
	int time = 0;
	stringstream statement;
	statement << " SELECT SUM(" << stop << "-start) AS time  ";
	statement << " FROM times ";
	statement << " WHERE taskID = " << taskID;
	statement << " AND start < " << stop;
	statement << " AND stop  > " << stop;
	statement << " AND start > " << start;
	statement << " AND deleted=0 ";
	QueryResult rows = database.exe(statement.str());
	if (rows.size() == 1)
	{
		vector<DataCell> row = rows.at(0);
		if (row[0].hasValue())
		{
			time = row[0].getInt();
		}
	}
	return time;
}

int TimeAccessor::getTimePassingStartLimit(int64_t taskID, time_t start, time_t stop)
{
	int time = 0;
	stringstream statement;
	statement << "SELECT SUM(stop-" << start << ") AS time ";
	statement << " FROM times";
	statement << " WHERE taskID = " << taskID;
	statement << " AND start <  " << start;
	statement << " AND stop  >  " << start;
	statement << " AND stop  <  " << stop;
	statement << " AND deleted=0 ";
	QueryResult rows = database.exe(statement.str());
	if (rows.size() == 1)
	{
		vector<DataCell> row = rows.at(0);
		if (row[0].hasValue())
		{
			time = row[0].getInt();
		}
	}
	return time;
}

std::vector<int64_t> TimeAccessor::getLatestTasks(int amount)
{
	vector<int64_t> resultList;
	stringstream statement;
	statement << "SELECT DISTINCT times.taskid FROM times JOIN tasks ON times.taskID = tasks.id AND tasks.deleted='0' ORDER BY times.stop DESC LIMIT "
			<< amount;

	QueryResult rows = database.exe(statement.str());

	for (vector<DataCell> row : rows)
	{
		int id = row[0].getInt();
		resultList.push_back(id);
	}
	return resultList;

}

std::vector<int64_t> TimeAccessor::getRunningTasks()
{
	std::vector<int64_t> resultList;
	Statement statement_getRunningTasks = database.prepare(
			"SELECT DISTINCT times.taskid FROM times WHERE times.running;");
	QueryResult rows = statement_getRunningTasks.execute();

	for (std::vector<DataCell> row : rows)
	{
		int id = row[0].getInt();
		resultList.push_back(id);
	}
	return resultList;
}

TimeList TimeAccessor::getDetailTimeList(int64_t taskID, time_t startTime, time_t stopTime)
{
	TimeList resultList;
	stringstream statement;
	statement << "SELECT id, start, stop, deleted, running, changed, uuid, taskUUID FROM v_times ";
	statement << " WHERE stop > " << startTime;
	statement << " AND start <" << stopTime;
	statement << " AND taskID = " << taskID;
	statement << " AND deleted=0 ";

	QueryResult rows = database.exe(statement.str());
	for (vector<DataCell> row : rows)
	{
		int64_t id = row[0].getInt();
		time_t start = row[1].getInt();
		time_t stop = row[2].getInt();
		bool deleted = row[3].getBool();
		bool running = row[4].getBool();
		time_t changed = row[5].getInt();
		auto uuid = toUuid(row[6].getString());
		auto taskUUID = toUuid(row[7].getString());
		if (uuid && taskUUID)
		{
			resultList.push_back(TimeEntry(id, *uuid, taskID, *taskUUID, start, stop, deleted, running, changed));
		}
	}
	return resultList;
}

void TimeAccessor::setRunning(int64_t timeID, bool running)
{
	auto te = getByID(timeID);
	if(te)
	{
		int64_t taskID = te->taskID();
		stringstream statement;
		statement << "UPDATE times SET running = " << (int) running;
		statement << " WHERE id=" << timeID;

		database.exe(statement.str());
		database.sendNotification(TASK_UPDATED, taskID);
	}
}

TimeList TimeAccessor::getTimesChangedSince(time_t timestamp)
{
	TimeList result;

	Statement statement = database.prepare("SELECT taskID, start, stop, running, changed, deleted, uuid, id, taskUUID FROM v_times WHERE changed>=?");

	statement.bindValue(1, timestamp);

	QueryResult rows = statement.execute();
	for (vector<DataCell> row : rows)
	{
		int taskID = row[0].getInt();
		time_t start = row[1].getInt();
		time_t stop = row[2].getInt();
		bool running = row[3].getBool();
		time_t changed = row[4].getInt();
		bool deleted = row[5].getBool();
		auto uuid = toUuid(row[6].getString());
		int64_t id = row[7].getInt();
		auto taskUUID = toUuid(row[8].getString());
		if(uuid && taskUUID)
		{
			TimeEntry te(id, *uuid, taskID, *taskUUID, start, stop, deleted, running, changed);
			result.push_back(te);
		}
	}
	return result;
}

int64_t TimeAccessor::uuidToId(UUID uuid)
{
	int64_t id = 0;
	Statement statement_uuidToId = database.prepare("SELECT id FROM times WHERE uuid=?;");
	statement_uuidToId.bindValue(1, uuid.c_str());
	QueryResult rows = statement_uuidToId.execute();
	for (std::vector<DataCell> row : rows)
	{
		id = row[0].getInt();
	}
	return id;
}

bool TimeAccessor::update(const TimeEntry &item)
{
	int64_t id = item.ID();
	auto existingItem = getByID(id);
	if (existingItem && item != *existingItem && item.changed() >= existingItem->changed())
	{
		Statement statement_updateTime = database.prepare(
				"UPDATE times SET uuid=?, taskID=?, start=?, stop=?, running=?, changed=?, deleted=? WHERE id=?");
		statement_updateTime.bindValue(1, item.getUUID().c_str());
		statement_updateTime.bindValue(2, item.taskID());
		statement_updateTime.bindValue(3, item.start());
		statement_updateTime.bindValue(4, item.stop());
		statement_updateTime.bindValue(5, item.running());
		statement_updateTime.bindValue(6, item.changed());
		statement_updateTime.bindValue(7, item.deleted());
		statement_updateTime.bindValue(8, item.ID());

		statement_updateTime.execute();

		database.sendNotification(TASK_UPDATED, item.taskID());
		return true;
	}
	return false;
}
vector<int64_t> TimeAccessor::getChildrenIDs(int64_t taskID)
{
	vector<int64_t> result;
	Statement statement_getChildrenIDs = database.prepare("SELECT id FROM tasks WHERE parent=?;");
	statement_getChildrenIDs.bindValue(1, taskID);
	QueryResult rows = statement_getChildrenIDs.execute();
	for (vector<DataCell> row : rows)
	{
		int64_t id = row[0].getInt();
		result.push_back(id);
	}
	return result;
}

time_t TimeAccessor::getTotalTimeWithChildren(int64_t taskID, time_t start, time_t stop)
{
	time_t totalTime = getTime(taskID, start, stop);
	vector<int64_t> children = getChildrenIDs(taskID);
	for (int64_t child : children)
	{
		totalTime += getTotalTimeWithChildren(child, start, stop);
	}
	return totalTime;
}

int64_t TimeAccessor::newEntry(const TimeEntry &item)
{
	Statement statement_newEntry = database.prepare(
			"INSERT INTO times (uuid,taskID, start, stop, changed,deleted) VALUES (?,?,?,?,?,?)");
	statement_newEntry.bindValue(1, item.getUUID().c_str());
	statement_newEntry.bindValue(2, item.taskID());
	statement_newEntry.bindValue(3, item.start());
	statement_newEntry.bindValue(4, item.stop());
	statement_newEntry.bindValue(5, item.changed());
	statement_newEntry.bindValue(6, item.deleted());
	statement_newEntry.execute();

	if (item.start() != item.stop())
	{
		database.sendNotification(TASK_UPDATED, item.taskID());
	}
	return database.getIDOfLastInsert();
}

void TimeAccessor::upgradeToDB5()
{
	time_t now = time(nullptr);

	database.exe("DELETE FROM times WHERE taskID = 0");
	database.exe("ALTER TABLE times RENAME TO times_backup");
	createTable();
	createViews();

	Statement statement = database.prepare("SELECT id, taskID, start, stop FROM  times_backup");
	QueryResult rows = statement.execute();
	for (vector<DataCell> row : rows)
	{
		int64_t id = row[0].getInt();
		int64_t taskID = row[1].getInt();
		time_t start = row[2].getInt();
		time_t stop = row[3].getInt();
		TimeEntry item(id, UUID(), taskID, {}, start, stop, false, false, now);
		newEntry(item);
	}
}

void TimeAccessor::createTable()
{
	database.exe("CREATE TABLE IF NOT EXISTS times "
			"(id          INTEGER PRIMARY KEY,"
			" uuid        TEXT UNIQUE,"
			" taskID      INTEGER,"
			" start       INTEGER,"
			" stop        INTEGER,"
			" changed     INTEGER,"
			" deleted     BOOL    DEFAULT 0,"
			" running     BOOL    DEFAULT 0,"
			" FOREIGN KEY(taskID) REFERENCES tasks(id) "
			") ");
}

void TimeAccessor::createViews()
{
	database.exe("DROP VIEW IF EXISTS v_times");

	database.exe("CREATE VIEW v_times AS"
			" SELECT times.*, tasks.uuid AS taskUUID "
			" FROM times "
			" LEFT JOIN tasks "
			" ON times.taskID = tasks.id");
}

void TimeAccessor::removeShortTimeSpans()
{
	database.exe("DELETE FROM times WHERE stop-start < 30");
}

std::vector<int64_t> TimeAccessor::getActiveTasks(time_t start, time_t stop)
{
	std::vector<int64_t> resultList;
	Statement statement_getTasks =
			database.prepare(
					"SELECT DISTINCT times.taskid FROM times JOIN tasks ON times.taskID = tasks.id AND tasks.deleted='0' WHERE (times.start>=? AND times.start<=?) OR (times.stop>=? AND times.stop<=?) OR (times.start<? AND times.stop>?)  AND times.deleted=0;");
	statement_getTasks.bindValue(1, start);
	statement_getTasks.bindValue(2, stop);
	statement_getTasks.bindValue(3, start);
	statement_getTasks.bindValue(4, stop);
	statement_getTasks.bindValue(5, start);
	statement_getTasks.bindValue(6, stop);

	QueryResult rows = statement_getTasks.execute();

	for (vector<DataCell> row : rows)
	{
		int id = row[0].getInt();
		resultList.push_back(id);
	}
	return resultList;
}

}
