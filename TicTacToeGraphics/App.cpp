#include "wxPrecomp.h"
#include "MainFrame.h"

class App : public wxApp {
public:
	virtual bool OnInit();
};

wxIMPLEMENT_APP(App);
bool App::OnInit() {
	MainFrame* frame = new MainFrame();

	frame->Show(true);

	// frame->AddChild(panel);

	return true;
} 