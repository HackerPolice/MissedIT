#include "Colors.hpp"

static void colorPicker() 
{
	const char* colorNames[IM_ARRAYSIZE(Colors::colors)];
	int size = IM_ARRAYSIZE(Colors::colors);
	for (int i = 0; i < size; i++)
		colorNames[i] = Colors::colors[i].name;

	static int colorSelected = 0;

		ImGui::Columns(2, nullptr, false);
		{
            ImGui::PushItemWidth(-1);
            ImGui::ListBoxHeader(XORSTR("##ColorNames"), ImVec2(-1, -1));
            for ( int i = 0; i < size; i++){
                if ( ImGui::Selectable(colorNames[i]) ){
                    colorSelected = i;
                }
            }
            // ImGui::ListBox(XORSTR("##COLORSELECTION"), &colorSelected, colorNames, IM_ARRAYSIZE(colorNames), size);
            ImGui::ListBoxFooter();
            ImGui::PopItemWidth();
		}
		ImGui::NextColumn();
		{
            ImGui::PushItemWidth(-1);
			if ( (int)Colors::colors[colorSelected].type == (int)Colors::ColorListVar::HEALTHCOLORVAR_TYPE)
			{
				UI::ColorPicker4((float*)Colors::colors[colorSelected].healthColorVarPtr);
				ImGui::Checkbox(XORSTR("Rainbow"), &Colors::colors[colorSelected].healthColorVarPtr->rainbow);
				ImGui::SameLine();
				ImGui::Checkbox(XORSTR("Health-Based"), &Colors::colors[colorSelected].healthColorVarPtr->hp);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &Colors::colors[colorSelected].healthColorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
				ImGui::PopItemWidth();
			}
			else
			{   
                
				UI::ColorPicker4((float*)Colors::colors[colorSelected].colorVarPtr);
				ImGui::Checkbox(XORSTR("Rainbow"), &Colors::colors[colorSelected].colorVarPtr->rainbow);
				ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &Colors::colors[colorSelected].colorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
				
			}
            ImGui::PopItemWidth();
		}
	// Colors::RenderWindow();
}


void Colors::RenderTab(){
    ImGui::Columns();
    {
        ImGui::BeginChild(XORSTR("ColorPicker"));
        {
            colorPicker();
        }ImGui::EndChild();
    }
}