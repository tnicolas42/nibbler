#include "Game.hpp"
#include "nibbler.hpp"

Game::Game() :
  _dynGuiManager(),
  _gameInfo(nullptr),
  _snake(),
  _speedMs(SPEED) {}

bool Game::init(uint16_t width, uint16_t height, uint8_t boardSize) {
	_gameInfo = new GameInfo();
	_gameInfo->width = width;
	_gameInfo->height = height;
	_gameInfo->boardSize = boardSize;
	_gameInfo->rules.canExitBorder = true;
	try {
		_dynGuiManager.loadGui(0);
		_dynGuiManager.nibblerGui->init(_gameInfo);
	}
	catch(DynGuiManager::DynGuiManagerException const & e) {
		logErr(e.what());
		return false;
	}

	_snake.push_back({3, 5});
	_snake.push_back({3, 6});
	_snake.push_back({3, 7});
	_snake.push_back({3, 8});
	_snake.push_back({3, 9});
	_snake.push_back({3, 10});
	_snake.push_back({3, 11});
	_snake.push_back({3, 12});
	_snake.push_back({3, 13});
	_snake.push_back({3, 14});
	_snake.push_back({3, 15});
	_snake.push_back({3, 16});
	_snake.push_back({3, 17});
	_snake.push_back({3, 18});
	_snake.push_back({3, 19});
	return true;
}

Game::Game(Game const &src) {
	*this = src;
}

Game::~Game() {
	delete _gameInfo;
}

Game &Game::operator=(Game const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void Game::run() {
	float						loopTime = 1000 / FPS;
	std::chrono::milliseconds	time_start;
	uint32_t					lastMoveTime = 0;
	ANibblerGui					*nibblerGui = nullptr;
	#if DEBUG_FPS_LOW == true
		bool firstLoop = true;
	#endif

	nibblerGui = _dynGuiManager.nibblerGui;

	while (nibblerGui->input.quit == false) {
		time_start = getMs();

		nibblerGui->updateInput();

		// update game
		_update();

		// verify id viability
		if (nibblerGui->input.loadGuiID < NB_GUI && \
		nibblerGui->input.loadGuiID != _dynGuiManager.getCurrentGuiID()) {
			// change Gui
			_dynGuiManager.loadGui(nibblerGui->input.loadGuiID);
			nibblerGui = _dynGuiManager.nibblerGui;
			nibblerGui->init(_gameInfo);

			nibblerGui->input.loadGuiID = NO_GUI_LOADED;
		}

		// move snake
		uint32_t now = getMs().count();
		if (_gameInfo->paused == false && now - lastMoveTime > _speedMs) {
			_move(_gameInfo->direction);
			lastMoveTime = now;
		}

		// draw on screen
		nibblerGui->draw(_snake);

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

void Game::_move(Direction::Enum direction) {
	int addX = 0;
	int addY = 0;
	if (direction == Direction::MOVE_UP)
		addY--;
	else if (direction == Direction::MOVE_DOWN)
		addY++;
	else if (direction == Direction::MOVE_LEFT)
		addX--;
	else if (direction == Direction::MOVE_RIGHT)
		addX++;

	if (_snake.size() > 0) {
		Snake newSnake(_snake[0].x + addX, _snake[0].y + addY);
		if (_gameInfo->rules.canExitBorder) {
			if (newSnake.x < 0) newSnake.x = _gameInfo->boardSize - 1;
			else if (newSnake.x >= _gameInfo->boardSize) newSnake.x = 0;
			if (newSnake.y < 0) newSnake.y = _gameInfo->boardSize - 1;
			else if (newSnake.y >= _gameInfo->boardSize) newSnake.y = 0;
		}
		_snake.push_front(newSnake);
		_snake.pop_back();
	}
}

void Game::_update() {
	// update win
	if (_snake.size() >= _gameInfo->boardSize * _gameInfo->boardSize) {
		_gameInfo->win = true;
	}
	// update gameOver
	if (_snake.size() == 0) {
		_gameInfo->gameOver = true;
	}
	else if (_snake[0].x < 0 || _snake[0].x >= _gameInfo->boardSize
	|| _snake[0].y < 0 || _snake[0].y >= _gameInfo->boardSize) {
		_gameInfo->gameOver = true;
	}

	// update paused mode
	if (_gameInfo->win || _gameInfo->gameOver) {
		_gameInfo->paused = true;
	}
	else {
		_gameInfo->paused = _dynGuiManager.nibblerGui->input.paused;
	}

	// update direction
	if (_gameInfo->direction != _dynGuiManager.nibblerGui->input.direction) {
		if (_snake.size() <= 1) {
			_gameInfo->direction = _dynGuiManager.nibblerGui->input.direction;
		}
		else {
			Snake direction(_snake[0].x - _snake[1].x, _snake[0].y - _snake[1].y);
			if (direction.x > 1) direction.x = -1;
			else if (direction.x < -1) direction.x = 1;
			if (direction.y > 1) direction.y = -1;
			else if (direction.y < -1) direction.y = 1;

			if (_dynGuiManager.nibblerGui->input.direction == Direction::MOVE_UP && direction.y != 1)
				_gameInfo->direction = _dynGuiManager.nibblerGui->input.direction;
			else if (_dynGuiManager.nibblerGui->input.direction == Direction::MOVE_DOWN && direction.y != -1)
				_gameInfo->direction = _dynGuiManager.nibblerGui->input.direction;
			else if (_dynGuiManager.nibblerGui->input.direction == Direction::MOVE_LEFT && direction.x != 1)
				_gameInfo->direction = _dynGuiManager.nibblerGui->input.direction;
			else if (_dynGuiManager.nibblerGui->input.direction == Direction::MOVE_RIGHT && direction.x != -1)
				_gameInfo->direction = _dynGuiManager.nibblerGui->input.direction;
		}
	}
}

// -- Exceptions errors --------------------------------------------------------
Game::GameException::GameException()
: std::runtime_error("[GameException]") {}

Game::GameException::GameException(const char* what_arg)
: std::runtime_error(std::string(std::string("[GameException] ") + what_arg).c_str()) {}
