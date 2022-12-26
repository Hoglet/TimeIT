#ifndef DEFAULTS_H
#define DEFAULTS_H

struct defaults
{
	static constexpr auto g_zero = 1;
	static constexpr auto g_time = 7;
	static constexpr auto compact_layout = true;
	static constexpr auto start_minimized = false;
	static constexpr auto quiet_mode = false;
	static constexpr auto url = "";
	static constexpr auto user = "";
	static constexpr auto password = "";
	static constexpr auto ignore_cert_err = false;
	static constexpr auto sync_interval = 10;
} ;


#endif /* DEFAULTS_H */
