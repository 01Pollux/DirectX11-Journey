
#include "utils/pch.hpp"
#include "cylinder.hpp"

#include "imgui/imgui.hpp"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	RenderableCylinder::RenderableCylinder(DX::DeviceResources* d3dres, const char* name) : 
		IRenderableShape(name, { 0.f, 0.5f, 20.f }, d3dres)
	{
		BuildCylinderMesh();

		InitializeBuffers();
		InitializeShaders();
	}

	void RenderableCylinder::Render(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->IASetIndexBuffer(m_CylinderIndicies.Get(), DXGI_FORMAT_R16_UINT, 0);
		d3dcontext->IASetInputLayout(m_CylinderInputLayout.Get());
		d3dcontext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		uint32_t strides[]{ sizeof(GeometryFactory::MeshData_t::verticies_type) };
		uint32_t offsets[]{ 0 };

		d3dcontext->IASetVertexBuffers(0, 1, m_CylinderVerticies.GetAddressOf(), strides, offsets);

		d3dcontext->VSSetShader(m_CylinderVtxShader.Get(), nullptr, 0);
		d3dcontext->PSSetShader(m_CylinderPxlShader.Get(), nullptr, 0);

		UpdateScene();

		d3dcontext->DrawIndexed(static_cast<uint32_t>(m_CylinderMesh.indicies.size()), 0, 0);
	}

	void RenderableCylinder::ImGuiRender()
	{
		if (ImGui::Button("Update"))
		{
			BuildCylinderMesh();
			InitializeBuffers();
		}

		ImGui::DragFloat3("Draw offset", m_DrawOffset.data());
		ImGui::DragFloat3("Rotation offset", m_RotationOffset);
		ImGui::DragFloat2("Bottom / Top radius", m_CylinderRadius);
		ImGui::DragInt2("Slices/Stacks", m_SlicesStacks);
		ImGui::DragFloat("Height", &m_CylinderHeight);
	}

	void RenderableCylinder::BuildCylinderMesh()
	{
		GeometryFactory::CreateCylinder(
			m_CylinderMesh,
			m_SlicesStacks[0],
			m_SlicesStacks[1],
			m_CylinderRadius[0],
			m_CylinderRadius[1],
			m_CylinderHeight
		);
	}

	void RenderableCylinder::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		// Create vertex buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				m_CylinderMesh.vertices,
				D3D11_BIND_VERTEX_BUFFER,
				m_CylinderVerticies.ReleaseAndGetAddressOf()
			)
		);

		// Create index buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				m_CylinderMesh.indicies,
				D3D11_BIND_INDEX_BUFFER,
				m_CylinderIndicies.ReleaseAndGetAddressOf()
			)
		);

		// Create constant buffer for position
		m_CylinderConstants_WRP.Create(d3ddevice);
	}

	void RenderableCylinder::InitializeShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		DX::ComPtr<ID3DBlob> shader_blob;
		// Create pixel shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/plane/simple_ps.cso", shader_blob.GetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreatePixelShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_CylinderPxlShader.GetAddressOf()
				)
			);
		}

		// Create vertex shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/plane/simple_vs.cso", shader_blob.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateVertexShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_CylinderVtxShader.GetAddressOf()
				)
			);
		}

		// create input layout
		DX::ThrowIfFailed(
			d3ddevice->CreateInputLayout(
				GeometryFactory::MeshData_t::verticies_type::InputElements,
				GeometryFactory::MeshData_t::verticies_type::InputElementCount,
				shader_blob->GetBufferPointer(),
				static_cast<uint32_t>(shader_blob->GetBufferSize()),
				m_CylinderInputLayout.GetAddressOf()
			)
		);
	}

	void RenderableCylinder::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_CylinderConstants_WRP.SetData(
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

		// set position in vertex shaders
		// set color in pixel shaders
		{
			ID3D11Buffer* buffer[] = { m_CylinderConstants_WRP.GetBuffer() };
			d3dcontext->VSSetConstantBuffers(
				0, 1, &buffer[0]
			);
		}
	}
}