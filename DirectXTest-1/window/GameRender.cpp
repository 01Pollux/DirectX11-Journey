
#include "utils/pch.hpp"
#include "Game.hpp"

#include <format>

// ImGui
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

// Sandboxes
#include "utils/sandbox.hpp"


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

	d3dcontext->OMSetRenderTargets(1, &renderTarget, depthStencil);
	d3dcontext->ClearRenderTargetView(renderTarget, DirectX::Colors::CornflowerBlue);
	d3dcontext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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

	m_Sandboxes.OnImGuiDraw();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
