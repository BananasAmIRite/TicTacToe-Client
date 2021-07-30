#ifndef GAMEINPUT_H
#define GAMEINPUT_H
#include "wxPrecomp.h"
class GameInput : public wxTextCtrl {
public: 
	GameInput(); 
    GameInput(wxWindow*, wxWindowID, // just copied everything from wxTextCtrl
        const wxString& = wxEmptyString,
        const wxPoint& = wxDefaultPosition,
        const wxSize& = wxDefaultSize,
        long = 0,
        const wxValidator& = wxDefaultValidator,
        const wxString& = wxASCII_STR(wxTextCtrlNameStr));
	void setEnterHandler(function<void(wxCommandEvent&, GameInput*)>);
private:
	function<void(wxCommandEvent&, GameInput*)> enterHandler;
    void onEnterEvent(wxCommandEvent&); 

};

#endif

