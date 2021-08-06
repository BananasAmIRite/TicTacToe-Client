#include "DisplayPanel.h"
#include "Utils.h"

void run_ioc(net::io_context* ioc) {
	// net::executor_work_guard<decltype(ioc->get_executor())> work{ ioc->get_executor() };
	ioc->run(); 
}

DisplayPanel::DisplayPanel(wxFrame* parent = NULL): game(NULL), displayState(DisplayState::CONNECTING), wxPanel(parent), parentFrame(parent), ioc() {
	Bind(wxEVT_PAINT, &DisplayPanel::onPaintEvent, this);
	Bind(wxEVT_SIZE, &DisplayPanel::onResize, this); 

	// connect(); 
}
void DisplayPanel::connect() {
	parentFrame->SetStatusText("Connecting...");
	// start the websocket session
	wsSession = make_shared<session>(ioc); 

	// setup events
	wsSession->setHandshakeHandler([this](beast::error_code ec) {
		parentFrame->SetStatusText("Connected!");
		queue(); // set display type to queue, displaying queue message
	});
	wsSession->setFailHandler([this](beast::error_code ec, const char* err) {handleFail(ec, err); }); // lambda cuz I want to retain `this`
	wsSession->setReadHandler([this](beast::error_code ec, size_t bt, boost::beast::flat_buffer buf) {
		handleMessage(ec, bt, buf); 
		});
	
	wsSession->run("localhost", "2000"); 

	f_ioc = async(launch::async, &run_ioc, &ioc); 
}

string DisplayPanel::sendMove(string position) {
	if (game == NULL || game->getPlayerType() == "") return "You are not currently in a game. ";
	wsSession->send("MOVE " + position); 
	//string retVal = game->setValue(position, game->getPlayerType());
	//Refresh(); // Refresh and Update are usually used to completely rerender the panel
	//Update();
	return "";
}

void DisplayPanel::move(string position, string plr) {
	string retVal = game->setValue(position, plr);
	Refresh(); // Refresh and Update are usually used to completely rerender the panel
	Update();
}

void DisplayPanel::queue() {
	setDisplayState(DisplayState::QUEUE); 
}

void DisplayPanel::start() {
	game = new Game;
	setDisplayState(DisplayState::GAME);
}

void DisplayPanel::end(string state) {
	if (state == "X" || state == "O" || state == "TIE") { // three allowed states
		if (state == "TIE") {
			outcome = state;
		} else {
			outcome = state == game->getPlayerType() ? "WIN" : "LOSS"; 
		}

		setDisplayState(DisplayState::GAME_END);
		delete game;
	}
}

void DisplayPanel::onPaintEvent(wxPaintEvent& evt) {
	wxPaintDC dc(this); 
	switch (displayState) {
		case DisplayState::GAME:
		{
			if (game == NULL) break;
			//dc.SetBackground(wxBrush(GetBackgroundColour()));
			//dc.Clear(); 
			drawLines(dc);
			string** board = game->getBoard();
			for (int i = 0; i < game->BOARD_SIZE_Y; ++i) {
				for (int j = 0; j < game->BOARD_SIZE_Y; ++j) {
					string character = *(board[i] + j);
					if (character == "") continue;
					drawLetter(j + 1, i + 1, dc, character);
				}
			}
			//for (int i = 0; i < game->BOARD_SIZE_Y * game->BOARD_SIZE_X; ++i) {
			//		string character = *(board[i]+j);
			//		drawLetter(); 
			//}
			// i'm not sure if you need or don't need to delete the array here
		}
		break; 
		case DisplayState::QUEUE: 
			//dc.SetBackground(wxBrush(GetBackgroundColour())); // uhh idk if this does anything, but ill do it just in case
			//dc.Clear();
			dc.DrawText("In Queue...", 1, 1); 
			break; 
		case DisplayState::CONNECTING: 
			//dc.SetBackground(wxBrush(GetBackgroundColour()));
			//dc.Clear();
			dc.DrawText("Connecting to Server...", 1, 1); 
			break; 
		case DisplayState::GAME_END: 
			dc.DrawText(outcome == "TIE" ? "You have tied. " : outcome == "WIN" ? "You have won. " : "You have lost. ", 1, 1); 
			break; 
		
	}
}

void DisplayPanel::onResize(wxSizeEvent& evt) {
	Refresh(); 
	Update(); 
}

void DisplayPanel::drawLetter(int x, int y, wxPaintDC& ctx, string letter) {
	const float LETTER_BEZEL = 0.1; 
	wxSize SIZE = ctx.GetWindow()->GetClientSize();
	int SIZE_X = SIZE.x;
	int SIZE_Y = SIZE.y;
	double fontPt = (SIZE_Y / 3 - (2 * (double)(SIZE_Y / 3 * (LETTER_BEZEL)))) / 1.333; 
	int windowX = (SIZE_X * x) / 3 - (SIZE_X / 3 * (1 - LETTER_BEZEL));
	int windowY = (SIZE_Y * y) / 3 - (SIZE_Y / 3 * (1-LETTER_BEZEL));
	wxPoint point(windowX, windowY); 
	//parentFrame->SetStatusText("X: " + to_string(windowX) + " Y: " + to_string(windowY) + " oldX: " + to_string(x) + " oldY: " + to_string(y));
	wxFontInfo info(fontPt); 
	const wxFont newFont(info);
	wxFont oldFont = ctx.GetFont(); 

	ctx.SetFont(newFont);
	ctx.DrawText(letter, point);
	ctx.SetFont(oldFont);
}

void DisplayPanel::drawLines(wxPaintDC& ctx) {
	const int WINDOW_BEZEL = 0.5; 
	wxSize SIZE = ctx.GetWindow()->GetClientSize(); 
	int SIZE_X = SIZE.x; 
	int SIZE_Y = SIZE.y; 

	// parentFrame->SetStatusText("X: " + to_string(SIZE_X) + ", Y: " + to_string(SIZE_Y));

	// i THINK these are the correct coords?
	ctx.DrawLine(wxPoint(SIZE_X * 1 / 3, (WINDOW_BEZEL * SIZE_Y)), wxPoint(SIZE_X * 1 / 3, SIZE_Y - (WINDOW_BEZEL * SIZE_Y)));
	ctx.DrawLine(wxPoint(SIZE_X * 2 / 3, (WINDOW_BEZEL * SIZE_Y)), wxPoint(SIZE_X * 2 / 3, SIZE_Y - (WINDOW_BEZEL * SIZE_Y)));
	ctx.DrawLine(wxPoint((WINDOW_BEZEL * SIZE_X), SIZE_Y * 1 / 3), wxPoint(SIZE_X - (WINDOW_BEZEL * SIZE_X), SIZE_Y * 1 / 3));
	ctx.DrawLine(wxPoint((WINDOW_BEZEL * SIZE_X), SIZE_Y * 2 / 3), wxPoint(SIZE_X - (WINDOW_BEZEL * SIZE_X), SIZE_Y * 2 / 3));
}

void DisplayPanel::setDisplayState(DisplayState state) {
	displayState = state; 
	Refresh(); // refresh and update every single time display state is modified
	Update();
}

void DisplayPanel::handleMessage(beast::error_code ec, std::size_t bytes_transferred, beast::flat_buffer buf) {
	parentFrame->SetStatusText("message: " + beast::buffers_to_string(buf.data())); 
	string msg = beast::buffers_to_string(buf.data()); 
	vector<string> splitted = TTT_utils::split(msg, " ");

	string request = splitted.at(0); 

	if (request == "STARTGAME") {
		start(); 
	}
	else if (request == "CALLMOVE") {
		string plr = splitted.at(1);
		string pos = splitted.at(2);

		// parentFrame->SetStatusText("call move called"); 

		move(pos, plr);
	}
	else if (request == "ENDGAME") {
		string winState = splitted.at(1); 
		end(winState); 
	}
	else if (request == "SETPLAYER") {
		string player = splitted.at(1); 
		game->setPlayerType(player); 
	}
}

void DisplayPanel::handleFail(beast::error_code ec,  char const* err) {
	parentFrame->SetStatusText("Error: " + string(err) + " with error code: " + to_string(ec.value()) + ". Reason: " + ec.message());
}

DisplayPanel::~DisplayPanel() {
	ioc.stop(); // stop the io_context event process or else program can't exit :P
}