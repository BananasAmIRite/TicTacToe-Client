#include "DisplayPanel.h"

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
	});
	wsSession->setFailHandler([this](beast::error_code ec, const char* err) {handleFail(ec, err); }); // lambda cuz I want to retain `this`
	wsSession->setReadHandler([this](beast::error_code ec, size_t bt, boost::beast::flat_buffer buf) {
		parentFrame->SetStatusText("message: " + beast::buffers_to_string(buf.data())); 
		});
	
	wsSession->run("localhost", "2000"); 

	f_ioc = async(launch::async, &run_ioc, &ioc); 
}

string DisplayPanel::move(string position) {
	if (game == NULL || game->getPlayerType() == "") return "You are not currently in a game. ";
	string retVal = game->setValue(position, game->getPlayerType()); 
	Refresh(); // Refresh and Update are usually used to completely rerender the panel
	Update(); 
	return retVal; 
}

void DisplayPanel::queue() {
	setDisplayState(DisplayState::QUEUE); 
}

void DisplayPanel::start(Game* g) {
	setDisplayState(DisplayState::GAME);
	game = g; 
}

void DisplayPanel::end(string state) {
	if (state == "X" || state == "O" || state == "TIE") { // three allowed states
		setDisplayState(DisplayState::GAME_END);
		if (state == "TIE") {
			outcome = state;
		} else {
			outcome = state == game->getPlayerType() ? "WIN" : "LOSS"; 
		}
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
}

void DisplayPanel::handleFail(beast::error_code ec,  char const* err) {
	parentFrame->SetStatusText("Error: " + string(err) + " with error code: " + to_string(ec.value()) + ". Reason: " + ec.message());
}

DisplayPanel::~DisplayPanel() {
	ioc.stop(); // stop the io_context event process or else program can't exit :P
}