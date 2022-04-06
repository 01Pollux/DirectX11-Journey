
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
		BuildSkullFromFile();

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


	void SkullFromFile::BuildSkullFromFile()
	{
		std::ifstream file("sandbox/from_file/skull.txt");

		size_t vertex_count, idx_count;
		std::string ignore;

		file >> ignore >> vertex_count;
		file >> ignore >> idx_count;

		std::getline(file, ignore); // \n
		std::getline(file, ignore); // VertexLisst
		std::getline(file, ignore); // {

		GeometryInstance::MeshData_t mesh;
		mesh.vertices.reserve(vertex_count);
		mesh.indices.resize(idx_count * 3);

		while (vertex_count--)
		{
			float x, y, z;
			file >> x >> y >> z;
			std::getline(file, ignore);

			mesh.vertices.emplace_back(
				DX::XMFLOAT3(x, y, z),
				DX::XMFLOAT4(DX::Colors::Black)
			);
		}

		std::getline(file, ignore); // }
		std::getline(file, ignore); // TriangleList
		std::getline(file, ignore); // {

		for (size_t i = 0; i < idx_count; i++)
		{
			uint32_t i0, i1, i2;
			file >> i0 >> i1 >> i2;

			mesh.indices[i * 3 + 0] = static_cast<uint16_t>(i0);
			mesh.indices[i * 3 + 1] = static_cast<uint16_t>(i1);
			mesh.indices[i * 3 + 2] = static_cast<uint16_t>(i2);
		}

		m_Skull.PushMesh(std::move(mesh));
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