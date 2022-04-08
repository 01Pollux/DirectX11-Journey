
#include "utils/pch.hpp"
#include "simple_plane.hpp"

#include "imgui/imgui.hpp"
#include "window/Game.hpp"

namespace Pleiades::Sandbox
{
	SimplePlane::SimplePlane(DX::DeviceResources* d3dres) : 
		ISandbox(d3dres)
	{
		BuildPlaneMesh();

		m_Plane->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		m_Plane->CreateShaders(GetDeviceResources()->GetD3DDevice());
	}


	void SimplePlane::OnFrame(uint64_t)
	{
		m_Plane->Bind(GetDeviceResources()->GetD3DDeviceContext());
		UpdateScene();
		m_Plane->Draw(GetDeviceResources()->GetD3DDeviceContext());
	}


	void SimplePlane::OnImGuiDraw()
	{
		if (ImGui::Button("Update"))
		{
			BuildPlaneMesh();
			m_Plane->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		}

		ImGui::DragFloat3("Draw offset", m_DrawOffset);
		ImGui::DragFloat3("Rotation offset", m_RotationOffset);
		ImGui::DragFloat2("Width/Height", m_PlaneSize);
		ImGui::DragInt2("Col/Rows", m_RowCols);
	}


	void SimplePlane::BuildPlaneMesh()
	{
		GeometryFactory::CreatePlane(
			m_Plane,
			m_RowCols[0],
			m_RowCols[1],
			m_PlaneSize[0],
			m_PlaneSize[1]
		);

		//// build colors
		//for (auto& mesh : m_Plane->Mesh.vertices)
		//{
		//	float y = mesh.position.y = GeometryFactory::GetHeight(mesh.position.x, mesh.position.z);

		//	if (y < -10.0f)
		//	{
		//		// Sandy beach color.
		//		mesh.color = DX::XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		//	}
		//	else if (y < 5.0f)
		//	{
		//		// Light yellow-green.
		//		mesh.color = DX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		//	}
		//	else if (y < 12.0f)
		//	{
		//		// Dark yellow-green.
		//		mesh.color = DX::XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		//	}
		//	else if (y < 20.0f)
		//	{
		//		// Dark brown.
		//		mesh.color = DX::XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		//	}
		//	else
		//	{
		//		// White snow.
		//		mesh.color = DX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		//	}
		//}
	}


	void SimplePlane::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Plane->d3dConstants_WRP.SetData(
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