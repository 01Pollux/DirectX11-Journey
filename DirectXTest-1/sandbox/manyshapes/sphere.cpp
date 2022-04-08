
#include "utils/pch.hpp"
#include "sphere.hpp"

#include "imgui/imgui.hpp"

namespace Pleiades::Sandbox
{
	RenderableSphere::RenderableSphere(DX::DeviceResources* d3dres, const char* name) :
		IRenderableShape(name, { 0.f, 0.5f, 20.f }, d3dres)
	{
		BuildCylinderMesh();

		m_Sphere->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		m_Sphere->CreateShaders(GetDeviceResources()->GetD3DDevice());
	}

	void RenderableSphere::Render(uint64_t)
	{
		m_Sphere->Bind(GetDeviceResources()->GetD3DDeviceContext());
		UpdateScene();
		m_Sphere->Draw(GetDeviceResources()->GetD3DDeviceContext());
	}

	void RenderableSphere::ImGuiRender()
	{
		if (ImGui::Button("Update"))
		{
			BuildCylinderMesh();
			m_Sphere->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		}

		ImGui::DragFloat3("Rotation offset", m_RotationOffset);
		ImGui::DragFloat("radius", &m_SphereRadius);
		ImGui::DragInt2("Slices/Stacks", m_SlicesStacks);
	}

	void RenderableSphere::BuildCylinderMesh()
	{
		GeometryFactory::CreateSphere(
			m_Sphere,
			m_SlicesStacks[0],
			m_SlicesStacks[1],
			m_SphereRadius
		);
	}

	void RenderableSphere::UpdateScene()
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