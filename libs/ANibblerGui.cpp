#include "ANibblerGui.hpp"

ANibblerGui::ANibblerGui()
: input(),
  _gameInfo(nullptr) {}

ANibblerGui::~ANibblerGui() {
}

ANibblerGui::ANibblerGui(ANibblerGui const &src) {
	*this = src;
}

ANibblerGui &ANibblerGui::operator=(ANibblerGui const &rhs) {
	if (this != &rhs) {
		input = rhs.input;
	}
	return *this;
}

bool ANibblerGui::init(GameInfo * gameInfo) {
	_gameInfo = gameInfo;

	input.paused = _gameInfo->paused;
	input.direction = _gameInfo->direction;

	return _init();
}

// -- GameInfo ------------------------------------------------------------------

GameInfo::GameInfo()
: title("nibbler"),
  width(800),
  height(600),
  boardSize(20),
  rules() {
	rules.canExitBorder = true;
	restart();
}

void GameInfo::restart() {
	direction = Direction::MOVE_UP;
	paused = false;
	win = false;
	gameOver = false;
}

// -- Snake ---------------------------------------------------------------------

Snake::Snake() : x(0), y(0) {}
Snake::Snake(int x_, int y_) : x(x_), y(y_) {}
bool Snake::operator==(Snake const & other) const {
	return x == other.x && y == other.y;
}

// -- Input --------------------------------------------------------------------

ANibblerGui::Input::Input()
: quit(false),
  paused(false),
  direction(Direction::MOVE_UP),
  loadGuiID(NO_GUI_LOADED),
  restart(false) {
}

ANibblerGui::Input::Input(ANibblerGui::Input const &src) {
	*this = src;
}

ANibblerGui::Input &ANibblerGui::Input::operator=(ANibblerGui::Input const &rhs) {
	if (this != &rhs) {
		direction = rhs.direction;
		loadGuiID = rhs.loadGuiID;
	}
	return *this;
}
