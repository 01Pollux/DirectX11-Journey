
#include "utils/pch.hpp"
#include "geosphere.hpp"

#include "imgui/imgui.hpp"

namespace Pleiades::Sandbox
{
	RenderableGeoSphere::RenderableGeoSphere(DX::DeviceResources* d3dres, const char* name) :
		IRenderableShape(name, { 0.f, 0.5f, 20.f }, d3dres)
	{
		BuildCylinderMesh();

		m_Sphere->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		m_Sphere->CreateShaders(GetDeviceResources()->GetD3DDevice());
	}

	void RenderableGeoSphere::Render(uint64_t)
	{
		m_Sphere->Bind(GetDeviceResources()->GetD3DDeviceContext());
		UpdateScene();
		m_Sphere->Draw(GetDeviceResources()->GetD3DDeviceContext());
	}

	void RenderableGeoSphere::ImGuiRender()
	{
		if (ImGui::Button("Update"))
		{
			BuildCylinderMesh();
			m_Sphere->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		}

		ImGui::DragFloat3("Rotation offset", m_RotationOffset);
		ImGui::DragFloat("radius", &m_SphereRadius);
		ImGui::DragInt("Tesselation", &m_Tesselation);
	}

	void RenderableGeoSphere::BuildCylinderMesh()
	{
		GeometryFactory::CreateGeoSphere(
			m_Sphere,
			m_Tesselation,
			m_SphereRadius
		);
	}

	void RenderableGeoSphere::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Sphere->d3dConstants_WRP.SetData(
			d3dcontext,
			{
				DirectX::XMMatrixTranspose(
					DX::XMMatrixRotationRollPitchYaw(
						m_RotationOffset[0],
						m_RotationOffset[1],
						m_RotationOffset[2]
					) *
					DX::XMMatrixTranslation(
						m_DrawOffset[0],
						m_DrawOffset[1],
						m_DrawOffset[2]
					) *
					DirectX::XMMatrixPerspectiveLH(
						0.25f * 3.14f, GetDeviceResources()->GetAspectRatio(), 1.f, 1000.f
					)
				)
			}
		);
	}
}