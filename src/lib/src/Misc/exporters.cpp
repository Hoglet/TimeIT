#include "libtimeit/misc/exporters.h"
#include <fmt/core.h>

#include <cstdio>
#include <libtimeit/utils.h>
#include <libtimeit/misc/file.h>

namespace libtimeit
{

bool export_to_csv( database& db , const string &filename, time_point<system_clock> report_start, time_point<system_clock> report_end )
{
	task_accessor tasks(db);
	time_accessor times(db);
	auto items = tasks.by_parent_id();
	try
	{
		file output( filename, "w" );
		for (auto item: items)
		{
			fmt::print( output, ",{}", item.name);
		}
		fmt::print( output , "\n");
		auto day_pointer = report_start;
		while ( day_pointer < report_end)
		{
			auto start = beginning_of_day(day_pointer);
			auto stop  = end_of_day(day_pointer);

			auto date = date_string(day_pointer);

			fmt::print( output , fmt::runtime(date) );
			for (auto item: items)
			{
				auto duration = times.duration_time( item.id, start, stop );
				auto duration_str = hh_mm( seconds(duration) );
				fmt::print( output, ",{}", duration_str);
			}
			fmt::print( output, "\n");
			day_pointer += 24h;
		}
		return true;
	}
	catch( ... )
	{
	}
	return false;
}




}
