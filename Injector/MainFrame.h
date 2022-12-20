#include <wx/wx.h>
#pragma once

#define SIMPLE 0
#define MANUALMAPPING 1

class MainFrame : public wxFrame
{
public:
	wxTextCtrl* DLLPath = nullptr;
	wxTextCtrl* Log = nullptr;
	wxTextCtrl* programTarget = nullptr;
	wxChoice* combobox = nullptr;

	MainFrame(const wxString& title);

private:
	void OnInject(wxCommandEvent& evt);
};
