#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wxPrecomp.h"
#include "DisplayPanel.h"

class MainFrame : public wxFrame {
	public:
		MainFrame();
		void OnExit(wxCommandEvent&); 
	private:
		DisplayPanel* displayPanel; 
		wxMenuBar* menubar; 
};

#endif
