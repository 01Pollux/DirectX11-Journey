
#include "utils/pch.hpp"
#include "car.hpp"

#include "imgui/imgui.hpp"
#include "window/Game.hpp"

#include <fstream>

namespace Pleiades::Sandbox
{
	CarFromFile::CarFromFile(DX::DeviceResources* d3dres) :
		ISandbox(d3dres)
	{
		m_Car.PushMesh(GeometryFactory::CreateFromTxt("sandbox/from_file/car.txt"));

		m_Car.CreateBuffers(GetDeviceResources()->GetD3DDevice());
		m_Car.CreateShaders(GetDeviceResources()->GetD3DDevice());
	}


	void CarFromFile::OnFrame(uint64_t)
	{
		m_Car.Bind(GetDeviceResources()->GetD3DDeviceContext());
		UpdateScene();
		m_Car.Draw(GetDeviceResources()->GetD3DDeviceContext());
	}


	void CarFromFile::OnImGuiDraw()
	{
		ImGui::DragFloat3("Draw offset", m_DrawOffset);
		ImGui::DragFloat3("Rotation offset", m_RotationOffset);
	}


	void CarFromFile::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Car.d3dConstants_WRP.SetData(
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