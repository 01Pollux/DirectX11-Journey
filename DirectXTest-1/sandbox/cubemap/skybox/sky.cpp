
#include "utils/pch.hpp"
#include "sky.hpp"

#include "DirectXTK/DDSTextureLoader.h"
#include "sandbox/render/cbuffer.hpp"
#include "cbuffers.hpp"


namespace Pleiades::Sandbox::SkyboxCubeMapDemo
{
	Skybox::Skybox(
		DX::Camera* camera,
		DX::DeviceResources* d3dres,
		const wchar_t* cubetexture_path,
		float radius
	) :
		IRenderable(camera, d3dres, GeometryFactory::CreateSphere(30, 30, radius))
	{
		this->CreateBuffers();
		this->CreateShaders();
		this->CreateResources(cubetexture_path);

		this->SetDrawInfo(
			DrawInfoIndexed_t{
				.IndexCount = uint32_t(m_MeshInfo.indices.size()),
			}
		);

		this->FreeMeshInfo();
	}


	void Skybox::BeginDraw(RenderConstantBuffer_t& cbuffer)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();
		
		ID3D11Buffer* buffers[]{ m_d3dVB_Sky.Get() };
		uint32_t strides[]{ sizeof(VertexInput_t) };
		uint32_t offsets[]{ 0 };


		d3dcontext->IASetVertexBuffers(
			0, 1, buffers, strides, offsets
		);

		d3dcontext->IASetIndexBuffer(
			m_d3dIB_Sky.Get(), DXGI_FORMAT_R16_UINT, 0
		);

		d3dcontext->IASetInputLayout(
			m_d3dIL_Sky.Get()
		);

		d3dcontext->VSSetShader(
			m_d3dVS_Sky.Get(),
			nullptr, 0
		);
		
		d3dcontext->PSSetShader(
			m_d3dPS_Sky.Get(),
			nullptr, 0
		);
		
		d3dcontext->PSSetShaderResources(
			0,
			1,
			m_d3dSRV_SkyBox.GetAddressOf()
		);

		d3dcontext->PSSetSamplers(
			0,
			1,
			m_d3dSampler_Sky.GetAddressOf()
		);
		
		d3dcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		d3dcontext->OMSetDepthStencilState(
			m_d3dLessEqualDDS.Get(),
			0
		);

		UpdateWorld(cbuffer);
		cbuffer.Update();
	}


	void Skybox::EndDraw(RenderConstantBuffer_t& cbuffer)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->OMSetDepthStencilState(
			nullptr,
			0
		);
	}


	void Skybox::UpdateWorld(RenderConstantBuffer_t& cbuffer)
	{
		const auto& pos = GetCamera()->position();

		const auto& world = DX::XMMatrixTranslationFromVector(DX::XMLoadFloat3(&GetCamera()->position()));
		const auto& view = GetCamera()->get_view();
		const auto& projection = GetCamera()->get_projection();

		cbuffer.Data<WorldConstantBuffers_t>(0)->WorldViewProj = DX::XMMatrixTranspose(world * view * projection);

		cbuffer.MarkDirty(0);
	}


	void Skybox::CreateBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		// Create vertex buffer
		{
			std::vector<VertexInput_t> vertices(m_MeshInfo.vertices.size());

			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.ByteWidth = uint32_t(vertices.size() * sizeof(VertexInput_t));
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			std::transform(
				m_MeshInfo.vertices.begin(), 
				m_MeshInfo.vertices.end(), 
				vertices.begin(), 
				[](const auto& vertex) { return VertexInput_t{ vertex.position }; }
			);

			D3D11_SUBRESOURCE_DATA subres_data{};
			subres_data.pSysMem = vertices.data();

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_data,
					m_d3dVB_Sky.GetAddressOf()
				)
			);
		}

		// Create index buffer
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.ByteWidth = uint32_t(m_MeshInfo.indices.size() * sizeof(uint16_t));
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA subres_data{};
			subres_data.pSysMem = m_MeshInfo.indices.data();

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_data,
					m_d3dIB_Sky.GetAddressOf()
				)
			);
		}
	}


	void Skybox::CreateShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();
		DX::ComPtr<ID3DBlob> shader_file;

		// Create vertex shader + input layout
		{
			this->ReadShader(
				L"resources/skybox/skybox_vs.cso",
				shader_file.GetAddressOf()
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateVertexShader(
					shader_file->GetBufferPointer(),
					shader_file->GetBufferSize(),
					nullptr,
					m_d3dVS_Sky.GetAddressOf()
				)
			);

			D3D11_INPUT_ELEMENT_DESC input_layout[]
			{
				{
					.SemanticName = "WorldPosition",
					.Format = DXGI_FORMAT_R32G32B32_FLOAT
				}
			};

			DX::ThrowIfFailed(
				d3ddevice->CreateInputLayout(
					input_layout,
					static_cast<uint32_t>(std::size(input_layout)),
					shader_file->GetBufferPointer(),
					shader_file->GetBufferSize(),
					m_d3dIL_Sky.GetAddressOf()
				)
			);
		}

		// Create pixel shader 
		{
			this->ReadShader(
				L"resources/skybox/skybox_ps.cso",
				shader_file.ReleaseAndGetAddressOf()
			);

			DX::ThrowIfFailed(
				d3ddevice->CreatePixelShader(
					shader_file->GetBufferPointer(),
					shader_file->GetBufferSize(),
					nullptr,
					m_d3dPS_Sky.GetAddressOf()
				)
			);
		}
	}


	void Skybox::CreateResources(
		const wchar_t* cubetexture_path
	)
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				d3ddevice,
				cubetexture_path,
				nullptr,
				m_d3dSRV_SkyBox.GetAddressOf()
			)
		);

		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU =
			sampler_desc.AddressV =
			sampler_desc.AddressW =
			D3D11_TEXTURE_ADDRESS_WRAP;

		DX::ThrowIfFailed(
			d3ddevice->CreateSamplerState(
				&sampler_desc,
				m_d3dSampler_Sky.GetAddressOf()
			)
		);


		CD3D11_DEPTH_STENCIL_DESC dss_desc(D3D11_DEFAULT);
		dss_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		DX::ThrowIfFailed(
			d3ddevice->CreateDepthStencilState(
				&dss_desc,
				m_d3dLessEqualDDS.GetAddressOf()
			)
		);
	}
}