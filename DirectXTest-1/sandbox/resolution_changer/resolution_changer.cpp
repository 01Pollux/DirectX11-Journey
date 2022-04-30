
#include "utils/pch.hpp"
#include "resolution_changer.hpp"

#include <format>

#include "window/Game.hpp"
#include "imgui/imgui.hpp"

namespace Pleiades::Sandbox
{
	ResolutionCChanger_ImGui::ResolutionCChanger_ImGui(
		DX::DeviceResources* d3dres
	) : ISandbox(d3dres)
	{
		DX::ComPtr<IDXGIDevice> dxgi_device;
		DX::ThrowIfFailed(
			GetDeviceResources()->GetD3DDevice()->QueryInterface(IID_PPV_ARGS(dxgi_device.GetAddressOf()))
		);

		DX::ComPtr<IDXGIAdapter> dxgi_adapter;
		DX::ThrowIfFailed(
			dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf())
		);

		uint32_t output_id = 0;
		DX::ComPtr<IDXGIOutput> dxgi_output;
		while (SUCCEEDED(dxgi_adapter->EnumOutputs(output_id++, dxgi_output.ReleaseAndGetAddressOf())))
		{
			uint32_t modes_count;
			DX::ThrowIfFailed(
				dxgi_output->GetDisplayModeList(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					DXGI_ENUM_MODES_INTERLACED,
					&modes_count,
					nullptr
				)
			);

			size_t cur_pos = m_DXGIModes.size();
			m_DXGIModes.resize(cur_pos + modes_count);

			DX::ThrowIfFailed(
				dxgi_output->GetDisplayModeList(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					DXGI_ENUM_MODES_INTERLACED,
					&modes_count,
					m_DXGIModes.data() + cur_pos
				)
			);
		}

		bool even_switch = false;
		auto iter = std::remove_if(
			m_DXGIModes.begin(),
			m_DXGIModes.end(),
			[even_switch](const auto&) mutable
			{
				even_switch = !even_switch;
				return !even_switch;
			}
		);

		if (iter != m_DXGIModes.end())
			m_DXGIModes.erase(iter, m_DXGIModes.end());
	}

	void ResolutionCChanger_ImGui::OnImGuiDraw()
	{
		static bool is_fullscreen = Game::Get()->IsFullscreen();

		if (ImGui::Checkbox("Fullscreen", &is_fullscreen))
			Game::Get()->ToggleFullscreen(is_fullscreen);

		if (ImGui::BeginCombo("##Resoltuions", ""))
		{
			for (auto& desc : m_DXGIModes)
			{
				if (ImGui::Selectable(
					std::format("[{} x {}] (Refresh rate: {})", desc.Width, desc.Height, desc.RefreshRate.Numerator / desc.RefreshRate.Denominator).c_str()
				))
				{
					SetWindowPos(
						GetDeviceResources()->GetWindow(),
						HWND_TOP, 
						0, 
						0,
						desc.Width,
						desc.Height, 
						SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED | (is_fullscreen ? WS_POPUP : 0)
					);
				}
			}
			ImGui::EndCombo();
		}
	}
}