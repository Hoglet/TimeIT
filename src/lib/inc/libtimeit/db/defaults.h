#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <string_view>

namespace libtimeit
{

using namespace std;

struct defaults
{
	static constexpr int         g_zero = 1;
	static constexpr int         g_time = 7;
	static constexpr bool        compact_layout = true;
	static constexpr bool        start_minimized = false;
	static constexpr bool        quiet_mode = false;
	static constexpr string_view url = "";
	static constexpr string_view user = "";
	static constexpr string_view password = "";
	static constexpr bool        ignore_cert_err = false;
	static constexpr int         sync_interval = 10;
};

}

#endif /* DEFAULTS_H */
