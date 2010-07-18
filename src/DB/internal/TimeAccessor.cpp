#include "TimeAccessor.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace DBAbstraction;

TimeAccessor::TimeAccessor(const std::string& dbname, boost::shared_ptr<TaskAccessor>& taskAccessor) : db(dbname)
{
	m_taskAccesor = taskAccessor;
	//Disable all previously running tasks
	stringstream statement;
	statement << "UPDATE times SET running = 0" ;
	try
	{
		db.exe(statement.str());
		statement.str("");

		//Remove short times;
		statement << "DELETE FROM times WHERE stop < (start + 60)";
		db.exe(statement.str());

	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
}


int64_t TimeAccessor::newTime(int64_t taskID, time_t startTime, time_t stopTime)
{
	stringstream statement;
	int64_t id;
	statement << "INSERT INTO times (taskID, start, stop) VALUES (" << taskID << ", " << startTime << ", " << stopTime << ")";
	try
	{
		db.exe(statement.str());
		id = db.getIDOfLastInsert();
		if(startTime != stopTime)
		{
			m_taskAccesor->taskUpdated(taskID);
		}
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
	return id;
}

void TimeAccessor::changeEndTime(int64_t timeID, time_t stopTime)
{
	TimeEntry te = getByID(timeID);
	stringstream statement;
	statement << "UPDATE times SET stop=" << stopTime;
	statement << " WHERE id=" << timeID;
	try
	{
		db.exe(statement.str());
		m_taskAccesor->taskUpdated(te.taskID);
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
}

void TimeAccessor::changeStartTime(int64_t timeID, time_t startTime)
{
	TimeEntry te = getByID(timeID);
	stringstream statement;
	statement << "UPDATE times SET start = " << startTime;
	statement << " WHERE id=" << timeID;
	try
	{
		db.exe(statement.str());
		m_taskAccesor->taskUpdated(te.taskID);
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
}

void TimeAccessor::updateTime(int64_t timeID, time_t startTime, time_t stopTime)
{
	TimeEntry te = getByID(timeID);
	stringstream statement;
	statement << "UPDATE times SET start = " << startTime << ", stop=" << stopTime;
	statement << " WHERE id=" << timeID;
	try
	{
		db.exe(statement.str());
		m_taskAccesor->taskUpdated(te.taskID);
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
}

void TimeAccessor::remove(int64_t id)
{
	TimeEntry te = getByID(id);
	stringstream statement;
	statement << "DELETE FROM times WHERE id=" << id;
	try
	{
		db.exe(statement.str());
		m_taskAccesor->taskUpdated(te.taskID);
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
}

TimeEntry TimeAccessor::getByID(int64_t id)
{
	stringstream statement;
	statement << "SELECT id, taskID, start, stop, running FROM times ";
	statement << " WHERE id == " << id;

	TimeEntry item;
	item.id = 0;
	try
	{
		db.exe(statement.str());
		std::vector< std::vector<DataCell> >::iterator iter=db.rows.begin();
		for( ;iter!=db.rows.end();iter++)
		{
			vector<DataCell> row=*iter;
			item.id = row[0].getInt();
			item.taskID = row[1].getInt();
			item.start = row[2].getInt();
			item.stop = row[3].getInt();
			item.running = row[4].getBool();
		}
	}
	catch(dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
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
	try
	{
		db.exe(statement.str());

		vector< vector<DataCell> >::iterator iter=db.rows.begin();
		for(;iter!=db.rows.end();iter++)
		{
			vector<DataCell> row=*iter;
			id = row[0].getInt();
			start = row[1].getInt();
			stop = row[2].getInt();
			name = row[3].getString();
			if(start<startTime)
			{
				start=startTime;
			}
			if(stop>stopTime)
			{
				stop=stopTime;
			}
			if( resultList.find(id) != resultList.end())
			{
				resultList[id].duration += (stop-start);
			}
			else
			{
				taskTime.duration = (stop-start);
				taskTime.name = name;
				resultList[id] = taskTime;
			}
		}
	}
	catch(dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
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
	try
	{
		db.exe(statement.str());
		vector< vector<DataCell> >::iterator iter= db.rows.begin();
		for(; iter< db.rows.end();iter++)
		{
			vector<DataCell> row=*iter;
			item.id = row[0].getInt();
			item.start = row[1].getInt();
			item.stop = row[2].getInt();
			resultList.push_back(item);
		}
	}
	catch(dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
	return resultList;
}

void TimeAccessor::setRunning(int64_t timeID, bool running)
{
	TimeEntry te = getByID(timeID);
	stringstream statement;
	statement << "UPDATE times SET running = " << (int)running;
	statement << " WHERE id=" << timeID;
	try
	{
		db.exe(statement.str());
		//ENHANCEMENT filter short times!
		m_taskAccesor->setTaskRunning(te.taskID, running);
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
}
