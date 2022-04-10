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

		m_SkullGeometry.Bind(d3dcontext);
		m_Effects.SetMaterial(m_Skull.Material);
		m_Skull.Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
		m_SkullGeometry.Draw(d3dcontext);


		m_ShapesGeometry.Bind(d3dcontext);
		// m_ShapesGeometry will call m_Shapes.Bind() on each registered shape
		m_ShapesGeometry.Draw(d3dcontext, m_GeometryCallbacks);
	}


	void LitSkullDemo::OnImGuiDraw()
	{
		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);

		if (update)
			UpdateViewProjection();

		for (auto& material : {
			&m_Plane.Material,
			&m_Skull.Material,
			&m_Cylinder[0].Material,
			&m_Cylinder[1].Material,
			&m_Cylinder[2].Material,
			&m_Cylinder[3].Material
			})
		{
			ImGui::PushID(material);

			for (auto [name, vec_color] : {
				std::pair{ "Ambient", &material->Ambient },
				std::pair{ "Diffuse", &material->Diffuse },
				std::pair{ "Specular", &material->Specular }
				})
			{
				float color[4]{
					DX::XMVectorGetX(*vec_color),
					DX::XMVectorGetY(*vec_color),
					DX::XMVectorGetZ(*vec_color),
					DX::XMVectorGetW(*vec_color)
				};
				if (ImGui::ColorEdit4(name, color))
					*vec_color = DX::XMVectorSet(color[0], color[1], color[2], color[3]);
			}

			ImGui::PopID();
			ImGui::Separator();
		}
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