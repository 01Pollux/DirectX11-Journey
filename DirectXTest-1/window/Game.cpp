//
// Game.cpp
//

#include "utils/pch.hpp"
#include "Game.hpp"

// ImGui
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"


Game::Game(bool fullscreen) :
	// Setting m_IsFullscreen to the opposite of the wanted state, just so we can swap it back later in Game::Initialize
	m_IsFullscreen(!fullscreen)
{
	m_DeviceResources = std::make_unique<DX::DeviceResources>();
	m_DeviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
	UninitializeImGui();
	DestroyWindow(m_DeviceResources->GetWindow());
}


// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_DeviceResources->SetWindow(window, width, height);

	m_DeviceResources->CreateDeviceResources();

	m_DeviceResources->CreateWindowSizeDependentResources();

	ToggleFullscreen(!m_IsFullscreen);

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	m_Sandboxes.Init(m_DeviceResources.get());

	InitializeImGui();
}


#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_Timer.Tick([this]() { Update(m_Timer); });

	Render();
}

// Updates the world.
void Game::Update(const Pleiades::StepTimer& timer)
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
	ImGui_ImplWin32_Init(m_DeviceResources->GetWindow());
	ImGui_ImplDX11_Init(
		m_DeviceResources->GetD3DDevice(),
		m_DeviceResources->GetD3DDeviceContext()
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
	m_Timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto r = m_DeviceResources->GetOutputSize();
	m_DeviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	m_DeviceResources->WindowSizeChanged(width, height);
	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = m_DeviceResources->GetOutputSize().right - m_DeviceResources->GetOutputSize().left;
	height = m_DeviceResources->GetOutputSize().bottom - m_DeviceResources->GetOutputSize().top;
}

void Game::ToggleFullscreen(bool state) noexcept
{
	if (m_IsFullscreen == state)
		return;
	
	m_IsFullscreen = state;
	HWND hWnd = m_DeviceResources->GetWindow();
	// Implements the classic ALT+ENTER fullscreen toggle
	if (state)
	{
		SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

		SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	else
	{
		SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

		int width, height;
		GetDefaultSize(width, height);

		ShowWindow(hWnd, SW_SHOWNORMAL);

		SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}

bool Game::IsFullscreen() const noexcept
{
	return m_IsFullscreen;
}

#pragma endregion
