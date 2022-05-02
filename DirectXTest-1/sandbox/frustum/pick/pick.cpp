
#include "utils/pch.hpp"
#include "pick.hpp"

#include <d3dcompiler.h>
#include <random>

namespace Pleiades::Sandbox
{
	PickFromMouse::PickFromMouse(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_EffectManager(d3dres->GetD3DDevice()),
		m_Camera(d3dres)
	{
		m_Camera.set_position(0.9f, 3.f, -25.f);
		m_Camera.set_move_speed(30.f);

		InitializeBuffers();
		InitializeShaders();
		InitializeDSS();

		auto& light = m_EffectManager.Buffer().Light;
		light.Ambient = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		light.Direction = DX::XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

		m_SkullMat.Ambient = DX::XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f);
		m_SkullMat.Diffuse = DX::XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		m_SkullMat.Specular = DX::XMVectorSet(0.8f, 0.8f, 0.8f, 16.0f);

		m_SelectMat.Ambient = DX::XMVectorSet(0.0f, 0.8f, 0.4f, 1.0f);
		m_SelectMat.Diffuse = DX::XMVectorSet(0.0f, 0.8f, 0.4f, 1.0f);
		m_SelectMat.Specular = DX::XMVectorSet(0.0f, 0.0f, 0.0f, 16.0f);
	}


	void PickFromMouse::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		m_SkullMesh = GeometryFactory::CreateFromTxt("sandbox/from_file/car.txt");

		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.ByteWidth = uint32_t(m_SkullMesh.vertices.size() * sizeof(m_SkullMesh.vertices[0]));
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subres_data{};
		subres_data.pSysMem = m_SkullMesh.vertices.data();

		// Generate vertex buffer for skull
		DX::ThrowIfFailed(
			d3ddevice->CreateBuffer(
				&buffer_desc,
				&subres_data,
				m_d3dSkullVB.GetAddressOf()
			)
		);

		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.ByteWidth = uint32_t(m_SkullMesh.indices.size() * sizeof(uint16_t));
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		subres_data.pSysMem = m_SkullMesh.indices.data();
		buffer_desc.CPUAccessFlags = 0;

		DX::ThrowIfFailed(
			d3ddevice->CreateBuffer(
				&buffer_desc,
				&subres_data,
				m_d3dSkullIB.GetAddressOf()
			)
		);

		DX::BoundingBox::CreateFromPoints(
			m_SkullBox,
			m_SkullMesh.vertices.size(),
			&m_SkullMesh.vertices[0].position,
			sizeof(m_SkullMesh.vertices[0])
		);
	}


	void PickFromMouse::InitializeShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();
		DX::ComPtr<ID3DBlob> shader_blob;

		// Create vertex shader + input layout
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/pick/pick_vs.cso", shader_blob.GetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateVertexShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_d3dVertexShader.GetAddressOf()
				)
			);

			D3D11_INPUT_ELEMENT_DESC input_desc[]{
				{
					.SemanticName = "LocalPosition",	
					.Format = DXGI_FORMAT_R32G32B32_FLOAT,		
					.InputSlot = 0,
					.AlignedByteOffset = 0,
					.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA, 
					.InstanceDataStepRate = 0 
				},
				{
					.SemanticName = "Normal",			
					.Format = DXGI_FORMAT_R32G32B32_FLOAT,		
					.InputSlot = 0,
					.AlignedByteOffset = sizeof(float[3]),
					.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					.InstanceDataStepRate = 0 
				}
			};

			DX::ThrowIfFailed(
				d3ddevice->CreateInputLayout(
					input_desc,
					static_cast<uint32_t>(std::size(input_desc)),
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					m_d3dInputLayout.GetAddressOf()
				)
			);
		}

		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/pick/pick_ps.cso", shader_blob.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreatePixelShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_d3dPixelShader.GetAddressOf()
				)
			);
		}
	}


	void PickFromMouse::InitializeDSS()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		CD3D11_DEPTH_STENCIL_DESC depth_desc(D3D11_DEFAULT);
		depth_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		DX::ThrowIfFailed(
			d3ddevice->CreateDepthStencilState(
				&depth_desc,
				m_d3dAllowRedraw.GetAddressOf()
			)
		);
	}
}