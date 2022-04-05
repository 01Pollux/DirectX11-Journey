
#include "utils/pch.hpp"
#include "cylinder.hpp"

#include "imgui/imgui.hpp"

namespace Pleiades::Sandbox
{
	RenderableCylinder::RenderableCylinder(DX::DeviceResources* d3dres, const char* name) : 
		IRenderableShape(name, { 0.f, 0.5f, 20.f }, d3dres)
	{
		BuildCylinderMesh();
		
		m_Cylinder->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		m_Cylinder->CreateShaders(GetDeviceResources()->GetD3DDevice());
	}

	void RenderableCylinder::Render(uint64_t)
	{
		m_Cylinder->Bind(GetDeviceResources()->GetD3DDeviceContext());
		UpdateScene();
		m_Cylinder->Draw(GetDeviceResources()->GetD3DDeviceContext());
	}

	void RenderableCylinder::ImGuiRender()
	{
		if (ImGui::Button("Update"))
		{
			BuildCylinderMesh();
			m_Cylinder->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		}

		ImGui::DragFloat3("Rotation offset", m_RotationOffset);
		ImGui::DragFloat2("Bottom / Top radius", m_CylinderRadius);
		ImGui::DragInt2("Slices/Stacks", m_SlicesStacks);
		ImGui::DragFloat("Height", &m_CylinderHeight);
	}

	void RenderableCylinder::BuildCylinderMesh()
	{
		GeometryFactory::CreateCylinder(
			m_Cylinder,
			m_SlicesStacks[0],
			m_SlicesStacks[1],
			m_CylinderRadius[0],
			m_CylinderRadius[1],
			m_CylinderHeight
		);
	}

	void RenderableCylinder::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Cylinder->d3dConstants_WRP.SetData(
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