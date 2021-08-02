#ifndef DISPLAYPANEL_H
#define DISPLAYPANEL_H
#include "wxPrecomp.h"
#include "Game.h"
#include "session.h"

enum class DisplayState {
	QUEUE, 
	GAME_END, 
	GAME, 
	CONNECTING
};

class DisplayPanel: public wxPanel {
public: 
	DisplayPanel(wxFrame*); 
	~DisplayPanel(); 
	string move(string); // sends a move to server; returns an error message to display if unsuccessful, otherwise empty string
	//
	void start(Game*); // Initiates game start; 
	void end(string); // Displays game end; takes in "X", "O", or "TIE"; This will also deallocate the Game pointer
	void queue(); // displays a queue screen; Will not be used yet
	void onPaintEvent(wxPaintEvent&); 
	void setDisplayState(DisplayState); 
	void onResize(wxSizeEvent&); 
	void connect(); 
private:
	Game* game = NULL; 
	DisplayState displayState; 
	shared_ptr<session> wsSession; 
	wxFrame* parentFrame; 
	net::io_context ioc; 
	void drawLetter(int, int, wxPaintDC&, string letter);
	void drawLines(wxPaintDC&);
	void handleMessage(beast::error_code, std::size_t, beast::flat_buffer);
	void handleFail(beast::error_code, char const*); 
	string outcome; // outcome of a match is stored to render
	future<void> f_ioc; 
	//void setValue(string, string); // sets a value LOCALLY; This will pretty much always succeed unless there is no Game pointer available
};

#endif

