
#include "utils/pch.hpp"
#include "simple_plane.hpp"

#include "window/Game.hpp"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	SimplePlane::SimplePlane(DX::DeviceResources* d3dres) : 
		ISandbox(d3dres)
	{
		BuildPlaneMesh();

		InitializeBuffers();
		InitializeShaders();
	}


	void SimplePlane::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->IASetIndexBuffer(m_PlaneIndicies.Get(), DXGI_FORMAT_R16_UINT, 0);
		d3dcontext->IASetInputLayout(m_PlaneInputLayout.Get());
		d3dcontext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		uint32_t strides[]{ sizeof(MeshData_t::verticies_type) };
		uint32_t offsets[]{ 0 };

		d3dcontext->IASetVertexBuffers(0, 1, m_PlaneVerticies.GetAddressOf(), strides, offsets);

		d3dcontext->VSSetShader(m_PlaneVtxShader.Get(), nullptr, 0);
		d3dcontext->PSSetShader(m_PlanePxlShader.Get(), nullptr, 0);

		UpdateScene();

		d3dcontext->DrawIndexed(static_cast<uint32_t>(m_PlaneMesh.indicies.size()), 0, 0);
	}


	void SimplePlane::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		// Create vertex buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				m_PlaneMesh.vertices,
				D3D11_BIND_VERTEX_BUFFER,
				m_PlaneVerticies.ReleaseAndGetAddressOf()
			)
		);

		// Create index buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				m_PlaneMesh.indicies,
				D3D11_BIND_INDEX_BUFFER,
				m_PlaneIndicies.ReleaseAndGetAddressOf()
			)
		);

		// Create constant buffer for position
		m_PlaneConstants_WRP.Create(d3ddevice);
	}

	void SimplePlane::InitializeShaders()
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
					m_PlanePxlShader.GetAddressOf()
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
					m_PlaneVtxShader.GetAddressOf()
				)
			);
		}

		// create input layout
		DX::ThrowIfFailed(
			d3ddevice->CreateInputLayout(
				MeshData_t::verticies_type::InputElements,
				MeshData_t::verticies_type::InputElementCount,
				shader_blob->GetBufferPointer(),
				static_cast<uint32_t>(shader_blob->GetBufferSize()),
				m_PlaneInputLayout.GetAddressOf()
			)
		);
	}


	void SimplePlane::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_PlaneConstants_WRP.SetData(
			d3dcontext,
			{
				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixPerspectiveLH(
						1.f, GetDeviceResources()->GetAspectRatio(), 1.f, 120.f
					)
				)
			}
		);

		// set position in vertex shaders
		// set color in pixel shaders
		{
			ID3D11Buffer* buffer[] = { m_PlaneConstants_WRP.GetBuffer() };
			d3dcontext->VSSetConstantBuffers(
				0, 1, &buffer[0]
			);
		}
	}
}