
#include "utils/pch.hpp"
#include "Game.hpp"

#include <format>

// ImGui
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

// Sandboxes
#include "utils/sandbox.hpp"


static void Enum_GPUAdapters(DX::DeviceResources* deviceres);


// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_Timer.GetFrameCount() == 0)
		return;

	Clear();

	m_DeviceResources->PIXBeginEvent(L"Render");
	[[maybe_unused]] auto d3dcontext = m_DeviceResources->GetD3DDeviceContext();

	// TODO: Add your rendering code here.

	m_Sandboxes.OnFrame(m_Timer.GetElapsedTicks());
	RenderImGui();

	m_DeviceResources->PIXEndEvent();

	// Show the new frame.
	m_DeviceResources->Present();
}


// Helper method to clear the back buffers.
void Game::Clear()
{
	m_DeviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto d3dcontext = m_DeviceResources->GetD3DDeviceContext();
	auto renderTarget = m_DeviceResources->GetRenderTargetView();
	auto depthStencil = m_DeviceResources->GetDepthStencilView();

	d3dcontext->ClearRenderTargetView(renderTarget, DirectX::Colors::CornflowerBlue);
	d3dcontext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	d3dcontext->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_DeviceResources->GetScreenViewport();
	d3dcontext->RSSetViewports(1, &viewport);

	m_DeviceResources->PIXEndEvent();
}


void Game::RenderImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool resolution = true, demo = false;
	if (ImGui::Begin("##Toggles"))
	{
		ImGui::Text("Press Alt+Enter to toggle pseudo-fullscreen mode");
		ImGui::Checkbox("Resolution Manager", &resolution);
		ImGui::Checkbox("Open demo", &demo);
	}
	ImGui::End();


	m_Sandboxes.OnImGuiDraw();
	ImGui::ShowDemoWindow(&demo);
	if (resolution)
	{
		if (ImGui::Begin("Resoltuion", &resolution))
			Enum_GPUAdapters(m_DeviceResources.get());
		ImGui::End();
	}


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}


void Enum_GPUAdapters(DX::DeviceResources* deviceres)
{
	using Microsoft::WRL::ComPtr;
	static bool once = false;

	static std::vector<DXGI_MODE_DESC> dxgi_modes;

	if (!once)
	{
		once = true;

		
		ComPtr<IDXGIDevice> dxgi_device;
		DX::ThrowIfFailed(
			deviceres->GetD3DDevice()->QueryInterface(IID_PPV_ARGS(dxgi_device.GetAddressOf()))
		);

		ComPtr<IDXGIAdapter> dxgi_adapter;
		DX::ThrowIfFailed(
			dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf())
		);

		uint32_t output_id = 0;
		ComPtr<IDXGIOutput> dxgi_output;
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

			size_t cur_pos = dxgi_modes.size();
			dxgi_modes.resize(cur_pos + modes_count);

			DX::ThrowIfFailed(
				dxgi_output->GetDisplayModeList(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					DXGI_ENUM_MODES_INTERLACED,
					&modes_count,
					dxgi_modes.data() + cur_pos
				)
			);

			bool even_switch = !(cur_pos % 2);
			auto iter = std::remove_if(
				dxgi_modes.begin() + cur_pos,
				dxgi_modes.end(),
				[even_switch](const auto&) mutable
				{
					even_switch = !even_switch;
					return !even_switch;
				}
			);

			if (iter != dxgi_modes.end())
				dxgi_modes.erase(iter, dxgi_modes.end());
		}
	}

	// Later, add way to toggle current resolutions
	
	if (ImGui::BeginCombo("##Resoltuions", ""))
	{
		for (auto& desc : dxgi_modes)
		{
			if (ImGui::Selectable(
				std::format("[{} x {}] (Refresh rate: {})", desc.Width, desc.Height, desc.RefreshRate.Numerator / desc.RefreshRate.Denominator).c_str(),
				(deviceres->GetOutputSize().right - deviceres->GetOutputSize().left) == static_cast<int>(desc.Width) &&
				(deviceres->GetOutputSize().bottom - deviceres->GetOutputSize().top) == static_cast<int>(desc.Height)
			))
			{
				deviceres->SetWindowSize(desc.Width, desc.Height);
				SetWindowPos(deviceres->GetWindow(), HWND_TOP, 0, 0, desc.Width, desc.Height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ImGui::EndCombo();
	}
	
}