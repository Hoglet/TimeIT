#include "TimeAccessor.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace DBAbstraction;

TimeAccessor::TimeAccessor(const std::string& dbname, boost::shared_ptr<Notifier>& notifier) :
		db(dbname)
{
	this->notifier = notifier;
	//Disable all previously running tasks
	stringstream statement;
	statement << "UPDATE times SET running = 0";
	db.exe(statement.str());
	statement.str("");

	//Remove short times;
	statement << "DELETE FROM times WHERE stop < (start + 60)";
	db.exe(statement.str());

}

int64_t TimeAccessor::newTime(int64_t taskID, time_t startTime, time_t stopTime)
{
	stringstream statement;
	int64_t id;
	statement << "INSERT INTO times (taskID, start, stop) VALUES (" << taskID << ", " << startTime << ", " << stopTime
			<< ")";
	db.exe(statement.str());
	id = db.getIDOfLastInsert();
	if (startTime != stopTime)
	{
		notifier->taskUpdated(taskID);
	}
	return id;
}

void TimeAccessor::changeEndTime(int64_t timeID, time_t stopTime)
{
	TimeEntry te = getByID(timeID);
	stringstream statement;
	statement << "UPDATE times SET stop=" << stopTime;
	statement << " WHERE id=" << timeID;
	db.exe(statement.str());
	notifier->taskUpdated(te.taskID);
}

void TimeAccessor::changeStartTime(int64_t timeID, time_t startTime)
{
	TimeEntry te = getByID(timeID);
	stringstream statement;
	statement << "UPDATE times SET start = " << startTime;
	statement << " WHERE id=" << timeID;
	db.exe(statement.str());
	notifier->taskUpdated(te.taskID);
}

void TimeAccessor::updateTime(int64_t timeID, time_t startTime, time_t stopTime)
{
	TimeEntry te = getByID(timeID);
	stringstream statement;
	statement << "UPDATE times SET start = " << startTime << ", stop=" << stopTime;
	statement << " WHERE id=" << timeID;
	db.exe(statement.str());
	notifier->taskUpdated(te.taskID);
}

void TimeAccessor::remove(int64_t id)
{
	TimeEntry te = getByID(id);
	stringstream statement;
	statement << "DELETE FROM times WHERE id=" << id;
	db.exe(statement.str());
	notifier->taskUpdated(te.taskID);
}

TimeEntry TimeAccessor::getByID(int64_t id)
{
	stringstream statement;
	statement << "SELECT id, taskID, start, stop, running FROM times ";
	statement << " WHERE id == " << id;

	TimeEntry item;
	item.id = 0;
	db.exe(statement.str());
	std::vector<std::vector<DataCell> >::iterator iter = db.rows.begin();
	for (; iter != db.rows.end(); iter++)
	{
		vector<DataCell> row = *iter;
		item.id = row[0].getInt();
		item.taskID = row[1].getInt();
		item.start = row[2].getInt();
		item.stop = row[3].getInt();
		item.running = row[4].getBool();
	}
	return item;
}

map<int64_t, TaskTime> TimeAccessor::getTimeList(time_t startTime, time_t stopTime)
{
	map<int64_t, TaskTime> resultList;
	stringstream statement;
	statement << "SELECT times.taskID, times.start, times.stop, tasks.name FROM times, tasks ";
	statement << " WHERE times.stop > " << startTime;
	statement << " AND times.start <" << stopTime;
	statement << " AND times.taskID = tasks.id ";
	statement << " AND tasks.deleted = 'false'";

	int id;
	int start;
	int stop;
	std::string name;
	TaskTime taskTime;
	db.exe(statement.str());

	vector<vector<DataCell> >::iterator iter = db.rows.begin();
	for (; iter != db.rows.end(); iter++)
	{
		vector<DataCell> row = *iter;
		id = row[0].getInt();
		start = row[1].getInt();
		stop = row[2].getInt();
		name = row[3].getString();
		if (start < startTime)
		{
			start = startTime;
		}
		if (stop > stopTime)
		{
			stop = stopTime;
		}
		if (resultList.find(id) != resultList.end())
		{
			resultList[id].duration += (stop - start);
		}
		else
		{
			taskTime.duration = (stop - start);
			taskTime.name = name;
			resultList[id] = taskTime;
		}
	}
	return resultList;
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

int TimeAccessor::getTimeCompletelyWithinLimits(int64_t & taskID, time_t & start, time_t & stop)
{
	int time = 0;
	stringstream statement;
	statement << "SELECT SUM(stop-start) AS time ";
	statement << " FROM times";
	statement << " WHERE taskID = " << taskID;
	if (stop > 0)
	{
		statement << "  AND start >=" << start;
		statement << "  AND stop  <= " << stop;
	}
	db.exe(statement.str());
	if (db.rows.size() == 1)
	{
		vector<DataCell> row = db.rows[0];
		if (row[0].isNull() == false)
		{
			time = row[0].getInt();
		}
	}

	return time;
}

int TimeAccessor::getTimePassingEndLimit(int64_t & taskID, time_t & start, time_t & stop)
{
	int time = 0;
	stringstream statement;
	statement << " SELECT SUM(" << stop << "-start) AS time  ";
	statement << " FROM times ";
	statement << " WHERE taskID = " << taskID;
	statement << " AND start < " << stop;
	statement << " AND stop  > " << stop;
	statement << " AND start > " << start;
	db.exe(statement.str());
	if (db.rows.size() == 1)
	{
		vector<DataCell> row = db.rows[0];
		if (row[0].isNull() == false)
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
	db.exe(statement.str());
	if (db.rows.size() == 1)
	{
		vector<DataCell> row = db.rows[0];
		if (row[0].isNull() == false)
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
	statement << "SELECT DISTINCT times.taskid FROM times  ORDER BY times.stop DESC LIMIT " << amount;

	int id;
	db.exe(statement.str());

	vector<vector<DataCell> >::iterator iter = db.rows.begin();
	for (; iter != db.rows.end(); iter++)
	{
		vector<DataCell> row = *iter;
		id = row[0].getInt();
		resultList.push_back(id);
	}
	return resultList;

}

std::vector<TimeEntry> TimeAccessor::getDetailTimeList(int64_t taskId, time_t startTime, time_t stopTime)
{
	vector<TimeEntry> resultList;
	stringstream statement;
	statement << "SELECT id, start, stop FROM times ";
	statement << " WHERE stop > " << startTime;
	statement << " AND start <" << stopTime;
	statement << " AND taskID = " << taskId;

	TimeEntry item;
	db.exe(statement.str());
	vector<vector<DataCell> >::iterator iter = db.rows.begin();
	for (; iter < db.rows.end(); iter++)
	{
		vector<DataCell> row = *iter;
		item.id = row[0].getInt();
		item.start = row[1].getInt();
		item.stop = row[2].getInt();
		resultList.push_back(item);
	}
	return resultList;
}

void TimeAccessor::setRunning(int64_t timeID, bool running)
{
	//TODO: Remove duplication of data. "Running" is in two tables
	TimeEntry te = getByID(timeID);
	int64_t taskID = te.taskID;
	stringstream statement;
	statement << "UPDATE times SET running = " << (int) running;
	statement << " WHERE id=" << timeID;

	stringstream statement2;
	statement2 << "UPDATE tasks SET running = " << (int) running;
	statement2 << " WHERE id=" << taskID;

	db.exe(statement.str());
	db.exe(statement2.str());
	notifier->taskUpdated(taskID);
}
