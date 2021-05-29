#include <libtimeit/logging.h>
#include <fmt/core.h>
#include <libtimeit/utils.h>

namespace libtimeit
{
using namespace std;

string time_string(time_t time_stamp)
{
	struct tm now = *localtime(&time_stamp);
	return fmt::format("{:02}:{:02}'{:02}",
					now.tm_hour,
					now.tm_min,
					now.tm_sec);
}
void logg(string text)
{
	auto now = time(nullptr);
	auto time_stamp = fmt::format("{} {}", date_string(now), time_string(now) ) ;
	fmt::print( "{} - {}\n", time_stamp, text );
}

}
