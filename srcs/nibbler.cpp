#include <unistd.h>
#include <chrono>

#include "nibbler.hpp"
#include "Logging.hpp"

void	initLogs() {
	// init logging
	#if DEBUG
		logging.setLoglevel(LOGDEBUG);
		logging.setPrintFileLine(LOGWARN, true);
		logging.setPrintFileLine(LOGERROR, true);
		logging.setPrintFileLine(LOGFATAL, true);
	#else
		logging.setLoglevel(LOGINFO);
	#endif
	logDebug("using debug mode");
}

std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
}

void	gameLoop(INibblerGui * nibblerGui) {
	float						loopTime = 1000 / FPS;
	std::chrono::milliseconds	time_start;
	Input::eInput				input = Input::NOTHING;
	#if DEBUG_FPS_LOW == true
		bool firstLoop = true;
	#endif

	while (input != Input::QUIT) {
		time_start = getMs();

		input = nibblerGui->getInput();
		nibblerGui->draw();

		// fps
		std::chrono::milliseconds time_loop = getMs() - time_start;
		if (time_loop.count() > loopTime) {
			#if DEBUG_FPS_LOW == true
				if (!firstLoop)
					logDebug("update loop slow -> " << time_loop.count() << "ms / " << loopTime << "ms (" << FPS << "fps)");
			#endif
		}
		else {
			usleep((loopTime - time_loop.count()) * 1000);
		}
		#if DEBUG_FPS_LOW == true
			firstLoop = false;
		#endif
	}
}