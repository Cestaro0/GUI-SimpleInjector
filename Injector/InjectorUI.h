#pragma once
#include <wx/wx.h>

namespace UI 
{
	namespace Colors 
	{
		namespace Button
		{
			static wxColour Foreground(60, 124, 125);
			static wxColour Background(255, 255, 255, -1);
		}
		
		namespace TextCtrl
		{
			static wxColour Foreground(41, 84, 105);
			static wxColour Background(255, 255, 255, -1);
		}

		namespace StaticText
		{
			static wxColour Foreground(41, 84, 105);
		}

		namespace ComboBox 
		{
			static wxColour Foreground(41, 84, 105);
			static wxColour Background(255, 255, 255, -1);
		}

		namespace Frame
		{
			static wxColour Background(240, 248, 255);
		}
	}

	namespace Font
	{
		static auto Custom = [&](
			size_t zsFsize, 
			wxFontStyle stEstilo = wxFontStyle::wxFONTSTYLE_NORMAL, 
			wxFontWeight ftTamanho = wxFontWeight::wxFONTWEIGHT_NORMAL, 
			wxFontFamily ftFamilia = wxFontFamily::wxFONTFAMILY_MODERN
			) -> wxFont
		{

			wxFont fonte(zsFsize, ftFamilia, stEstilo, ftTamanho);

			return fonte;

		};
	}
}