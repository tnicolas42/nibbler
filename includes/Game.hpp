#pragma once

#include "ANibblerGui.hpp"
#include "ANibblerSound.hpp"
#include "DynManager.hpp"

class Game {
	public:
		Game();
		Game(Game const &src);
		virtual ~Game();

		Game &operator=(Game const &rhs);

		bool	init();
		void	run();
		void	restart();

		DynManager<ANibblerSound>		dynSoundManager;
		DynManager<ANibblerGui>			dynGuiManager;

		class GameException : public std::runtime_error {
			public:
				GameException();
				explicit GameException(const char* what_arg);
		};

	private:
		GameInfo *						_gameInfo;
		std::vector<uint8_t>			_needExtend;
		std::vector<Vec2>				_lastDeletedSnake;
		std::deque<Vec2>				_wall;
		uint32_t						_speedMs;

		void				_move(Direction::Enum direction, int id);
		void				_moveIA(Direction::Enum lastDir, int id);
		void				_updateFood();
		void				_updateBonus();
		void				_updateWall();
		void				_update();
		void				_updateSinglePlayer();
		void				_updateMultiPlayer();
		void				_changeGui(int guiID, int soundID);
};
