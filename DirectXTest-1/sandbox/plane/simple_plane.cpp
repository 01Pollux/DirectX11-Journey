
#include "utils/pch.hpp"
#include "simple_plane.hpp"

#include <format>

#include "window/Game.hpp"
#include "imgui/imgui.hpp"

namespace Pleiades::Sandbox
{
	SimplePlane::SimplePlane(DX::DeviceResources* d3dres) : 
		ISandbox(d3dres)
	{
		BuildPlaneMesh();

	}

	void SimplePlane::OnFrame(uint64_t)
	{
	}
}