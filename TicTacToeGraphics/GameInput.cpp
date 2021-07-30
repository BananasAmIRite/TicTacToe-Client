#include "GameInput.h"
GameInput::GameInput() {}
GameInput::GameInput(wxWindow* parent, wxWindowID id,
    const wxString& value,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name): wxTextCtrl(parent, id, value, pos, size, style | wxTE_PROCESS_ENTER, validator) {
    Bind(wxEVT_TEXT_ENTER, &GameInput::onEnterEvent, this, wxID_ANY);
}

void GameInput::setEnterHandler(function<void(wxCommandEvent&, GameInput*)> handler) {
    enterHandler = handler; 
}

void GameInput::onEnterEvent(wxCommandEvent& evt) {
    if (enterHandler) enterHandler(evt, this); 
}