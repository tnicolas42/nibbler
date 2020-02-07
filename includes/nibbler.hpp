#pragma once

#ifndef DEBUG
	#define DEBUG false
#endif
#define DEBUG_FPS_LOW	DEBUG & false

#define IA_CHANGE_DIR_PROBA 10

#include "DynGuiManager.hpp"
#include "SettingsJson.hpp"

void						initLogs();
bool						initSettings(std::string const & filename);
bool						initUserData(std::string const & filename);
bool						saveUserData(std::string const & filename);
std::chrono::milliseconds	getMs();

extern SettingsJson s;
extern SettingsJson userData;
