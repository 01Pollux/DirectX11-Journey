
#include "utils/pch.hpp"
#include "bezier_tess.hpp"

#include "imgui/imgui.hpp"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	BezierTesselation::BezierTesselation(DX::DeviceResources* d3dres) :
		ISandbox(d3dres)
	{
		UpdateViewProj();
		CreateBuffers();
		CreateShaders();
	}

	void BezierTesselation::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);

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

		d3dcontext->Draw(16, 0);

		d3dcontext->HSSetShader(nullptr, nullptr, 0);
		d3dcontext->DSSetShader(nullptr, nullptr, 0);
	}


	void BezierTesselation::OnImGuiDraw()
	{
		bool update = ImGui::DragFloat3("Position", &m_WorldConstant.EyePosition.x);
		update |= ImGui::DragFloat3("Rotation", &m_Rotation.x);

		update |= ImGui::DragFloat4("Tess Factor", &m_WorldConstant.TessFactor.x);
		update |= ImGui::DragFloat2("Inside tess Factor", &m_WorldConstant.InsideTessFactor.x);

		if (update)
		{
			UpdateViewProj();
			UpdateConstantBuffer();
		}

		ImGui::Separator();
		update = false;
		constexpr const char* control_pts[]{
			"CP#1",
			"CP#2",
			"CP#3",
			"CP#4"
		};
		
		for (size_t i = 0, group = 1; i < m_VertexInput.size(); i++)
		{
			ImGui::PushID(int(i));

			update |= ImGui::DragFloat3(control_pts[i / 4], &m_VertexInput[i].PosL.x);
			if (!(group++ % 4))
				ImGui::Separator();

			ImGui::PopID();
		}

		if (update)
		{
			auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();
			d3dcontext->UpdateSubresource(
				m_VertexBuffer.Get(),
				0,
				nullptr,
				m_VertexInput.data(),
				uint32_t(m_VertexInput.size() * sizeof(VertexInput_t)),
				0
			);
		}
	}


	void BezierTesselation::CreateBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		// create vertex buffer
		{
			using DX::XMFLOAT3;

			m_VertexInput = {
				// Row 0
				XMFLOAT3(-10.0f, -10.0f, +15.0f),
				XMFLOAT3(-5.0f,  0.0f, +15.0f),
				XMFLOAT3(+5.0f,  0.0f, +15.0f),
				XMFLOAT3(+10.0f, 0.0f, +15.0f),

				// Row 1
				XMFLOAT3(-15.0f, 0.0f, +5.0f),
				XMFLOAT3(-5.0f,  0.0f, +5.0f),
				XMFLOAT3(+5.0f,  20.0f, +5.0f),
				XMFLOAT3(+15.0f, 0.0f, +5.0f),

				// Row 2
				XMFLOAT3(-15.0f, 0.0f, -5.0f),
				XMFLOAT3(-5.0f,  0.0f, -5.0f),
				XMFLOAT3(+5.0f,  0.0f, -5.0f),
				XMFLOAT3(+15.0f, 0.0f, -5.0f),

				// Row 3
				XMFLOAT3(-10.0f, 10.0f, -15.0f),
				XMFLOAT3(-5.0f,  0.0f, -15.0f),
				XMFLOAT3(+5.0f,  0.0f, -15.0f),
				XMFLOAT3(+25.0f, 10.0f, -15.0f)
			};

			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.StructureByteStride = sizeof(m_VertexInput[0]);
			buffer_desc.ByteWidth = uint32_t(m_VertexInput.size() * sizeof(VertexInput_t));
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA buffer_data{};
			buffer_data.pSysMem = m_VertexInput.data();

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


	void BezierTesselation::CreateShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		DX::ComPtr<ID3DBlob> shader_blob;

		// vertex shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(
					L"resources/tesselation/bezier/bezier_vs.cso",
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
					L"resources/tesselation/bezier/bezier_hs.cso",
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
					L"resources/tesselation/bezier/bezier_ds.cso",
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
					L"resources/tesselation/bezier/bezier_ps.cso",
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


	void BezierTesselation::UpdateConstantBuffer()
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