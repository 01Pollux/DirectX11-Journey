#ifndef IMCXX_MISC_NO_SHORTCUT_DEMO

#include "../shortcut.hpp"

#include "imgui/imcxx/viewports.hpp"
#include "imgui/imcxx/window.hpp"
#include "imgui/imcxx/override.hpp"
#include "imgui/imcxx/popup.hpp"
#include "imgui/imcxx/menuitem.hpp"
#include "imgui/imcxx/nodes.hpp"
#include "imgui/imcxx/colors.hpp"
#include "imgui/imcxx/input.hpp"

namespace imcxx::misc
{
	void render_shortcuts_demo()
	{
		ImGui::SetNextWindowSize({ 630.f, 140.f }, ImGuiCond_Once);
		imcxx::window notification_demo(
			"Shortcuts demo"
		);
		if (!notification_demo)
			return;

		if (imcxx::collapsing_header example_header{ "Random examples", ImGuiTreeNodeFlags_DefaultOpen })
		{
			ImGui::TextUnformatted(
				"Ctrl+Shift+L, Shift+X: Opens a window that can only be closed with 'p_close' boolean\n"
				"Ctrl+Alt+S, Ctrl+M: Opens/Close a window and can be closed with 'p_close' boolean"
			);

			static bool init = false;

			if (!init)
			{
				init = true;
				shortcuts::bind(
					"Persistent_Window#1",
					{
						{ ImGuiKey_LeftCtrl,	ImGuiKey_LeftShift,		ImGuiKey_L },
						{ ImGuiKey_RightCtrl,	ImGuiKey_LeftShift,		ImGuiKey_L },
						{ ImGuiKey_RightCtrl,	ImGuiKey_RightShift,	ImGuiKey_L },
						{ ImGuiKey_LeftCtrl,	ImGuiKey_RightShift,	ImGuiKey_L },
					},
					shortcuts_type::bind_persist,
					[](bool* p_close)
					{
						imcxx::window ctrl_shift_x{ "Ctrl+Shift+L Window", p_close };
						if (ctrl_shift_x)
						{
							ImGui::Button("Random #1");
							ImGui::Button("Random #2");
							ImGui::Button("Random #3"); ImGui::SameLine();
							ImGui::Button("Random #4"); ImGui::SameLine();
							ImGui::Button("Random #5"); ImGui::SameLine();
							ImGui::Button("Random #6");
							ImGui::Button("Random #7");
						}
					}
				);

				shortcuts::bind(
					"Persistent_Window#2",
					{
						{ ImGuiKey_LeftShift,	ImGuiKey_X },
						{ ImGuiKey_LeftShift,	ImGuiKey_X },
						{ ImGuiKey_RightShift,	ImGuiKey_X },
						{ ImGuiKey_RightShift,	ImGuiKey_X },
					},
					shortcuts_type::bind_persist,
					[](bool* p_close)
					{
						imcxx::window ctrl_shift_x{ "Shift+X Window", p_close };
						if (ctrl_shift_x)
						{
							ImGui::Button("Random #1");
							ImGui::Button("Random #2");
							ImGui::Button("Random #3"); ImGui::SameLine();
							ImGui::Button("Random #4"); ImGui::SameLine();
							ImGui::Button("Random #5"); ImGui::SameLine();
							ImGui::Button("Random #6");
							ImGui::Button("Random #7");
						}
					}
				);

				shortcuts::bind(
					"Switchable_Window#1",
					{
						{ ImGuiKey_LeftCtrl,	ImGuiKey_LeftAlt,	ImGuiKey_S },
						{ ImGuiKey_RightCtrl,	ImGuiKey_LeftAlt,	ImGuiKey_S },
						{ ImGuiKey_RightCtrl,	ImGuiKey_RightAlt,	ImGuiKey_S },
						{ ImGuiKey_LeftCtrl,	ImGuiKey_RightAlt,	ImGuiKey_S },
					},
					shortcuts_type::bind_default,
					[](bool* p_close)
					{
						imcxx::window ctrl_shift_x{ "Ctrl+Alt+S Window", p_close };
						if (ctrl_shift_x)
						{
							ImGui::Button("Random #1");
							ImGui::Button("Random #2");
							ImGui::Button("Random #3"); ImGui::SameLine();
							ImGui::Button("Random #4"); ImGui::SameLine();
							ImGui::Button("Random #5"); ImGui::SameLine();
							ImGui::Button("Random #6");
							ImGui::Button("Random #7");
						}
					}
				);

				shortcuts::bind(
					"Switchable_Window#2",
					{
						{ ImGuiKey_LeftCtrl,	ImGuiKey_M },
						{ ImGuiKey_RightCtrl,	ImGuiKey_M },
						{ ImGuiKey_RightCtrl,	ImGuiKey_M },
						{ ImGuiKey_LeftCtrl,	ImGuiKey_M },
					},
					shortcuts_type::bind_default,
					[](bool* p_close)
					{
						imcxx::window ctrl_shift_x{ "Ctrl+M Window", p_close };
						if (ctrl_shift_x)
						{
							ImGui::Button("Random #1");
							ImGui::Button("Random #2");
							ImGui::Button("Random #3"); ImGui::SameLine();
							ImGui::Button("Random #4"); ImGui::SameLine();
							ImGui::Button("Random #5"); ImGui::SameLine();
							ImGui::Button("Random #6");
							ImGui::Button("Random #7");
						}
					}
				);
			}
		}
	}
}

#endif