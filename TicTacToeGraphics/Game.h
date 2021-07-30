#ifndef GAME_H
#define GAME_H
#include "wxPrecomp.h"

class Game {
public: 
	static const int BOARD_SIZE_X = 3;
	static const int BOARD_SIZE_Y = 3;
	Game(); 
	string setValue(string, string); 
	void setMoveHandler(function<void(string, string)>); // function<void(position, player)>
	void setPlayerType(string);
	string getPlayerType(); 
	string** getBoard(); 
private:
	//string board[BOARD_SIZE_Y][BOARD_SIZE_X];
	string** board;
	string plrType; 
	function<void(string, string)> moveHandler; // handler for when a move has occurred
	void setupBoard(); 
};
#endif
