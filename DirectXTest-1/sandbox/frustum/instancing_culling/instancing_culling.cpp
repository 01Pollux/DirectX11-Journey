
#include "utils/pch.hpp"
#include "instancing_culling.hpp"

#include "sandbox/geometry_factory.hpp"

#include <d3dcompiler.h>
#include <random>

namespace Pleiades::Sandbox
{
	InstancedFrustum::InstancedFrustum(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_EffectManager(d3dres->GetD3DDevice()),
		m_Camera(d3dres)
	{
		m_Camera.set_position(145.f, 145.f, -15.f);

		InitializeInstancedGround();
		InitializeBuffers();
		InitializeShaders();

		auto& light = m_EffectManager.Buffer().Light;
		light.Ambient = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		light.Direction = DX::XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

		m_SkullMat.Ambient = DX::XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f);
		m_SkullMat.Diffuse = DX::XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		m_SkullMat.Specular = DX::XMVectorSet(0.8f, 0.8f, 0.8f, 16.0f);
	}


	void InstancedFrustum::InitializeInstancedGround()
	{
		constexpr float width = 200.f, height = 200.f, depth = 200.f;
		constexpr int count = 5;

		constexpr float dx = width / count;
		constexpr float dy = height / count;
		constexpr float dz = depth / count;

		constexpr float x = width * .5f;
		constexpr float y = height * .5f;
		constexpr float z = depth * .5f;

		m_InstancedWorld.resize(count * count * count);

		std::mt19937_64 engine;
		std::uniform_real<float> rand_f(0.f, 1.f);

		for (size_t i = 0; i < count; i++)
		{
			for (size_t j = 0; j < count; j++)
			{
				for (size_t k = 0; k < count; k++)
				{
					auto& inst = m_InstancedWorld[i * count * count + j * count + k];
					inst.World = DX::XMMatrixTranspose(DX::XMMatrixTranslation(x + j * dx, y + i * dy, z + k * dz));
					inst.Color = DX::XMFLOAT4(rand_f(engine), rand_f(engine), rand_f(engine), 1.f);
				}
			}
		}
	}

	void InstancedFrustum::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		auto mesh = GeometryFactory::CreateFromTxt("sandbox/from_file/skull.txt");

		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.ByteWidth = uint32_t(mesh.vertices.size() * sizeof(mesh.vertices[0]));
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subres_data{};
		subres_data.pSysMem = mesh.vertices.data();

		// Generate vertex buffer for skull
		DX::ThrowIfFailed(
			d3ddevice->CreateBuffer(
				&buffer_desc,
				&subres_data,
				m_d3dSkullVB.GetAddressOf()
			)
		);


		// Generate vertex buffer for world's ground
		buffer_desc.ByteWidth = uint32_t(m_InstancedWorld.size() * sizeof(m_InstancedWorld[0]));
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		DX::ThrowIfFailed(
			d3ddevice->CreateBuffer(
				&buffer_desc,
				nullptr,
				m_d3dInstWorldVB.GetAddressOf()
			)
		);


		m_IndexCount = uint32_t(mesh.indices.size());
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.ByteWidth = uint32_t(mesh.indices.size() * sizeof(uint16_t));
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		subres_data.pSysMem = mesh.indices.data();
		buffer_desc.CPUAccessFlags = 0;

		DX::ThrowIfFailed(
			d3ddevice->CreateBuffer(
				&buffer_desc,
				&subres_data,
				m_IndexBuffer.GetAddressOf()
			)
		);
	}


	void InstancedFrustum::InitializeShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();
		DX::ComPtr<ID3DBlob> shader_blob;

		// Create vertex shader + input layout
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/frustum/frustum_vs.cso", shader_blob.GetAddressOf())
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
				},
				{
					.SemanticName = "World",
					.SemanticIndex = 0,
					.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
					.InputSlot = 1,
					.AlignedByteOffset = 0,
					.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
					.InstanceDataStepRate = 1
				},
				{
					.SemanticName = "World",
					.SemanticIndex = 1,
					.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
					.InputSlot = 1,
					.AlignedByteOffset = sizeof(float[4]),
					.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
					.InstanceDataStepRate = 1
				},
				{
					.SemanticName = "World",
					.SemanticIndex = 2,
					.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
					.InputSlot = 1,
					.AlignedByteOffset = sizeof(float[4]) * 2,
					.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
					.InstanceDataStepRate = 1
				},
				{
					.SemanticName = "World",
					.SemanticIndex = 3,
					.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
					.InputSlot = 1,
					.AlignedByteOffset = sizeof(float[4]) * 3,
					.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
					.InstanceDataStepRate = 1
				},
				{
					.SemanticName = "Color",	
					.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
					.InputSlot = 1,
					.AlignedByteOffset = sizeof(float[4]) * 4,
					.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
					.InstanceDataStepRate = 1
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
				D3DReadFileToBlob(L"resources/frustum/frustum_ps.cso", shader_blob.ReleaseAndGetAddressOf())
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
}