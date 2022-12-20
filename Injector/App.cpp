#include "App.h"
#include "MainFrame.h"
#include "InjectorUI.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	MainFrame* mainFrame = new MainFrame("Injector");

	mainFrame->SetIcon(wxIcon("MyAppIcon"));

	mainFrame->SetBackgroundColour(UI::Colors::Frame::Background);

	mainFrame->Show();

	return true;
}