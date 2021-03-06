#pragma once

#ifndef DEBUG
	#define DEBUG false
#endif
#define DEBUG_FPS_LOW	DEBUG & false

#include "SettingsJson.hpp"

void						initLogs();
bool						initSettings(std::string const & filename);
bool						initUserData(std::string const & filename);
bool						saveUserData(std::string const & filename);
bool						usage();
bool						argparse(int nbArgs, char const **args);
std::chrono::milliseconds	getMs();

extern SettingsJson s;
extern SettingsJson userData;
