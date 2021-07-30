#include "Game.h"

Game::Game() : plrType("") {
	setupBoard(); 
}

void Game::setupBoard() {
	board = new string * [BOARD_SIZE_Y]; 
	for (int i = 0; i < BOARD_SIZE_Y; ++i) {
		board[i] = new string [BOARD_SIZE_X];
		for (int j = 0; j < BOARD_SIZE_X; ++j) {
			board[i][j] = ""; 
		}
	}
}

string Game::setValue(string position, string plr) {
		int valY = (position[0] - '0') - 1; // force cast: '1' -> 1, '2' -> 2, etc
		int valX = (position[1] - '0') - 1;

		if (valY < 0 || valY > 2 || valX < 0 || valY > 2) return "An error occurred when parsing the text. "; 

		board[valY][valX] = plr;
		if (moveHandler != NULL) moveHandler(position, plr);

	return ""; 
}

void Game::setMoveHandler(function<void(string, string)> handler) {
	moveHandler = handler; 
}

void Game::setPlayerType(string plr) {
	if (plrType != "" || (plr != "X" && plr != "O")) return;
	plrType = plr; 
}

string Game::getPlayerType() {
	return plrType;
}

string** Game::getBoard() {
	return board; 
}