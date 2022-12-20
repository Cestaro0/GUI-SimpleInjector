#include "MainFrame.h"
#include "injector.h"
#include "InjectorUI.h"
#include "Manualinjector.h"
#include <wx/wx.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(350, 430))
{
	wxPanel* panel = new wxPanel(this);

	wxStaticText* label = new wxStaticText(panel, wxID_ANY, "DLL path:", wxPoint(20, 20));
	label->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));
	label->SetForegroundColour(UI::Colors::StaticText::Foreground);

	DLLPath = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(20, 40), wxSize(300, -1));
	DLLPath->SetFocus();
	DLLPath->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));

	wxStaticText* label2 = new wxStaticText(panel, wxID_ANY, "Program target:", wxPoint(20, 65));
	label2->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));
	label2->SetForegroundColour(UI::Colors::StaticText::Foreground);

	programTarget = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(20, 85), wxSize(-1, -1));
	programTarget->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));
	

	wxStaticText* label3 = new wxStaticText(panel, wxID_ANY, "Log:", wxPoint(20, 120), wxSize(-1, -1));
	label3->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));
	label3->SetForegroundColour(UI::Colors::StaticText::Foreground);

	Log = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(20, 140), wxSize(300, 220), wxTE_MULTILINE);
	Log->SetEditable(false);
	Log->SetBackgroundColour(UI::Colors::TextCtrl::Background);
	Log->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));
	
	wxArrayString ChoicesComboBox;
	ChoicesComboBox.Add("Simple");
	ChoicesComboBox.Add("Manual Map");
	
	combobox = new wxChoice(panel, wxID_ANY, wxPoint(135, 85), wxSize(105, -1), ChoicesComboBox);
	combobox->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));
	combobox->SetBackgroundColour(UI::Colors::ComboBox::Background);
	combobox->SetForegroundColour(UI::Colors::ComboBox::Foreground);

	wxButton* inject = new wxButton(panel, wxID_ANY, "inject", wxPoint(245, 84), wxSize(-1, 25));
	inject->SetFont(UI::Font::Custom(8, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, wxFONTFAMILY_SCRIPT));
	inject->SetBackgroundColour(UI::Colors::Button::Background);
	inject->SetForegroundColour(UI::Colors::Button::Foreground);

	inject->Bind(wxEVT_BUTTON, &MainFrame::OnInject, this);
}

void MainFrame::OnInject(wxCommandEvent& evt)
{
	injector* foo = new injector();
	Manualinjector* foo2 = new Manualinjector();

	std::wstring ProgramTarget = static_cast<std::wstring>(programTarget->GetValue().c_str());	//const wchar_t* ProgramTarget = static_cast<const wchar_t*>(aux.c_str());
	wcscat((wchar_t*)ProgramTarget.c_str(), L".exe");
	std::string DllPath = static_cast<std::string>(DLLPath->GetValue().c_str());	//LPCSTR DllPath = (LPCSTR)str.c_str();  gambiarras :P
	
	switch (combobox->GetSelection())
	{
	case SIMPLE:
	{
		if (!foo->DoInject(ProgramTarget.c_str(), DllPath.c_str()))
		{
			wxMessageBox("This Process cannot open", "Error", wxICON_ERROR);
		}
		else
		{
			Log->WriteText("[!] - Good work!\n");
			Log->WriteText("[+] - DLL is been injected successfully\n");
		}
		break;
	}
		
	case MANUALMAPPING:
	{
		if (!foo2->ManualInject(ProgramTarget.c_str(), DllPath.c_str()))
		{
			wxMessageBox("This Process cannot open", "Error", wxICON_ERROR);
		}
		else
		{
			Log->WriteText("[!] - Good work!\n");
			Log->WriteText("[+] - DLL is been injected successfully\n");
		}
		break;
	}
	}
	
}