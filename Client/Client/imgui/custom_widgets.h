#pragma once

#include <string>
#include <WinUser.h>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "keyboard.h"
#include "timer.hpp"
#include "xorstr.hpp"

namespace widgets
{
	// menu color style colors
	auto bg = ImVec4{ 0.09f, 0.09f, 0.119f, 1.f };
	//auto bg = ImVec4{ 0.000000f, 0.000000f, 0.000001f, 1.000000f, };
	auto bg_half = ImVec4{ 0.000000f, 0.000000f, 0.000001f, .500000f, };
	auto still = ImVec4{ 0.294118f, 0.000000f, 0.403922f, 1.000000f };
	auto still_half = ImVec4{ 0.294118f, 0.000000f, 0.403922f, .200000f };
	auto still_q = ImVec4{ 0.294118f, 0.000000f, 0.403922f, .500000f };
	auto hovered = ImVec4{ 0.878431f, 0.290196f, 0.905882f, 1.000000f };
	auto active = ImVec4{ 0.984f, 0.521f, 1.f, 1.f };
	auto text_active = ImVec4{ 0.878431f, 0.290196f, 0.905882f, 0.500000f };
	auto dark_purple = ImVec4{ 0.097664f, 0.f, 0.099415f, 1.f };
	auto light_purple = ImVec4{ 0.898f, 0.219f, 1.f, 1.f };
	auto white_purple = ImVec4{ 0.984f, 0.521f, 1.f, 1.f };
	auto purple = ImVec4{ 0.498f, 0.01f, 0.568f, 1.f };
	auto _black = ImVec4{ 0.f, 0.f, 0.f, 1.f };
	auto _white = ImVec4{ 1.f, 1.f, 1.f, 1.f };
	auto green = ImVec4(0.134f, 0.814f, 0.092f, 1.00f);
	auto grey = ImVec4{ 0.768f, 0.768f, 0.768f, 1.f };
	auto grey_half = ImVec4{ 0.768f, 0.768f, 0.768f, .5f };
	auto shadow_inner = ImVec4{ 0.058824f, 0.000000f, 0.215686f, 1.000000f };
	auto shadow_outer = ImVec4{ 0.380392f, 0.058824f, 0.658824f, 1.000000f };

	// easy access colors
	const inline auto black = ImGui::ColorConvertFloat4ToU32({ 0.0f, 0.0f, 0.0f, 1.f });
	const inline auto white = ImGui::ColorConvertFloat4ToU32({ 1.f, 1.f, 1.f, 0.25f });
	const inline auto gray = ImGui::ColorConvertFloat4ToU32({ 0.5f, 0.5f, 0.5f, 0.5f });
	const inline auto invisible = ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.f });

	// tab structure for tab widget
	struct tab
	{
		std::string label;
		bool* active;
	};

	// text background color
	inline auto text_background = ImVec4{ 0.f, 0.f, 0.f, 1.f };
	inline auto text_foreground = ImVec4{ 1.f, 1.f, 1.f, 1.f };

	// border color for most widgets
	inline auto border = ImGui::ColorConvertFloat4ToU32(_black);//ImGui::ColorConvertFloat4ToU32({ .1f, .1f, .1f, 1.f });

	// for shadow_button1
	inline auto shader1_F = ImVec4{ 0.404f, 0.016f, 0.549f, 1.f };
	inline auto shader2_F = ImVec4{ .175f, 0.f, 0.333f, 1.f };
	inline auto shader1 = ImGui::ColorConvertFloat4ToU32({ 0.404f, 0.016f, 0.549f, 1.f });
	inline auto shader2 = ImGui::ColorConvertFloat4ToU32({ .175f, 0.f, 0.333f, 1.f });

	// for combo_box
	inline auto combo_shader1 = ImGui::ColorConvertFloat4ToU32({ 0.09f, 0.09f, 0.119f, 1.f });
	inline auto combo_shader2 = ImGui::ColorConvertFloat4ToU32({ 0.09f, 0.09f, 0.119f, 1.f });
	inline auto combo_border_out = black;
	inline auto combo_border_in = ImGui::ColorConvertFloat4ToU32({ 0.561f, 0.f, 1.f, 0.917f });

	// shader 3, 4, 5, 6 are for shadow_button2 (button on top of combo box)
	const inline auto shader3 = ImGui::ColorConvertFloat4ToU32({ 0.02f, 0.02f, 0.02f, .8f });
	const inline auto shader4 = ImGui::ColorConvertFloat4ToU32({ 0.04f, 0.04f, 0.04f, .7f });
	const inline auto shader5 = ImGui::ColorConvertFloat4ToU32({ 0.10f, 0.10f, 0.10f, .4f });
	const inline auto shader6 = ImGui::ColorConvertFloat4ToU32({ 0.12f, 0.12f, 0.12f, .6f });

	// for widget gradients
	inline auto widget_shader1_f = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };
	inline auto widget_shader2_f = ImVec4{ 0.3f, 0.3f, 0.3f, 0.5f };
	inline auto widget_shader1 = ImGui::ColorConvertFloat4ToU32(widget_shader1_f);
	inline auto widget_shader2 = ImGui::ColorConvertFloat4ToU32(widget_shader2_f);

	// font declarationgs
	inline ImFont* font_small;
	inline ImFont* font_normal;
	inline ImFont* font_small_small;

	inline void set_theme()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		{
			colors[ImGuiCol_Text] = _white;
			colors[ImGuiCol_TextDisabled] = grey_half;
			colors[ImGuiCol_WindowBg] = bg;
			colors[ImGuiCol_ChildBg] = still_q;
			colors[ImGuiCol_PopupBg] = still_q;
			colors[ImGuiCol_Border] = _black;
			colors[ImGuiCol_BorderShadow] = dark_purple;
			colors[ImGuiCol_FrameBg] = still_half;
			colors[ImGuiCol_FrameBgHovered] = hovered;
			colors[ImGuiCol_FrameBgActive] = active;
			colors[ImGuiCol_TitleBg] = still;
			colors[ImGuiCol_TitleBgActive] = still;
			colors[ImGuiCol_TitleBgCollapsed] = still;
			colors[ImGuiCol_MenuBarBg] = dark_purple;
			colors[ImGuiCol_ScrollbarBg] = dark_purple;
			colors[ImGuiCol_ScrollbarGrab] = purple;
			colors[ImGuiCol_ScrollbarGrabHovered] = hovered;
			colors[ImGuiCol_ScrollbarGrabActive] = active;
			colors[ImGuiCol_CheckMark] = white_purple;
			colors[ImGuiCol_SliderGrab] = purple;
			colors[ImGuiCol_SliderGrabActive] = active;
			colors[ImGuiCol_Button] = still;
			colors[ImGuiCol_ButtonHovered] = hovered;
			colors[ImGuiCol_ButtonActive] = active;
			colors[ImGuiCol_Header] = still;
			colors[ImGuiCol_HeaderHovered] = hovered;
			colors[ImGuiCol_HeaderActive] = active;
			colors[ImGuiCol_ResizeGrip] = dark_purple;
			colors[ImGuiCol_ResizeGripHovered] = hovered;
			colors[ImGuiCol_ResizeGripActive] = active;
			colors[ImGuiCol_PlotLines] = dark_purple;
			colors[ImGuiCol_PlotLinesHovered] = dark_purple;
			colors[ImGuiCol_PlotHistogram] = dark_purple;
			colors[ImGuiCol_PlotHistogramHovered] = hovered;
			colors[ImGuiCol_TextSelectedBg] = text_active;
			colors[ImGuiCol_ModalWindowDarkening] = bg_half;
		}
	}

	inline void shadow_text(const std::string& label)
	{
		auto pos = ImGui::GetCursorPos();
		const int size = 1;

		ImGui::SetCursorPos({ pos.x - size, pos.y });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos({ pos.x - size, pos.y - size });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos({ pos.x, pos.y - size });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos({ pos.x + size, pos.y - size });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos({ pos.x + size, pos.y });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos({ pos.x + size, pos.y + size });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos({ pos.x, pos.y + size });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos({ pos.x - size, pos.y + size });
		ImGui::TextColored(text_background, label.c_str());

		ImGui::SetCursorPos(pos);
		ImGui::TextColored(text_foreground, label.c_str());
	}

	inline void combo_box(const std::string& label, ImVec2 top_left, ImVec2 bottom_right)
	{
		auto previous_pos = ImGui::GetCursorPos();
		auto wnd_pos = ImGui::GetWindowPos();
		auto abs_top_left = top_left;
		auto abs_bottom_right = bottom_right;
		abs_top_left.offset(wnd_pos);
		abs_bottom_right.offset(wnd_pos);

		auto text_rect = ImGui::CalcTextSize(label.c_str());
		auto draw_list = ImGui::GetWindowDrawList();

		const int y_gap = 0;
		const ImVec2 points[] =
		{
			{abs_top_left.x + 20, abs_top_left.y + y_gap},				// top right + text start
			{abs_top_left.x, abs_top_left.y + y_gap},					// top right
			{abs_top_left.x, abs_bottom_right.y},					// bottom left
			abs_bottom_right,									// bottom right		
			{abs_bottom_right.x, abs_top_left.y + y_gap},				// top right
			{abs_top_left.x + 20 + text_rect.x, abs_top_left.y + y_gap} // top right + text end
		};

		const int in = 1;
		const ImVec2 points_inner[] =
		{
			{abs_top_left.x + 20, abs_top_left.y + y_gap + in},				 // top left + text start
			{abs_top_left.x + in, abs_top_left.y + y_gap + in},				 // top left
			{abs_top_left.x + in, abs_bottom_right.y - in},				 // bottom left
			{abs_bottom_right.x - in, abs_bottom_right.y - in},			 // bottom right		
			{abs_bottom_right.x - in, abs_top_left.y + y_gap + in},			 // top right
			{abs_top_left.x + 20 + text_rect.x, abs_top_left.y + y_gap + in} // top right + text end
		};

		draw_list->AddPolyline(&points_inner[0], IM_ARRAYSIZE(points_inner), combo_border_in, false, 1.f);
		draw_list->AddPolyline(&points[0], IM_ARRAYSIZE(points), combo_border_out, false, 1.f);
		//draw_list->AddRectFilledMultiColor({ abs_top_left.x, abs_top_left.y + 7 }, abs_bottom_right, combo_shader1, combo_shader1, combo_shader2, combo_shader2);

		ImGui::SetCursorPos({ top_left.x + 20, top_left.y - text_rect.y / 2 });
		shadow_text(label);

		ImGui::SetCursorPos(previous_pos);
	}

	inline bool shadow_button(const std::string& label, ImVec2 top_left, ImVec2 size)
	{
		auto previous_pos = ImGui::GetCursorPos();
		auto wnd_pos = ImGui::GetWindowPos();
		auto abs_top_left = top_left;
		auto abs_bottom_right = ImVec2{ top_left.x + size.x, top_left.y + size.y };
		abs_top_left.offset(wnd_pos);
		abs_bottom_right.offset(wnd_pos);

		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertU32ToFloat4(shader1));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorConvertU32ToFloat4(shader1));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorConvertU32ToFloat4(shader2));

		bool clicked = false;

		ImGui::SetCursorPos({ top_left.x + 1, top_left.y + 1 });
		if (ImGui::Button(("##" + label).c_str(), { size.x - 2, size.y - 2 }))
			clicked = true;

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		if (ImGui::IsItemActive())
		{
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor({ abs_top_left.x + 2, abs_top_left.y + 2 }, { abs_bottom_right.x - 2, abs_bottom_right.y - 2 }, shader1, shader2, shader1, shader2);
			ImGui::GetWindowDrawList()->AddRect({ abs_top_left.x + 1, abs_top_left.y + 1 }, { abs_bottom_right.x - 1, abs_bottom_right.y - 1 }, border);
		}

		else
		{
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor({ abs_top_left.x + 1, abs_top_left.y + 1 }, { abs_bottom_right.x - 1, abs_bottom_right.y - 1 }, shader1, shader2, shader1, shader2);
			ImGui::GetWindowDrawList()->AddRect(abs_top_left, abs_bottom_right, border);
		}

		bool pushed = false;
		if (ImGui::IsItemActive())
		{
			ImGui::PushFont(font_small);
			pushed = true;
		}

		auto text_size = ImGui::CalcTextSize(label.c_str());
		ImGui::SetCursorPos({ (size.x / 2 - text_size.x / 2) + top_left.x, (size.y / 2 - text_size.y / 2) + top_left.y });

		shadow_text(label);

		if (pushed)
			ImGui::PopFont();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return clicked;
	}

	inline bool shadow_button2(const std::string& label, ImVec2 top_left, ImVec2 size, bool show_label = true, const std::string id_pad = "")
	{
		auto previous_pos = ImGui::GetCursorPos();
		auto wnd_pos = ImGui::GetWindowPos();
		auto abs_top_left = top_left;
		auto abs_bottom_right = ImVec2{ top_left.x + size.x, top_left.y + size.y };
		abs_top_left.offset(wnd_pos);
		abs_bottom_right.offset(wnd_pos);

		ImGui::PushStyleColor(ImGuiCol_Button, invisible);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, invisible);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, invisible);

		bool clicked = false;

		ImGui::SetCursorPos({ top_left.x + 1, top_left.y + 1 });
		if (ImGui::Button(("##" + label + id_pad).c_str(), { size.x - 2, size.y - 2 }))
			clicked = true;
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		if (ImGui::IsItemActive())
		{
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor({ abs_top_left.x + 2, abs_top_left.y + 2 }, { abs_bottom_right.x - 2, abs_bottom_right.y - 2 }, widget_shader1, widget_shader2, widget_shader1, widget_shader2);
			ImGui::GetWindowDrawList()->AddRect({ abs_top_left.x + 1, abs_top_left.y + 1 }, { abs_bottom_right.x - 1, abs_bottom_right.y - 1 }, black, 0.f, 15, 1.f);
		}

		else
		{
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor({ abs_top_left.x + 1, abs_top_left.y + 1 }, { abs_bottom_right.x - 1, abs_bottom_right.y - 1 }, widget_shader1, widget_shader2, widget_shader1, widget_shader2);
			ImGui::GetWindowDrawList()->AddRect(abs_top_left, abs_bottom_right, black, 0.f, 15, 1.f);
		}

		bool pushed = false;
		if (ImGui::IsItemActive())
		{
			ImGui::PushFont(font_small);
			pushed = true;
		}

		if (show_label)
		{
			auto text_size = ImGui::CalcTextSize(label.c_str());
			ImGui::SetCursorPos({ (size.x / 2 - text_size.x / 2) + top_left.x, (size.y / 2 - text_size.y / 2) + top_left.y });

			shadow_text(label);
		}

		if (pushed)
			ImGui::PopFont();

		ImGui::PopStyleColor(3);

		return clicked;
	}

	inline void tab_system(const std::vector<tab>& tabs, ImVec2 top_left, ImVec2 size, int tab_gap)
	{
		for (auto i = 0u; i < tabs.size(); ++i)
		{
			float tab_y_pos = top_left.y + i * (size.y + tab_gap);
			if (shadow_button(tabs[i].label, { top_left.x, tab_y_pos }, size))
				for (auto j = 0u; j < tabs.size(); ++j)
					*tabs[j].active = i == j;

			if (*tabs[i].active)
			{
				ImGui::GetWindowDrawList()->AddRectFilled({ top_left.x + size.x + 3, tab_y_pos + 3 }, { top_left.x + size.x + 5, tab_y_pos - 3 + size.y }, widget_shader1);
				ImGui::GetWindowDrawList()->AddRect({ top_left.x + size.x + 2, tab_y_pos + 2 }, { top_left.x + size.x + 6, tab_y_pos - 2 + size.y }, border);
				//const float tri_x = top_left.x + size.x + 3;
				//const float arrow_inset = 8.f;
				//ImGui::GetWindowDrawList()->AddTriangleFilled({ tri_x, tab_y_pos + arrow_inset }, { tri_x, tab_y_pos + size.y - arrow_inset }, { tri_x + size.y / 2 - arrow_inset, tab_y_pos + size.y / 2 }, gray);
			}
		}
	}

	inline void float_slider(const std::string& label, float& val, float min, float max, ImVec2 top_left, ImVec2 size, int decimal_count = 2)
	{
		ImGui::PushAllowKeyboardFocus(false);
		// render text & value abover slider first
		ImGui::SetCursorPos({ top_left.x + 2, top_left.y });
		auto text_size = ImGui::CalcTextSize(label.c_str());
		shadow_text(label);

		ImGui::PushFont(font_small_small);
		auto value_str = std::to_string(static_cast<int>(max)) + "." + std::string(decimal_count, '0');
		auto value_text_size = ImGui::CalcTextSize(value_str.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.f, 0.f, 0.f, 0.f });
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.f, 0.f, 0.f, 0.f });
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.f, 0.f, 0.f, 0.f });
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, { 0.f, 0.f, 0.f, 0.f });

		ImGui::SetCursorPos({ top_left.x + size.x - value_text_size.x, top_left.y + 2 });
		ImGui::SetNextItemWidth(value_text_size.x);
		auto format = "%" + std::to_string(std::to_string(static_cast<int>(max)).size() + 3) + ".2f";
		if (ImGui::InputFloat(("##inputfloat" + label).c_str(), &val, 0, 0, format.c_str(), ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll))
			val = std::clamp<float>(val, min, max);

		ImGui::PopFont();

		// set cursor pos to below text
		ImGui::SetCursorPos({ top_left.x, top_left.y + text_size.y });

		// calculate the abs position for directlly drawing
		auto abs_top_left = ImGui::GetCursorPos();
		abs_top_left.offset(ImGui::GetWindowPos());
		ImGui::SetNextItemWidth(size.x);
		ImGui::GetIO().KeyCtrl = false;
		ImGui::SliderFloat(("##" + label).c_str(), &val, min, max, "", 1.f);
		if (ImGui::IsItemHovered() || ImGui::IsItemActive())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		const auto color2 = ImGui::ColorConvertFloat4ToU32({ widget_shader1_f.x, widget_shader1_f.y, widget_shader1_f.z, widget_shader1_f.w + (val / max / 2.f) });
		const auto color = widget_shader2;
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(abs_top_left, { abs_top_left.x + size.x * val / max, abs_top_left.y + text_size.y }, color, color2, color2, color);
		//ImGui::GetWindowDrawList()->AddRectFilled(abs_top_left, { abs_top_left.x + size.x * val / max, abs_top_left.y + text_size.y }, ImGui::ColorConvertFloat4ToU32({ 0.5f, 0.5f, 0.5f, 1.0f }));
		ImGui::GetWindowDrawList()->AddRect(abs_top_left, { abs_top_left.x + size.x, abs_top_left.y + text_size.y }, black, 0.f, 15, 1.f);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(4);
		ImGui::PopAllowKeyboardFocus();
	}

	inline bool percent_slider(const std::string& label, float& val, float min, float max, ImVec2 top_left, ImVec2 size, int decimal_count = 0)
	{
		bool return_value = false;
		ImGui::PushAllowKeyboardFocus(false);
		// render text & value abover slider first
		ImGui::SetCursorPos({ top_left.x + 2, top_left.y });
		auto text_size = ImGui::CalcTextSize(label.c_str());
		shadow_text(label);

		ImGui::PushFont(font_small_small);
		auto value_str = std::to_string(static_cast<int>(max)) + "." + std::string(decimal_count, '0');
		auto value_text_size = ImGui::CalcTextSize(value_str.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.f, 0.f, 0.f, 0.f });
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.f, 0.f, 0.f, 0.f });
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.f, 0.f, 0.f, 0.f });
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, { 0.f, 0.f, 0.f, 0.f });

		ImGui::SetCursorPos({ top_left.x + size.x - value_text_size.x, top_left.y + 2 });
		ImGui::SetNextItemWidth(value_text_size.x);
		auto format = "%" + std::to_string(std::to_string(static_cast<int>(max)).size()) + ".0f%%";
		if (ImGui::InputFloat(("##inputfloat" + label).c_str(), &val, 0, 0, format.c_str(), ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll))
		{
			return_value = true;
			val = std::clamp<float>(val, min, max);
		}
			
		ImGui::PopFont();

		// set cursor pos to below text
		ImGui::SetCursorPos({ top_left.x, top_left.y + text_size.y });

		// calculate the abs position for directlly drawing
		auto abs_top_left = ImGui::GetCursorPos();
		abs_top_left.offset(ImGui::GetWindowPos());
		ImGui::SetNextItemWidth(size.x);
		ImGui::GetIO().KeyCtrl = false;
		if (ImGui::SliderFloat(("##" + label).c_str(), &val, min, max, "", 1.f))
		{
			return_value = true;
		}
		if (ImGui::IsItemHovered() || ImGui::IsItemActive())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		const auto color2 = ImGui::ColorConvertFloat4ToU32({ widget_shader1_f.x, widget_shader1_f.y, widget_shader1_f.z, widget_shader1_f.w + (val / max / 2.f) });
		const auto color = widget_shader2;
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(abs_top_left, { abs_top_left.x + size.x * val / max, abs_top_left.y + text_size.y }, color, color2, color2, color);
		//ImGui::GetWindowDrawList()->AddRectFilled(abs_top_left, { abs_top_left.x + size.x * val / max, abs_top_left.y + text_size.y }, ImGui::ColorConvertFloat4ToU32({ 0.5f, 0.5f, 0.5f, 1.0f }));
		ImGui::GetWindowDrawList()->AddRect(abs_top_left, { abs_top_left.x + size.x, abs_top_left.y + text_size.y }, black, 0.f, 15, 1.f);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(4);
		ImGui::PopAllowKeyboardFocus();

		return return_value;
	}

	inline void int_slider(const std::string& label, int& val, int min, int max, ImVec2 top_left, ImVec2 size)
	{
		ImGui::PushAllowKeyboardFocus(false);
		// render text & value abover slider first
		ImGui::SetCursorPos({ top_left.x + 2, top_left.y });
		auto text_size = ImGui::CalcTextSize(label.c_str());
		shadow_text(label);

		ImGui::PushFont(font_small_small);
		auto value_str = std::to_string(max);
		auto value_text_size = ImGui::CalcTextSize(value_str.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });

		ImGui::PushStyleColor(ImGuiCol_FrameBg, invisible);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, invisible);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, invisible);
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, invisible);
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, invisible);
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, invisible);

		ImGui::SetCursorPos({ top_left.x + size.x - value_text_size.x, top_left.y + 2 });
		ImGui::SetNextItemWidth(value_text_size.x);
		auto format = "%" + std::to_string(std::to_string(max).size()) + "d";
		if (ImGui::InputInt(("##inputfloat" + label).c_str(), &val, 0, 0, format.c_str(), ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll))
			val = std::clamp(val, min, max);

		ImGui::PopFont();

		// set cursor pos to below text
		ImGui::SetCursorPos({ top_left.x, top_left.y + text_size.y });

		// calculate the abs position for directlly drawing
		auto abs_top_left = ImGui::GetCursorPos();
		abs_top_left.offset(ImGui::GetWindowPos());
		ImGui::SetNextItemWidth(size.x);
		ImGui::GetIO().KeyCtrl = false;
		ImGui::SliderInt(("##" + label).c_str(), &val, min, max, "");
		if (ImGui::IsItemHovered() || ImGui::IsItemActive())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		const auto color2 = ImGui::ColorConvertFloat4ToU32({ widget_shader1_f.x, widget_shader1_f.y, widget_shader1_f.z, widget_shader1_f.w + (static_cast<float>(val) / static_cast<float>(max) / 2.f) });
		const auto color = widget_shader2;
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(abs_top_left, { abs_top_left.x + size.x * val / max, abs_top_left.y + text_size.y }, color, color2, color2, color);
		//ImGui::GetWindowDrawList()->AddRectFilled(abs_top_left, { abs_top_left.x + size.x * val / max, abs_top_left.y + text_size.y }, ImGui::ColorConvertFloat4ToU32({ 0.5f, 0.5f, 0.5f, 1.0f }));
		ImGui::GetWindowDrawList()->AddRect(abs_top_left, { abs_top_left.x + size.x, abs_top_left.y + text_size.y }, black, 0.f, 15, 1.75f);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(6);
		ImGui::PopAllowKeyboardFocus();
	}

	inline void checkbox(const std::string& label, bool& val, ImVec2 top_left, ImVec2 size = { 18, 18 })
	{
		ImGui::PushStyleColor(ImGuiCol_CheckMark, invisible);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, invisible);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, invisible);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, invisible);

		ImGui::SetCursorPos(top_left);
		ImGui::Checkbox(("##" + label).c_str(), &val);

		auto list = ImGui::GetWindowDrawList();
		auto abs_top_left = top_left;
		abs_top_left.offset(ImGui::GetWindowPos());

		if (val || ImGui::IsItemActive())
		{
			if (ImGui::IsItemActive())
			{
				const float passing = 4.f;
				const auto box_tl = ImVec2{ abs_top_left.x + passing, abs_top_left.y + passing };
				const auto box_br = ImVec2{ abs_top_left.x + size.x - passing, abs_top_left.y + size.y - passing };
				list->AddRectFilledMultiColor(box_tl, box_br, widget_shader1, widget_shader2, widget_shader1, widget_shader2);
			}
			/*else if (ImGui::IsItemHovered())
			{
				const float passing = 4.f;
				const auto box_tl = ImVec2{ abs_top_left.x + passing, abs_top_left.y + passing };
				const auto box_br = ImVec2{ abs_top_left.x + size.x - passing, abs_top_left.y + size.y - passing };
				list->AddRectFilledMultiColor(box_tl, box_br, widget_shader1, widget_shader2, widget_shader1, widget_shader2);
			}*/
			else
			{
				const float passing = 3.f;
				const auto box_tl = ImVec2{ abs_top_left.x + passing, abs_top_left.y + passing };
				const auto box_br = ImVec2{ abs_top_left.x + size.x - passing, abs_top_left.y + size.y - passing };
				//list->AddRectFilled(box_tl, box_br, widget_shader1);
				list->AddRectFilledMultiColor(box_tl, box_br, widget_shader1, widget_shader2, widget_shader1, widget_shader2);
			}
		}
		list->AddRect(abs_top_left, { abs_top_left.x + size.x, abs_top_left.y + size.y }, black, 0.f, 15, 1.f);

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		ImGui::SetCursorPos({ top_left.x + size.x + 5, top_left.y + ImGui::GetStyle().FramePadding.y });
		shadow_text(label);

		ImGui::PopStyleColor(4);
	}

	inline bool color_picker(const std::string& label, float* color, ImVec2 top_left)
	{
		bool return_value = false;

		ImGui::SetCursorPos(top_left);
		if (ImGui::ColorEdit4(("##" + label).c_str(), color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview))
			return_value = true;

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		auto abs_top_left = top_left;
		abs_top_left.offset(ImGui::GetWindowPos());
		ImGui::GetWindowDrawList()->AddRect(abs_top_left, { abs_top_left.x + 18, abs_top_left.y + 18 }, black, 0.f, 15, 1.f);

		ImGui::SetCursorPos({ top_left.x + 18 + 5, top_left.y + ImGui::GetStyle().FramePadding.y });
		shadow_text(label);

		return return_value;
	}

	inline void key_selector(const std::string& label, const std::vector<int*>& key_list, int& key, ImVec2 top_left, ImVec2 size)
	{
		auto key_name = input::keyboard::vkey_to_name(key);
		static bool duplicate = false;
		static uint64_t time_count = 0;
		int new_key = -1;

		if (shadow_button2(key_name.data(), top_left, size, true, label))
			ImGui::OpenPopup((xorstr_("##key_select") + label).c_str());

		ImGui::SetCursorPos({ top_left.x + size.x + 2, top_left.y + 4 });
		shadow_text(label);

		ImGui::PushStyleColor(ImGuiCol_ModalWindowDarkening, { 0.09f, 0.09f, 0.119f, .9f });

		ImVec2 popup_size = { 138, 50 };
		ImGui::SetNextWindowSize(popup_size);
		if (ImGui::BeginPopupModal((xorstr_("##key_select") + label).c_str(), (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar))
		{
			auto wnd_pos = ImGui::GetWindowPos();
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(wnd_pos, { wnd_pos.x + popup_size.x, wnd_pos.y + popup_size.y }, shader2, shader2, shader1, shader1);

			static timer t{};

			ImGui::SetCursorPos({ 5, 10 });

			auto elapsed = t.elapsed_time<std::chrono::milliseconds>().count() / 500 % 3;

			if (elapsed == 0)
				shadow_text(xorstr_("press any key."));
			else if (elapsed == 1)
				shadow_text(xorstr_("press any key.."));
			else if (elapsed == 2)
				shadow_text(xorstr_("press any key..."));

			static bool cleared = false;
			if (!cleared)
			{
				for (int key = 0u; key < VK_OEM_CLEAR + 1; ++key)
					GetAsyncKeyState(key);
				cleared = true;
			}

			Sleep(10);
			for (int key_i = 0u; key_i < VK_OEM_CLEAR + 1; ++key_i)
			{
				if (GetAsyncKeyState(key_i) & 0x1)
					new_key = key_i;
			}

			for (auto key_i : key_list)
			{
				if (new_key > 0 && *key_i == new_key)
				{
					if (new_key == key)
						break;

					new_key = -1;
					duplicate = true;
					time_count = t.elapsed_time<std::chrono::seconds>().count();
				}
			}

			ImGui::SetCursorPos({ 5, 30 });
			if (duplicate)
			{
				shadow_text(xorstr_("key in use!"));

				if (t.elapsed_time<std::chrono::seconds>().count() - time_count > 2)
					duplicate = false;
			}
			else
				shadow_text(xorstr_("(esc to clear)"));

			if (new_key != -1)
			{
				if (new_key == VK_ESCAPE)
					key = 0;
				else
					key = new_key;

				cleared = false;
				duplicate = false;
				ImGui::CloseCurrentPopup();

				if (key == VK_LBUTTON)
					Sleep(150);
			}

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();
	}

	inline bool title_bar(const std::string& label, bool close_button)
	{
		auto pos = ImVec2{ 0, 0 };
		pos.offset(ImGui::GetWindowPos());

		auto size = ImGui::GetWindowSize();

		ImGui::GetStyle().WindowPadding = { 0,0 };
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(pos, { pos.x + size.x, pos.y + 20 }, shader2, shader2, shader1, shader1);

		ImGui::SetCursorPos({ 5,5 });
		shadow_text(label);

		if (close_button)
		{
			bool closed = false;
			if (shadow_button2("", { size.x - 18, 3 }, { 15, 15 }, false, "asdfadfasdf"))
				closed = true;

			ImGui::SetCursorPos({ size.x - 14, 4 });
			shadow_text("x");

			return closed;
		}

		return false;
	}

	inline bool text_entry_hint(const std::string& hint, const ImVec2& top_left, const ImVec2& size, std::string& buf)
	{
		ImGui::SetCursorPos(top_left);
		ImGui::SetNextItemWidth(size.x);
		ImGui::InputTextWithHint(("##" + hint).c_str(), hint.c_str(), buf.data(), buf.size());
	}
}