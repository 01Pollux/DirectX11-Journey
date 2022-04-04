
#include "utils/pch.hpp"
#include "geometry_factory.hpp"

#include <d3dcompiler.h>

namespace Pleiades
{
	void GeometryFactory::CreatePlaneVertices(
		MeshData_t& mesh,
		uint32_t rows,
		uint32_t columns,
		float width,
		float height
	)
	{
		//size_t vertex_count = (columns - 1) * (rows - 1);
		//size_t triangle_count = vertex_count * 2;

		float x_step = 1.f / columns;
		float z_step = 1.f / rows;

		float dx = width * x_step;
		float dz = height * z_step;

		float neg_half_width = width * -0.5f;
		float half_height = height * 0.5f;

		/*
						Z = height
		(-0.5, 0.5)		 |
						 |
						 |
						 |
						 |
		---------------(0,0)---------------- X = width
						 |
						 |
						 |
						 |		(0.5, -0.5)

		[-0.5 * width + x * dx, 0.5 * height - y * dy, 0.f]
		*/

		mesh.vertices.clear();
		mesh.vertices.reserve(static_cast<size_t>(rows) * columns);

		for (size_t z = 0; z < rows; z++)
		{
			float cur_z = half_height - z * dz;
			for (size_t x = 0; x < columns; x++)
			{
				float cur_x = neg_half_width + x * dx;
				mesh.vertices.emplace_back(
					DX::XMFLOAT3{ cur_x, 0.f, cur_z },
					DX::XMFLOAT4(DX::Colors::Magenta)
				);
			}
		}
	}

	void GeometryFactory::CreatePlaneIndicies(
		MeshData_t& mesh,
		uint32_t rows,
		uint32_t columns
	)
	{
		mesh.indicies.clear();
		mesh.indicies.reserve(static_cast<size_t>(columns - 1) * (rows - 1) * 6);

		for (uint16_t i = 0; i < rows - 1; i++)
		{
			for (uint16_t j = 0; j < columns - 1; j++)
			{
				mesh.indicies.push_back(static_cast<uint16_t>(i * columns + j));
				mesh.indicies.push_back(static_cast<uint16_t>(i * columns + j + 1));
				mesh.indicies.push_back(static_cast<uint16_t>((i+ 1) * columns + j));

				mesh.indicies.push_back(static_cast<uint16_t>((i + 1) * columns + j));
				mesh.indicies.push_back(static_cast<uint16_t>(i * columns + j + 1));
				mesh.indicies.push_back(static_cast<uint16_t>((i + 1) * columns + j + 1));
			}
		}
	}



	void GeometryFactory::CreateCylinderVerticesAndIndicies(
		MeshData_t& mesh, 
		uint32_t slices, 
		uint32_t stacks, 
		float bottom_radius,
		float top_radius, 
		float height
	)
	{
		const float stack_height = height / stacks;
		const float radius_step = (top_radius - bottom_radius) / stacks;

		const float theta = DX::XM_2PI / slices;

		mesh.vertices.clear();
		mesh.vertices.reserve(static_cast<size_t>(stacks + 1) * (slices + 1));

		for (uint32_t i = 0; i <= stacks; i++)
		{
			const float cur_y = height * -0.5f + i * stack_height;
			const float cur_radius = bottom_radius + i * radius_step;

			for (uint32_t j = 0; j <= slices; j++)
			{
				float c, s;
				DX::XMScalarSinCos(&s, &c, j * theta);

				mesh.vertices.emplace_back(
					DX::XMFLOAT3{ c * cur_radius, cur_y, s * cur_radius },
					DX::XMFLOAT4(DX::Colors::Magenta)
				);
			}
		}

		CreateCylinderIndicies(
			mesh,
			slices,
			stacks
		);

		CreateCylinderTopFace(
			mesh,
			slices,
			top_radius,
			height
		);

		CreateCylinderBottomFace(
			mesh,
			slices,
			bottom_radius,
			height
		);
	}

	void GeometryFactory::CreateCylinderTopFace(
		MeshData_t& mesh,
		uint32_t slices,
		float top_radius,
		float height
	)
	{
		const uint32_t base_i = static_cast<uint32_t>(mesh.vertices.size()) + 1;

		const float theta = DX::XM_2PI / slices;
		const float y = height * 0.5f;

		mesh.vertices.reserve(mesh.vertices.size() + slices + 2);

		mesh.vertices.emplace_back(
			DX::XMFLOAT3{ 0, y, 0 },
			DX::XMFLOAT4(DX::Colors::Magenta)
		);

		for (uint32_t i = 0; i <= slices; i++)
		{
			float x, z;
			DX::XMScalarSinCos(&z, &x, i * theta);

			mesh.vertices.emplace_back(
				DX::XMFLOAT3{ top_radius * x, y, top_radius * z },
				DX::XMFLOAT4(DX::Colors::Magenta)
			);
		}

		for (uint16_t i = 0; i < slices; i++)
		{
			mesh.indicies.push_back(static_cast<uint16_t>(base_i - 1));
			mesh.indicies.push_back(static_cast<uint16_t>(base_i + i + 1));
			mesh.indicies.push_back(static_cast<uint16_t>(base_i + i));
		}
	}

	void GeometryFactory::CreateCylinderBottomFace(
		MeshData_t& mesh, 
		uint32_t slices, 
		float bottom_radius, 
		float height
	)
	{
		const uint32_t base_i = static_cast<uint32_t>(mesh.vertices.size()) + 1;

		const float theta = DX::XM_2PI / slices;
		const float y = height * -0.5f;

		mesh.vertices.reserve(mesh.vertices.size() + slices + 2);

		mesh.vertices.emplace_back(
			DX::XMFLOAT3{ 0, y, 0 },
			DX::XMFLOAT4(DX::Colors::Magenta)
		);

		for (uint32_t i = 0; i <= slices; i++)
		{
			float x, z;
			DX::XMScalarSinCos(&z, &x, i * theta);

			mesh.vertices.emplace_back(
				DX::XMFLOAT3{ bottom_radius * x, y, bottom_radius * z },
				DX::XMFLOAT4(DX::Colors::Magenta)
			);
		}

		for (uint16_t i = 0; i < slices; i++)
		{
			mesh.indicies.push_back(static_cast<uint16_t>(base_i - 1));
			mesh.indicies.push_back(static_cast<uint16_t>(base_i + i));
			mesh.indicies.push_back(static_cast<uint16_t>(base_i + i + 1));
		}
	}

	void GeometryFactory::CreateCylinderIndicies(
		MeshData_t& mesh, 
		uint32_t slices, 
		uint32_t stacks
	)
	{
		mesh.indicies.clear();
		mesh.indicies.reserve(static_cast<size_t>(stacks) * slices + mesh.indicies.size());

		const uint32_t ring_count = slices + 1;

		for (uint16_t i = 0; i < stacks; i++)
		{
			for (uint16_t j = 0; j < slices; j++)
			{
				mesh.indicies.push_back(static_cast<uint16_t>(i * ring_count + j));
				mesh.indicies.push_back(static_cast<uint16_t>((i + 1) * ring_count + j));
				mesh.indicies.push_back(static_cast<uint16_t>((i + 1) * ring_count + j + 1));

				mesh.indicies.push_back(static_cast<uint16_t>(i * ring_count + j));
				mesh.indicies.push_back(static_cast<uint16_t>((i + 1) * ring_count + j + 1));
				mesh.indicies.push_back(static_cast<uint16_t>(i * ring_count + j + 1));
			}
		}
	}


	void GeometryInstance::CreateBuffers(ID3D11Device* d3ddevice)
	{
		// Create vertex buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				Mesh.vertices,
				D3D11_BIND_VERTEX_BUFFER,
				d3dVerticies.ReleaseAndGetAddressOf()
			)
		);

		// Create index buffer
		DX::ThrowIfFailed(
			DX::CreateStaticBuffer(
				d3ddevice,
				Mesh.indicies,
				D3D11_BIND_INDEX_BUFFER,
				d3dIndicies.ReleaseAndGetAddressOf()
			)
		);

		// Create constant buffer for position
		d3dConstants_WRP.Create(d3ddevice);
	}


	void GeometryInstance::CreateShaders(ID3D11Device* d3ddevice)
	{
		DX::ComPtr<ID3DBlob> shader_blob;
		// Create pixel shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/geometry_default_ps.cso", shader_blob.GetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreatePixelShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					d3dPxlShader.GetAddressOf()
				)
			);
		}

		// Create vertex shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(L"resources/geometry_default_vs.cso", shader_blob.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateVertexShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					d3dVtxShader.GetAddressOf()
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
				d3dInputLayout.GetAddressOf()
			)
		);
	}


	void GeometryInstance::Bind(ID3D11DeviceContext* d3dcontext)
	{
		d3dcontext->IASetIndexBuffer(d3dIndicies.Get(), DXGI_FORMAT_R16_UINT, 0);
		d3dcontext->IASetInputLayout(d3dInputLayout.Get());
		d3dcontext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		uint32_t strides[]{ sizeof(MeshData_t::verticies_type) };
		uint32_t offsets[]{ 0 };

		d3dcontext->IASetVertexBuffers(0, 1, d3dVerticies.GetAddressOf(), strides, offsets);

		d3dcontext->VSSetShader(d3dVtxShader.Get(), nullptr, 0);
		ID3D11Buffer* buffer[] = { d3dConstants_WRP.GetBuffer() };
		d3dcontext->VSSetConstantBuffers(0, 1, buffer);

		d3dcontext->PSSetShader(d3dPxlShader.Get(), nullptr, 0);
	}

}