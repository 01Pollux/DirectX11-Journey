#include "utils/pch.hpp"
#include "skull.hpp"

#include <imgui/imgui.hpp>

namespace Pleiades::Sandbox
{
	LitSkullDemo::LitSkullDemo(DX::DeviceResources* d3dres) : 
		ISandbox(d3dres),
		m_Effects(d3dres->GetD3DDevice(), GetDefaultWolrdConstants())
	{
		m_GeometryCallbacks.reserve(9 /* 1 ground + 4 spheres/cylinders */);

		InitializeShapes();

		InitializeShapesMesh();
		InitializeSkullMesh();

		InitializeBuffers();
		InitializeShaders();
	}


	void LitSkullDemo::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Effects.SetWorldEyePosition({ m_CamPosition[0], m_CamPosition[1], m_CamPosition[2] });

		m_Effects.Bind(d3dcontext);

		m_ShapesGeometry.Bind(d3dcontext);
		// m_ShapesGeometry will call m_Shapes.Bind() on each registered shape
		m_ShapesGeometry.Draw(d3dcontext, m_GeometryCallbacks);


		m_SkullGeometry.Bind(d3dcontext);
		m_Skull.Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
		m_SkullGeometry.Draw(d3dcontext, m_GeometryCallbacks);
	}


	void LitSkullDemo::OnImGuiDraw()
	{
		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);

		if (update)
			UpdateViewProjection();
	}


	void LitSkullDemo::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();
		m_ShapesGeometry.CreateBuffers(d3ddevice, true);
		m_SkullGeometry.CreateBuffers(d3ddevice, true);
	}


	void LitSkullDemo::InitializeShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();
		m_ShapesGeometry.CreateShaders(d3ddevice, L"resources/lights/skull_demo_vs.cso", L"resources/lights/skull_demo_ps.cso");
		m_SkullGeometry.CreateShaders(d3ddevice, L"resources/lights/skull_demo_vs.cso", L"resources/lights/skull_demo_ps.cso");
	}
}