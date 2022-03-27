#pragma once

#include <vector>
#include <functional>
#include "../../scopes.hpp"

namespace imcxx::misc
{
	/// <summary>
	/// Create or query a notification, configurable by 'popup_info'
	/// </summary>
	class notification : public scope_wrap<notification, imcxx::scope_traits::no_dtor, false>
	{
	public:
		struct reg_info;
		struct query_info;
		struct string_color
		{
			std::string string;
			uint32_t color;

			_CONSTEXPR20 string_color(std::string_view str, uint32_t color = 0xFF'FF'FF'FF) noexcept :
				string(str), color(color)
			{}
		};

		/// <summary>
		/// Register a new notification for display
		/// <param name="id">if 'Id' is not nullptr, Id will have notification's id</param>
		IMGUI_API notification(reg_info&&, uint32_t* id = nullptr);


		/// <summary>
		/// Find an pre-existing popup
		/// </summary>
		IMGUI_API notification(query_info&);

		/// <summary>
		/// Display the popups
		/// </summary>
		IMGUI_API static void render();

		/// <summary>
		/// Helper to convert vector of floats to rgba u32 at compile time
		/// </summary>
		static constexpr uint32_t color_to_u32(const ImVec4& clr) noexcept
		{
			constexpr auto to_u8 = [](float f)
			{
				constexpr auto saturate = [](float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; };
				return static_cast<uint8_t>(saturate(f) * 255.f + 0.5f);
			};
			return
				to_u8(clr.x) << IM_COL32_R_SHIFT |
				to_u8(clr.y) << IM_COL32_G_SHIFT |
				to_u8(clr.z) << IM_COL32_B_SHIFT |
				to_u8(clr.w) << IM_COL32_A_SHIFT;
		}
	};


	struct notification::reg_info
	{
		/// <summary>
		/// title's string with color
		/// </summary>
		std::vector<string_color> Title;
		/// <summary>
		/// text's string with color
		/// </summary>
		std::vector<string_color> Texts;
		/// <summary>
		/// notification's duration in milli-seconds
		/// </summary>
		float Duration;
		/// <summary>
		/// notification's background/foreground color
		/// </summary>
		uint32_t 
			BGColor{ color_to_u32(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]) },
			BorderColor{ color_to_u32(ImGui::GetStyle().Colors[ImGuiCol_Border]) };

		/// <summary>
		/// callback to invoke when the user right-click the notification
		/// </summary>
		std::function<void(uint32_t id)> OnRightClick;
		/// <summary>
		/// callback to invoke when the notification's lifetime end
		/// </summary>
		std::function<void(uint32_t id, bool force_close)> OnEnd;
	};


	struct notification::query_info
	{
		// In
		
		/// <summary>
		/// if 'Delete' was set to true, 
		/// </summary>
		bool Delete;
		/// <summary>
		/// Id of notification
		/// </summary>
		uint32_t Id;

		// Out

		/// <summary>
		/// if 'Exists' is not nullptr and the notification exists, 'Exists' will be set to true, else false
		/// </summary>
		bool* Exists{ };
		/// <summary>
		/// if 'Pending' is not nullptr and the notification is on pending, 'Pending' will be set to true, else false
		/// </summary>
		bool* Pending{ };
		/// <summary>
		/// if 'Title' is not nullptr and the notification exists, 'Title' will be pointing to notification's title
		/// </summary>
		std::vector<string_color>** Title{ };
		/// <summary>
		/// if 'Texts' is not nullptr and the notification exists, 'Texts' will be pointing to notification's title
		/// </summary>
		std::vector<string_color>** Texts{ };

		/// <summary>
		/// if 'BGColor' or 'FGColor' is not nullptr and the notification exists, they will be pointing to notification's background/foreground color
		/// </summary>
		uint32_t
			** BGColor,
			** BorderColor;
	};
}
