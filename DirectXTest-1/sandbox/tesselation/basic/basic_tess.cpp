
#include "utils/pch.hpp"
#include "basic_tess.hpp"

#include "imgui/imgui.hpp"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	BasicTesselation::BasicTesselation(DX::DeviceResources* d3dres) :
		ISandbox(d3dres)
	{
		UpdateViewProj();
		CreateBuffers();
		CreateShaders();
	}

	void BasicTesselation::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

		uint32_t strides[]{ sizeof(VertexInput_t) };
		uint32_t offsets[]{ 0 };
		d3dcontext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), strides, offsets);
		d3dcontext->IASetInputLayout(m_InputLayout.Get());

		d3dcontext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
		d3dcontext->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());

		d3dcontext->HSSetShader(m_HullShader.Get(), nullptr, 0);
		d3dcontext->HSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());

		d3dcontext->DSSetShader(m_DomainShader.Get(), nullptr, 0);
		d3dcontext->DSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());

		d3dcontext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
		d3dcontext->PSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());

		d3dcontext->Draw(4, 0);

		d3dcontext->HSSetShader(nullptr, nullptr, 0);
		d3dcontext->DSSetShader(nullptr, nullptr, 0);
	}


	void BasicTesselation::OnImGuiDraw()
	{
		bool update = ImGui::DragFloat3("Position", &m_WorldConstant.EyePosition.x);
		update |= ImGui::DragFloat3("Rotation", &m_Rotation.x);

		if (update)
		{
			UpdateViewProj();
			UpdateConstantBuffer();
		}
	}


	void BasicTesselation::CreateBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		// create vertex buffer
		{
			using DX::XMFLOAT3;

			VertexInput_t vs_input[]{
				XMFLOAT3{ -10.f, 0.f, +10.f },
				XMFLOAT3{ +10.f, 0.f, +10.f },
				XMFLOAT3{ -10.f, 0.f, -10.f }
			};

			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth = sizeof(vs_input);
			buffer_desc.StructureByteStride = sizeof(vs_input[0]);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA buffer_data{};
			buffer_data.pSysMem = vs_input;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&buffer_data,
					m_VertexBuffer.GetAddressOf()
				)
			);
		}

		// create constant buffer
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth = sizeof(m_WorldConstant);
			buffer_desc.StructureByteStride = sizeof(m_WorldConstant);
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			D3D11_SUBRESOURCE_DATA buffer_data{};
			buffer_data.pSysMem = &m_WorldConstant;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&buffer_data,
					m_ConstantBuffer.GetAddressOf()
				)
			);
		}
	}


	void BasicTesselation::CreateShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		DX::ComPtr<ID3DBlob> shader_blob;

		// vertex shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(
					L"resources/tesselation/basic/basic_vs.cso",
					shader_blob.ReleaseAndGetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateVertexShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_VertexShader.GetAddressOf()
				)
			);

			D3D11_INPUT_ELEMENT_DESC input_desc[]{
				{
					.SemanticName = "Position",
					.Format = DXGI_FORMAT_R32G32B32_FLOAT,
					.InputSlot = 0
				}
			};

			DX::ThrowIfFailed(
				d3ddevice->CreateInputLayout(
					input_desc,
					1,
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					m_InputLayout.GetAddressOf()
				)
			);
		}

		// hull shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(
					L"resources/tesselation/basic/basic_hs.cso",
					shader_blob.ReleaseAndGetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateHullShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_HullShader.GetAddressOf()
				)
			);
		}

		// domain shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(
					L"resources/tesselation/basic/basic_ds.cso",
					shader_blob.ReleaseAndGetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateDomainShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_DomainShader.GetAddressOf()
				)
			);
		}

		// pixel shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(
					L"resources/tesselation/basic/basic_ps.cso",
					shader_blob.ReleaseAndGetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreatePixelShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_PixelShader.GetAddressOf()
				)
			);
		}
	}


	void BasicTesselation::UpdateConstantBuffer()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->UpdateSubresource(
			m_ConstantBuffer.Get(),
			0,
			nullptr,
			&m_WorldConstant,
			sizeof(m_WorldConstant),
			0
		);
	}
}