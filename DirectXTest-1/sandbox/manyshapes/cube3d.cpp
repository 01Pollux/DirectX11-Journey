
#include "utils/pch.hpp"
#include "directx/DeviceResources.hpp"

#include "shapes.hpp"

#include "imgui/imgui.hpp"
#include "cube3d.hpp"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	RenderableCube3D::RenderableCube3D(DX::DeviceResources* d3dres, const char* name) :
		IRenderableShape(name, { 0.f, 0.5f, 20.f }, d3dres)
	{
		// Create index and vertex buffer
		InitializeBuffers();
		// Create vertex shader, pixel shader, and input layout
		InitializeShaders();
	}


	void RenderableCube3D::Render(uint64_t ticks)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->IASetIndexBuffer(m_CubeIndicies.Get(), DXGI_FORMAT_R16_UINT, 0);
		d3dcontext->IASetInputLayout(m_CubeInputLayout.Get());
		d3dcontext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		uint32_t strides[]{ sizeof(Vertex_t) };
		uint32_t offsets[]{ 0 };

		d3dcontext->IASetVertexBuffers(0, 1, m_CubeVerticies.GetAddressOf(), strides, offsets);


		d3dcontext->VSSetShader(m_CubeVtxShader.Get(), nullptr, 0);
		d3dcontext->PSSetShader(m_CubePxlShader.Get(), nullptr, 0);

		m_CurAngle += static_cast<float>(StepTimer::TicksToSeconds(ticks) * 2);
		Rotate();
		if (m_CurAngle >= 360.f)
			m_CurAngle = 0.f;

		d3dcontext->DrawIndexed(36, 0, 0);
	}


	void RenderableCube3D::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		constexpr std::array positions{
			Vertex_t{ { -1.f, -1.f, -1.f, 1.f } },
			Vertex_t{ {  1.f, -1.f, -1.f, 1.f } },
			Vertex_t{ { -1.f,  1.f, -1.f, 1.f } },
			Vertex_t{ {  1.f,  1.f, -1.f, 1.f } },
			Vertex_t{ { -1.f, -1.f,  1.f, 1.f } },
			Vertex_t{ {  1.f, -1.f,  1.f, 1.f } },
			Vertex_t{ { -1.f,  1.f,  1.f, 1.f } },
			Vertex_t{ {  1.f,  1.f,  1.f, 1.f } },
		};

		constexpr std::array<unsigned short, 36> indicies
		{
			0, 2, 1,	2, 3, 1,
			1, 3, 5,	3, 7, 5,
			2, 6, 3,	3, 6, 7,
			4, 5, 7,	4, 7, 6,
			0, 4, 2,	2, 4, 6,
			0, 1, 4,	1, 5, 4
		};

		static SingleColorInSquare_t colors[]
		{
			{ 1.f, 0.f, 1.f },
			{ 1.f, 0.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 0.f, 1.f },
			{ 1.f, 1.f, 0.f },
			{ 0.f, 1.f, 1.f },
		};

		// Create vertex buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				positions,
				D3D11_BIND_VERTEX_BUFFER,
				m_CubeVerticies.GetAddressOf()
			)
		);

		// Create index buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				indicies,
				D3D11_BIND_INDEX_BUFFER,
				m_CubeIndicies.GetAddressOf()
			)
		);

		// Create constant buffer for position
		m_CubeCBuffer_Position.Create(d3ddevice);
		
		// Create constant buffer for color
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.ByteWidth = sizeof(colors);
			buffer_desc.StructureByteStride = sizeof(colors[0]);

			D3D11_SUBRESOURCE_DATA subres_data{};
			subres_data.pSysMem = colors;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_data,
					m_CubeCBuffer_SingleColor.GetAddressOf()
				)
			);
		}
	}


	void RenderableCube3D::InitializeShaders()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		DX::ComPtr<ID3DBlob> shader_blob;
		// Create pixel shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/manyshapes/cube3d_ps.cso", shader_blob.GetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreatePixelShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_CubePxlShader.GetAddressOf()
				)
			);
		}

		// Create vertex shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/manyshapes/cube3d_vs.cso", shader_blob.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateVertexShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_CubeVtxShader.GetAddressOf()
				)
			);
		}

		const D3D11_INPUT_ELEMENT_DESC triangle_input[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
		};

		// create input layout
		DX::ThrowIfFailed(
			d3ddevice->CreateInputLayout(
				triangle_input,
				static_cast<uint32_t>(std::size(triangle_input)),
				shader_blob->GetBufferPointer(),
				static_cast<uint32_t>(shader_blob->GetBufferSize()),
				m_CubeInputLayout.GetAddressOf()
			)
		);
	}


	void RenderableCube3D::Rotate()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_CubeCBuffer_Position.SetData(
			d3dcontext,
			{
				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixRotationY(m_CurAngle) *
					DirectX::XMMatrixRotationX(m_CurAngle) *
					DirectX::XMMatrixTranslation(
						m_DrawOffset[0],
						m_DrawOffset[1],
						m_DrawOffset[2]
					) *
					DirectX::XMMatrixPerspectiveLH(
						1.f, GetDeviceResources()->GetAspectRatio(), 1.f, 120.f
					)
				)
			}
		);

		// set position in vertex shaders
		// set color in pixel shaders
		{
			ID3D11Buffer* buffer[] = { m_CubeCBuffer_Position.GetBuffer(), m_CubeCBuffer_SingleColor.Get() };
			d3dcontext->VSSetConstantBuffers(
				0, 1, &buffer[0]
			);
			
			d3dcontext->PSSetConstantBuffers(
				1, 1, &buffer[1]
			);
		}
	}
}