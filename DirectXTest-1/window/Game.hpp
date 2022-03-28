//
// Game.h
//

#pragma once

#include "directx/DeviceResources.hpp"
#include "utils/StepTimer.hpp"
#include "utils/sandbox.hpp"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:
	Game();
	~Game();

	Game(Game&&) = default;
	Game& operator= (Game&&) = default;

	Game(Game const&) = delete;
	Game& operator= (Game const&) = delete;

	// Initialization and management
	void Initialize(HWND window, int width, int height);

	// Basic game loop
	void Tick();

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);

	// Properties
	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void Update(const Pleiades::StepTimer& timer);
	void Render();

	void Clear();

	void InitializeImGui();
	void RenderImGui();
	void UninitializeImGui();

private:
	// Device resources.
	std::unique_ptr<DX::DeviceResources>    m_DeviceResources;

	// Rendering loop timer.
	Pleiades::StepTimer                     m_Timer;
	
	// Samples for sandboxes
	Pleiades::SandboxHolder					m_Sandboxes;
};
