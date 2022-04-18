
#include "utils/pch.hpp"
#include "skull.hpp"

#include "imgui/imgui.hpp"
#include "window/Game.hpp"

#include <fstream>

namespace Pleiades::Sandbox
{
	SkullFromFile::SkullFromFile(DX::DeviceResources* d3dres) : 
		ISandbox(d3dres)
	{
		m_Skull.PushMesh(GeometryFactory::CreateFromTxt("sandbox/from_file/skull.txt"));

		m_Skull.CreateBuffers(GetDeviceResources()->GetD3DDevice());
		m_Skull.CreateShaders(GetDeviceResources()->GetD3DDevice());
	}


	void SkullFromFile::OnFrame(uint64_t)
	{
		m_Skull.Bind(GetDeviceResources()->GetD3DDeviceContext());
		UpdateScene();
		m_Skull.Draw(GetDeviceResources()->GetD3DDeviceContext());
	}


	void SkullFromFile::OnImGuiDraw()
	{
		ImGui::DragFloat3("Draw offset", m_DrawOffset);
		ImGui::DragFloat3("Rotation offset", m_RotationOffset);
	}


	void SkullFromFile::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Skull.d3dConstants_WRP.SetData(
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