
#include "utils/pch.hpp"
#include "directx/DeviceResources.hpp"

#include "shapes.hpp"

#include "imgui/imgui.hpp"
#include "cube3d.hpp"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	struct Cube3D_Vertex
	{
		DirectX::XMVECTOR Position;
	};
	static_assert(sizeof(Cube3D_Vertex) == sizeof(float[4]));

	struct Cube3D_ConstantPosition
	{
		DirectX::XMMATRIX Transformation;
	};

	struct Cube3D_SingleColorInSquare
	{
		float Color[4];
	};


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

		d3dcontext->IASetIndexBuffer(m_CubeIBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		d3dcontext->IASetInputLayout(m_CubeIL.Get());
		d3dcontext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		uint32_t strides[]{ sizeof(Cube3D_Vertex) };
		uint32_t offsets[]{ 0 };

		d3dcontext->IASetVertexBuffers(0, 1, m_CubeVBuffer.GetAddressOf(), strides, offsets);

		d3dcontext->VSSetShader(m_CubeVS.Get(), nullptr, 0);
		d3dcontext->PSSetShader(m_CubePS.Get(), nullptr, 0);

		m_CurAngle += static_cast<float>(StepTimer::TicksToSeconds(ticks) * 2);
		Rotate();
		if (m_CurAngle >= 360.f)
			m_CurAngle = 0.f;

		d3dcontext->DrawIndexed(9 * 4, 0, 0);
	}


	void RenderableCube3D::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		constexpr Cube3D_Vertex position[]{
			{ -1.f, -1.f, -1.f, 1.f },
			{  1.f, -1.f, -1.f, 1.f },
			{ -1.f,  1.f, -1.f, 1.f },
			{  1.f,  1.f, -1.f, 1.f },
			{ -1.f, -1.f,  1.f, 1.f },
			{  1.f, -1.f,  1.f, 1.f },
			{ -1.f,  1.f,  1.f, 1.f },
			{  1.f,  1.f,  1.f, 1.f },
		};

		constexpr unsigned short indicies[]
		{
			0, 2, 1,	2, 3, 1,
			1, 3, 5,	3, 7, 5,
			2, 6, 3,	3, 6, 7,
			4, 5, 7,	4, 7, 6,
			0, 4, 2,	2, 4, 6,
			0, 1, 4,	1, 5, 4
		};

		static Cube3D_SingleColorInSquare colors[]
		{
			{ 1.f, 0.f, 1.f },
			{ 1.f, 0.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 0.f, 1.f },
			{ 1.f, 1.f, 0.f },
			{ 0.f, 1.f, 1.f },
		};

		// Create vertex buffer
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.ByteWidth = sizeof(position);
			buffer_desc.StructureByteStride = sizeof(position[0]);

			D3D11_SUBRESOURCE_DATA subres_data{};
			subres_data.pSysMem = position;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_data,
					m_CubeVBuffer.GetAddressOf()
				)
			);
		}

		// Create index buffer
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.ByteWidth = sizeof(indicies);
			buffer_desc.StructureByteStride = sizeof(indicies[0]);

			D3D11_SUBRESOURCE_DATA subres_data{};
			subres_data.pSysMem = indicies;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_data,
					m_CubeIBuffer.GetAddressOf()
				)
			);
		}

		// Create constant buffer for position
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			buffer_desc.ByteWidth = sizeof(Cube3D_ConstantPosition);
			buffer_desc.StructureByteStride = sizeof(DirectX::XMMATRIX);

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					nullptr,
					m_CubeCBuffer_Position.GetAddressOf()
				)
			);
		}

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
					m_CubePS.GetAddressOf()
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
					m_CubeVS.GetAddressOf()
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
				m_CubeIL.GetAddressOf()
			)
		);
	}


	void RenderableCube3D::Rotate()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();
		// set color in pixel shaders
		{
			d3dcontext->PSSetConstantBuffers(
				1 /* register(b1) */, 1, m_CubeCBuffer_SingleColor.GetAddressOf()
			);
		}

		Cube3D_ConstantPosition triangle_buffer
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
		};

		D3D11_MAPPED_SUBRESOURCE cdata{};
		DX::ThrowIfFailed(
			d3dcontext->Map(
				m_CubeCBuffer_Position.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&cdata
			)
		);

		std::memcpy(cdata.pData, &triangle_buffer, sizeof(triangle_buffer));

		d3dcontext->Unmap(
			m_CubeCBuffer_Position.Get(),
			0
		);


		// set position in vertex shaders
		{
			auto buffer = m_CubeCBuffer_Position.Get();
			d3dcontext->VSSetConstantBuffers(
				0, 1, &buffer
			);
		}
	}
}