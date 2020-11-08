#include "TimeAccessor.h"
#include <iostream>
#include <sstream>
#include <string>
#include <UUIDTool.h>

using namespace std;
using namespace DBAbstraction;
namespace DB
{

ITimeAccessor::~ITimeAccessor()
{

}

TimeAccessor::TimeAccessor(shared_ptr<CSQL> &op_db, std::shared_ptr<Notifier> &notifier)
{
	db = op_db;
	this->notifier = notifier;
}

TimeAccessor::~TimeAccessor()
{
}

void TimeAccessor::stopAllRunning()
{
	db->exe("UPDATE times SET running = 0");
}

int64_t TimeAccessor::newTime(int64_t taskID, time_t start, time_t stop)
{
	time_t now = time(0);
	TimeEntry te(0, UUIDTool::randomUUID(), taskID, "", start, stop, false, false, now);
	return newEntry(te);
}

void TimeAccessor::updateTime(int64_t timeID, time_t startTime, time_t stopTime)
{
	TimeEntry te = getByID(timeID);
	time_t now = time(0);
	stringstream statement;
	statement << "UPDATE times SET start = " << startTime << ", stop=" << stopTime;
	statement << ", changed=" << now;
	statement << " WHERE id=" << timeID;
	db->exe(statement.str());
	notifier->sendNotification(TASK_UPDATED, te.taskID());
}

void TimeAccessor::remove(int64_t id)
{
	TimeEntry te = getByID(id);
	update(te.withDeleted(true));
}

TimeEntry TimeAccessor::getByID(int64_t id)
{
	std::shared_ptr<Statement> statement = db->prepare(
			"SELECT taskID, start, stop, running, changed, deleted, uuid, taskUUID FROM v_times WHERE id = ?");
	statement->bindValue(1, id);

	int taskID = 0;
	std::string taskUUID;
	time_t start = 0;
	time_t stop = 0;
	bool running = false;
	int64_t changed = 0;
	bool deleted = false;
	std::string uuid;

	std::shared_ptr<QueryResult> rows = statement->execute();
	if (rows->size() > 0)
	{
		vector<DataCell> row = rows->at(0);
		taskID = row[0].getInt();
		start = row[1].getInt();
		stop = row[2].getInt();
		running = row[3].getBool();
		changed = row[4].getInt();
		deleted = row[5].getBool();
		uuid = row[6].getString();
		taskUUID = row[7].getString();
	}
	return TimeEntry(id, uuid, taskID, taskUUID, start, stop, deleted, running, changed);
}

int TimeAccessor::getTime(int64_t taskID, time_t start, time_t stop)
{
	int time = 0;
	time = getTimeCompletelyWithinLimits(taskID, start, stop);
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
	std::shared_ptr<QueryResult> rows;
	if (stop > 0)
	{
		std::shared_ptr<DBAbstraction::Statement> statement_timeCompletelyWithinLimits = db->prepare("SELECT SUM(stop-start) AS time "
				" FROM times  WHERE taskID = ? AND start>=? AND stop<=? and deleted=0;");
		statement_timeCompletelyWithinLimits->bindValue(1, taskID);
		statement_timeCompletelyWithinLimits->bindValue(2, start);
		statement_timeCompletelyWithinLimits->bindValue(3, stop);
		rows = statement_timeCompletelyWithinLimits->execute();
	}
	else
	{
		std::shared_ptr<DBAbstraction::Statement> statement_getTime = db->prepare("SELECT SUM(stop-start) AS time  FROM times WHERE taskID = ? AND deleted=0;");
		statement_getTime->bindValue(1, taskID);
		rows = statement_getTime->execute();
	}
	if (rows->size() == 1)
	{
		vector<DataCell> row = rows->at(0);
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
	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	if (rows->size() == 1)
	{
		vector<DataCell> row = rows->at(0);
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
	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	if (rows->size() == 1)
	{
		vector<DataCell> row = rows->at(0);
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

	std::shared_ptr<QueryResult> rows = db->exe(statement.str());

	for (vector<DataCell> row : *rows)
	{
		int id = row[0].getInt();
		resultList.push_back(id);
	}
	return resultList;

}

std::vector<int64_t> TimeAccessor::getRunningTasks()
{
	std::vector<int64_t> resultList;
	std::shared_ptr<Statement> statement_getRunningTasks = db->prepare(
			"SELECT DISTINCT times.taskid FROM times WHERE times.running;");
	std::shared_ptr<QueryResult> rows = statement_getRunningTasks->execute();

	for (std::vector<DataCell> row : *rows)
	{
		int id = row[0].getInt();
		resultList.push_back(id);
	}
	return resultList;
}

std::vector<TimeEntry> TimeAccessor::getDetailTimeList(int64_t taskID, time_t startTime, time_t stopTime)
{
	vector<TimeEntry> resultList;
	stringstream statement;
	statement << "SELECT id, start, stop, deleted, running, changed, uuid, taskUUID FROM v_times ";
	statement << " WHERE stop > " << startTime;
	statement << " AND start <" << stopTime;
	statement << " AND taskID = " << taskID;
	statement << " AND deleted=0 ";

	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	for (vector<DataCell> row : *rows)
	{
		int64_t id = row[0].getInt();
		time_t start = row[1].getInt();
		time_t stop = row[2].getInt();
		bool deleted = row[3].getBool();
		bool running = row[4].getBool();
		time_t changed = row[5].getInt();
		std::string uuid = row[6].getString();
		std::string taskUUID = row[7].getString();
		resultList.push_back(TimeEntry(id, uuid, taskID, taskUUID, start, stop, deleted, running, changed));
	}
	return resultList;
}

void TimeAccessor::setRunning(int64_t timeID, bool running)
{
	TimeEntry te = getByID(timeID);
	int64_t taskID = te.taskID();
	stringstream statement;
	statement << "UPDATE times SET running = " << (int) running;
	statement << " WHERE id=" << timeID;

	db->exe(statement.str());
	notifier->sendNotification(TASK_UPDATED, taskID);
}

std::shared_ptr<std::vector<TimeEntry> > TimeAccessor::getTimesChangedSince(time_t timestamp)
{
	std::shared_ptr<std::vector<TimeEntry> > result = shared_ptr<std::vector<TimeEntry>>(new vector<TimeEntry>);

	std::shared_ptr<Statement> statement = db->prepare("SELECT taskID, start, stop, running, changed, deleted, uuid, id, taskUUID FROM v_times WHERE changed>=?");

	statement->bindValue(1, timestamp);

	std::shared_ptr<QueryResult> rows = statement->execute();
	for (vector<DataCell> row : *rows)
	{
		int taskID = row[0].getInt();
		time_t start = row[1].getInt();
		time_t stop = row[2].getInt();
		bool running = row[3].getBool();
		time_t changed = row[4].getInt();
		bool deleted = row[5].getBool();
		std::string uuid = "";
		if (row[6].hasValue())
		{
			uuid = row[6].getString();
		}
		int64_t id = row[7].getInt();
		std::string taskUUID = row[8].getString();
		TimeEntry te(id, uuid, taskID, taskUUID, start, stop, deleted, running, changed);
		result->push_back(te);
	}
	return result;
}

int64_t TimeAccessor::uuidToId(std::string uuid)
{
	int64_t id = 0;
	std::shared_ptr<DBAbstraction::Statement> statement_uuidToId = db->prepare("SELECT id FROM times WHERE uuid=?;");
	statement_uuidToId->bindValue(1, uuid);
	std::shared_ptr<QueryResult> rows = statement_uuidToId->execute();
	for (std::vector<DataCell> row : *rows)
	{
		id = row[0].getInt();
	}
	return id;
}

bool TimeAccessor::update(const TimeEntry &item)
{
	int64_t id = item.ID();
	TimeEntry existingItem = getByID(id);
	if (item != existingItem && item.changed() >= existingItem.changed())
	{
		std::shared_ptr<DBAbstraction::Statement> statement_updateTime = db->prepare(
				"UPDATE times SET uuid=?, taskID=?, start=?, stop=?, running=?, changed=?, deleted=? WHERE id=?");
		statement_updateTime->bindValue(1, item.UUID());
		statement_updateTime->bindValue(2, item.taskID());
		statement_updateTime->bindValue(3, item.start());
		statement_updateTime->bindValue(4, item.stop());
		statement_updateTime->bindValue(5, item.running());
		statement_updateTime->bindValue(6, item.changed());
		statement_updateTime->bindValue(7, item.deleted());
		statement_updateTime->bindValue(8, item.ID());

		statement_updateTime->execute();

		notifier->sendNotification(TASK_UPDATED, item.taskID());
		return true;
	}
	return false;
}
shared_ptr<vector<int64_t>> TimeAccessor::getChildrenIDs(int64_t taskID)
{
	shared_ptr<vector<int64_t>> result = shared_ptr<vector<int64_t> >(new vector<int64_t>);
	shared_ptr<Statement> statement_getChildrenIDs = db->prepare("SELECT id FROM tasks WHERE parent=?;");
	statement_getChildrenIDs->bindValue(1, taskID);
	shared_ptr<QueryResult> rows = statement_getChildrenIDs->execute();
	for (vector<DataCell> row : *rows)
	{
		int64_t id = row[0].getInt();
		result->push_back(id);
	}
	return result;
}

time_t TimeAccessor::getTotalTimeWithChildren(int64_t taskID, time_t start, time_t stop)
{
	time_t totalTime = getTime(taskID, start, stop);
	shared_ptr<vector<int64_t>> children = getChildrenIDs(taskID);
	for (int64_t child : *children)
	{
		totalTime += getTotalTimeWithChildren(child, start, stop);
	}
	return totalTime;
}

int64_t TimeAccessor::newEntry(const TimeEntry &item)
{
	std::shared_ptr<DBAbstraction::Statement> statement_newEntry = db->prepare(
			"INSERT INTO times (uuid,taskID, start, stop, changed,deleted) VALUES (?,?,?,?,?,?)");
	statement_newEntry->bindValue(1, item.UUID());
	statement_newEntry->bindValue(2, item.taskID());
	statement_newEntry->bindValue(3, item.start());
	statement_newEntry->bindValue(4, item.stop());
	statement_newEntry->bindValue(5, item.changed());
	statement_newEntry->bindValue(6, item.deleted());
	statement_newEntry->execute();

	if (item.start() != item.stop())
	{
		notifier->sendNotification(TASK_UPDATED, item.taskID());
	}
	return db->getIDOfLastInsert();
}

void TimeAccessor::upgradeToDB5()
{
	time_t now = time(0);

	db->exe("DELETE FROM times WHERE taskID = 0");
	db->exe("ALTER TABLE times RENAME TO times_backup");
	createTable();
	createViews();

	shared_ptr<Statement> statement = db->prepare("SELECT id, taskID, start, stop FROM  times_backup");
	shared_ptr<QueryResult> rows = statement->execute();
	for (vector<DataCell> row : *rows)
	{
		int64_t id = row[0].getInt();
		int64_t taskID = row[1].getInt();
		time_t start = row[2].getInt();
		time_t stop = row[3].getInt();
		string uuid = UUIDTool::randomUUID();
		TimeEntry item(id, uuid, taskID, "", start, stop, false, false, now);
		newEntry(item);
	}
}

void TimeAccessor::createTable()
{
	db->exe("CREATE TABLE IF NOT EXISTS times "
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
	db->exe("DROP VIEW IF EXISTS v_times");

	db->exe("CREATE VIEW v_times AS"
			" SELECT times.*, tasks.uuid AS taskUUID "
			" FROM times "
			" LEFT JOIN tasks "
			" ON times.taskID = tasks.id");
}

void TimeAccessor::removeShortTimeSpans()
{
	db->exe("DELETE FROM times WHERE stop-start < 30");
}

std::vector<int64_t> TimeAccessor::getActiveTasks(time_t start, time_t stop)
{
	std::vector<int64_t> resultList;
	std::shared_ptr<Statement> statement_getTasks =
			db->prepare(
					"SELECT DISTINCT times.taskid FROM times JOIN tasks ON times.taskID = tasks.id AND tasks.deleted='0' WHERE (times.start>=? AND times.start<=?) OR (times.stop>=? AND times.stop<=?) OR (times.start<? AND times.stop>?)  AND times.deleted=0;");
	statement_getTasks->bindValue(1, start);
	statement_getTasks->bindValue(2, stop);
	statement_getTasks->bindValue(3, start);
	statement_getTasks->bindValue(4, stop);
	statement_getTasks->bindValue(5, start);
	statement_getTasks->bindValue(6, stop);

	std::shared_ptr<QueryResult> rows = statement_getTasks->execute();

	for (std::vector<DataCell> row : *rows)
	{
		int id = row[0].getInt();
		resultList.push_back(id);
	}
	return resultList;
}

}
