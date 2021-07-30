#ifndef TICTACTOEINPUT_H
#define TICTACTOEINPUT_H
#include "GameInput.h"
#include "DisplayPanel.h"

class TicTacToeInput : public GameInput {
	public:
		TicTacToeInput(wxWindow*, DisplayPanel*, wxStaticText*);
	private:
		DisplayPanel* dPanel;
		wxStaticText* errorText; 
		void onEvent(wxCommandEvent&, GameInput*);
};

#endif

