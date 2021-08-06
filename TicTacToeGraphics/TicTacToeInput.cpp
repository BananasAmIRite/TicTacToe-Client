#include "TicTacToeInput.h"

TicTacToeInput::TicTacToeInput(wxWindow* parent, DisplayPanel* panel, wxStaticText* errText = NULL): GameInput(parent, wxID_ANY), dPanel(panel), errorText(errText) {
	setEnterHandler([this](wxCommandEvent& evt, GameInput* input) {
		onEvent(evt, input); 
	});
}
void TicTacToeInput::onEvent(wxCommandEvent& evt, GameInput* input) {
	string line1 = GetLineText(0); // first line of text
	ChangeValue(""); 
	string err = dPanel->sendMove(line1); 
	if (errorText != NULL) errorText->SetLabelText(err); 
}