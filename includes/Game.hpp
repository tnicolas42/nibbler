#pragma once

#include "DynGuiManager.hpp"

class Game {
	public:
		Game();
		Game(Game const &src);
		virtual ~Game();

		Game &operator=(Game const &rhs);

		bool	init();
		void	run();
		void	restart();

		class GameException : public std::runtime_error {
			public:
				GameException();
				explicit GameException(const char* what_arg);
		};

	private:
		DynGuiManager					_dynGuiManager;
		GameInfo *						_gameInfo;
		std::vector<std::deque<Vec2>>	_snake;
		std::vector<uint8_t>			_needExtend;
		std::vector<Vec2>				_lastDeletedSnake;
		std::deque<Vec2>				_food;
		uint32_t						_speedMs;

		void				_move(Direction::Enum direction, int id);
		void				_updateFood();
		void				_update();
		void				_updateSinglePlayer();
		void				_updateMultiPlayer();
};
