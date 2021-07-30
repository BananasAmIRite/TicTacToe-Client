#include "MainFrame.h"
#include "TicTacToeInput.h"

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, wxS("Tic Tac Toe")) {
	Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

	CreateStatusBar(); 

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	displayPanel = new DisplayPanel(this); 
	wxStaticText* text = new wxStaticText(this, -1, "");
	text->SetForegroundColour(*wxRED); 
	TicTacToeInput* input = new TicTacToeInput(this, displayPanel, text);

	sizer->Add(displayPanel, 0, wxEXPAND | wxSHAPED , 10); 
	sizer->Add(text); 
	sizer->Add(input);
	displayPanel->Show(); 

	SetSizer(sizer); 

	// Centre(); 
	// wxPanel panel = new wxPanel(this); 

	Game* game = new Game; 

	displayPanel->start(game);
	game->setPlayerType("X"); 
	displayPanel->move("11"); 
	displayPanel->move("12");
	displayPanel->move("23");

	// displayPanel->end("TIE"); 
}

void MainFrame::OnExit(wxCommandEvent& evt) {
	Close(true); 
}