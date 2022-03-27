//
// Game.cpp
//

#include "utils/pch.hpp"
#include "Game.hpp"

// ImGui
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"


Game::Game()
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
	UninitializeImGui();
	DestroyWindow(m_deviceResources->GetWindow());
}


// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();

	m_deviceResources->CreateWindowSizeDependentResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	InitializeImGui();
}


#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([this]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	[[maybe_unused]] float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.
}
#pragma endregion


#pragma region ImGui handler
void Game::InitializeImGui()
{
	// Initialize ImGui
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui::GetIO().ConfigFlags |=
		ImGuiConfigFlags_DockingEnable |
		ImGuiConfigFlags_ViewportsEnable;

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_deviceResources->GetWindow());
	ImGui_ImplDX11_Init(
		m_deviceResources->GetD3DDevice(),
		m_deviceResources->GetD3DDeviceContext()
	);
}

void Game::UninitializeImGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
#pragma endregion


#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	m_deviceResources->WindowSizeChanged(width, height);
	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = m_deviceResources->GetWindowSize()[0];
	height = m_deviceResources->GetWindowSize()[1];
}
#pragma endregion
