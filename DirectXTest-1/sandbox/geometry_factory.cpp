
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
					DX::XMFLOAT3{ 0.f, 1.f, 0.f },
					DX::XMFLOAT2{ x * x_step, z * z_step }
				);
			}
		}
	}

	void GeometryFactory::CreatePlaneIndices(
		MeshData_t& mesh,
		uint32_t rows,
		uint32_t columns
	)
	{
		mesh.indices.clear();
		mesh.indices.reserve(static_cast<size_t>(columns - 1) * (rows - 1) * 6);

		for (uint16_t i = 0; i < rows - 1; i++)
		{
			for (uint16_t j = 0; j < columns - 1; j++)
			{
				mesh.indices.push_back(static_cast<uint16_t>(i * columns + j));
				mesh.indices.push_back(static_cast<uint16_t>(i * columns + j + 1));
				mesh.indices.push_back(static_cast<uint16_t>((i + 1) * columns + j));

				mesh.indices.push_back(static_cast<uint16_t>((i + 1) * columns + j));
				mesh.indices.push_back(static_cast<uint16_t>(i * columns + j + 1));
				mesh.indices.push_back(static_cast<uint16_t>((i + 1) * columns + j + 1));
			}
		}
	}



	void GeometryFactory::CreateCylinderVerticesAndIndices(
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

		const float dr = bottom_radius - top_radius;

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
					DX::XMFLOAT3{ },
					DX::XMFLOAT2{ static_cast<float>(j) / slices, 1.f - static_cast<float>(i) / stacks }
				);

				// Cylinder can be parameterized as follows, where we introduce v
				// parameter that goes in the same direction as the v tex-coord
				// so that the bitangent goes in the same direction as the v tex-coord.
				//   Let r0 be the bottom radius and let r1 be the top radius.
				//   y(v) = h - hv for v in [0,1].
				//   r(v) = r1 + (r0-r1)v
				//
				//   x(t, v) = r(v)*cos(t)
				//   y(t, v) = h - hv
				//   z(t, v) = r(v)*sin(t)
				// 
				//  dx/dt = -r(v)*sin(t)
				//  dy/dt = 0
				//  dz/dt = +r(v)*cos(t)
				//
				//  dx/dv = (r0-r1)*cos(t)
				//  dy/dv = -h
				//  dz/dv = (r0-r1)*sin(t)

				DX::XMVECTOR tangentu = DX::XMVectorSet(-s, 0.f, c, 0.f);
				DX::XMVECTOR tangentv = DX::XMVectorSet(dr * c, -height, dr * s, 0.f);

				DX::XMStoreFloat3(
					&mesh.vertices.back().normal,
					DX::XMVector3Normalize(DX::XMVector3Cross(tangentu, tangentv))
				);
			}
		}

		CreateCylinderIndices(
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
			DX::XMFLOAT3{ 0.f, 1.f, 0.f },
			DX::XMFLOAT2{ .5f, .5f }
		);

		for (uint32_t i = 0; i <= slices; i++)
		{
			float x, z;
			DX::XMScalarSinCos(&z, &x, i * theta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			mesh.vertices.emplace_back(
				DX::XMFLOAT3{ top_radius * x, y, top_radius * z },
				DX::XMFLOAT3{ 0.f, 1.f, 0.f },
				DX::XMFLOAT2{ u, v }
			);
		}

		for (uint16_t i = 0; i < slices; i++)
		{
			mesh.indices.push_back(static_cast<uint16_t>(base_i - 1));
			mesh.indices.push_back(static_cast<uint16_t>(base_i + i + 1));
			mesh.indices.push_back(static_cast<uint16_t>(base_i + i));
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
			DX::XMFLOAT3{ 0.f, -1.f, 0.f },
			DX::XMFLOAT2{ .5f, .5f }
		);

		for (uint32_t i = 0; i <= slices; i++)
		{
			float x, z;
			DX::XMScalarSinCos(&z, &x, i * theta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			mesh.vertices.emplace_back(
				DX::XMFLOAT3{ bottom_radius * x, y, bottom_radius * z },
				DX::XMFLOAT3{ 0.f, -1.f, 0.f },
				DX::XMFLOAT2{ u, v }
			);
		}

		for (uint16_t i = 0; i < slices; i++)
		{
			mesh.indices.push_back(static_cast<uint16_t>(base_i - 1));
			mesh.indices.push_back(static_cast<uint16_t>(base_i + i));
			mesh.indices.push_back(static_cast<uint16_t>(base_i + i + 1));
		}
	}

	void GeometryFactory::CreateCylinderIndices(
		MeshData_t& mesh,
		uint32_t slices,
		uint32_t stacks
	)
	{
		mesh.indices.clear();
		mesh.indices.reserve(static_cast<size_t>(stacks) * slices + mesh.indices.size());

		const uint32_t ring_count = slices + 1;

		for (uint16_t i = 0; i < stacks; i++)
		{
			for (uint16_t j = 0; j < slices; j++)
			{
				mesh.indices.push_back(static_cast<uint16_t>(i * ring_count + j));
				mesh.indices.push_back(static_cast<uint16_t>((i + 1) * ring_count + j));
				mesh.indices.push_back(static_cast<uint16_t>((i + 1) * ring_count + j + 1));

				mesh.indices.push_back(static_cast<uint16_t>(i * ring_count + j));
				mesh.indices.push_back(static_cast<uint16_t>((i + 1) * ring_count + j + 1));
				mesh.indices.push_back(static_cast<uint16_t>(i * ring_count + j + 1));
			}
		}
	}


	void GeometryFactory::CreateSphereVertices(
		MeshData_t& mesh,
		uint32_t slices,
		uint32_t stacks,
		float radius
	)
	{
		mesh.vertices.clear();
		mesh.vertices.reserve(static_cast<size_t>(slices) * stacks);

		float phi_step = DX::XM_PI / stacks;
		float theta_step = DX::XM_2PI / slices;

		mesh.vertices.emplace_back(
			DX::XMFLOAT3{ 0.f, radius, 0.f },
			DX::XMFLOAT3{ 0.f, 1.f, 0.f },
			DX::XMFLOAT2{ 0.f, 0.f }
		);

		for (uint32_t i = 0; i < stacks; i++)
		{
			float phi = i * phi_step;
			float sin_phi, cos_phi;
			DX::XMScalarSinCos(&sin_phi, &cos_phi, phi);

			for (uint32_t j = 0; j <= slices; j++)
			{
				float theta = j * theta_step;
				float sin_theta, cos_theta;
				DX::XMScalarSinCos(&sin_theta, &cos_theta, theta);

				mesh.vertices.emplace_back(
					DX::XMFLOAT3(radius * cos_phi * sin_theta, radius * cos_theta, radius * sin_phi * sin_theta),
					DX::XMFLOAT3{},
					DX::XMFLOAT2{ theta / DX::XM_2PI, phi / DX::XM_PI }
				);

				DX::XMStoreFloat3(
					&mesh.vertices.back().normal,
					DX::XMVector3Normalize(DX::XMLoadFloat3(&mesh.vertices.back().position))
				);
			}
		}

		mesh.vertices.emplace_back(
			DX::XMFLOAT3{ 0.f, -radius, 0.f },
			DX::XMFLOAT3{ 0.f, -1.f, 0.f },
			DX::XMFLOAT2{ 0.f, 1.f }
		);
	}


	void GeometryFactory::CreateSphereIndices(
		MeshData_t& mesh,
		uint32_t slices,
		uint32_t stacks
	)
	{
		mesh.indices.clear();
		mesh.indices.reserve((static_cast<size_t>(slices) * stacks + slices) * 6);

		for (uint16_t i = 0; i < slices; i++)
		{
			mesh.indices.push_back(static_cast<uint16_t>(0));
			mesh.indices.push_back(static_cast<uint16_t>(i + 1));
			mesh.indices.push_back(static_cast<uint16_t>(i));
		}

		size_t base_i = 1, ring_count = slices + 1;
		for (uint16_t i = 0; i < stacks; i++)
		{
			for (uint16_t j = 0; j < slices; j++)
			{
				mesh.indices.push_back(static_cast<uint16_t>(base_i + i * ring_count + j));
				mesh.indices.push_back(static_cast<uint16_t>(base_i + i * ring_count + j + 1));
				mesh.indices.push_back(static_cast<uint16_t>(base_i + (i + 1) * ring_count + j));

				mesh.indices.push_back(static_cast<uint16_t>(base_i + (i + 1) * ring_count + j));
				mesh.indices.push_back(static_cast<uint16_t>(base_i + i * ring_count + j + 1));
				mesh.indices.push_back(static_cast<uint16_t>(base_i + (i + 1) * ring_count + j + 1));
			}
		}

		base_i = mesh.vertices.size() - 1 - ring_count;

		for (uint16_t i = 0; i < slices; i++)
		{
			mesh.indices.push_back(static_cast<uint16_t>(mesh.vertices.size() - 1));
			mesh.indices.push_back(static_cast<uint16_t>(base_i + i));
			mesh.indices.push_back(static_cast<uint16_t>(base_i + i + 1));
		}
	}


	void GeometryFactory::CreateSphereVerticesAndIndices(
		MeshData_t& mesh,
		uint32_t num_divisions,
		float radius
	)
	{
		using DX::XMFLOAT3;

		constexpr float X = .525731f;
		constexpr float Z = .850651f;

		constexpr XMFLOAT3 poses[12] =
		{
			XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
			XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
			XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
			XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
			XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
			XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
		};

		constexpr uint16_t indices[60] =
		{
			1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
			1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
			3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
			10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
		};

		mesh.vertices.clear();
		mesh.indices.clear();

		mesh.vertices.reserve(12);
		mesh.indices.reserve(12);

		for (const auto& pos : poses)
			mesh.vertices.emplace_back(pos, DX::XMFLOAT3{}, DX::XMFLOAT2{});
		for (auto idx : indices)
			mesh.indices.emplace_back(idx);

		while (num_divisions--)
			SubdivideMesh(mesh);

		for (auto& vertex : mesh.vertices)
		{
			DX::XMVECTOR normal = DX::XMVector3Normalize(DX::XMLoadFloat3(&vertex.position));

			DX::XMStoreFloat3(
				&vertex.position,
				DX::XMVectorScale(
					normal,
					radius
				)
			);

			DX::XMStoreFloat3(
				&vertex.normal,
				normal
			);

			// Derive texture coordinates from spherical coordinates.
			float theta = atanf(vertex.position.z / vertex.position.x); // in [-pi/2, +pi/2]

			// Quadrant I or IV
			if (vertex.position.x >= 0.0f)
			{
				// If x = 0, then atanf(y/x) = +pi/2 if y > 0
				//                atanf(y/x) = -pi/2 if y < 0

				if (theta < 0.0f)
					theta += DX::XM_2PI; // in [0, 2*pi).
			}

			float phi = acosf(vertex.position.y / radius);

			vertex.textureCoordinate.x = theta / DX::XM_2PI;
			vertex.textureCoordinate.x = phi / DX::XM_PI;
		}
	}

	void GeometryFactory::SubdivideMesh(MeshData_t& mesh)
	{

		/*
			  v1
			  *
			 / \
			/   \
		 m0*-----*m1
		  / \   / \
		 /   \ /   \
		*-----*-----*
		v0    m2     v2
		*/

		MeshData_t new_mesh;

		new_mesh.vertices.resize(mesh.indices.size() * 2);
		new_mesh.indices.reserve(mesh.indices.size() * 4);

		for (size_t i = 0; i < mesh.indices.size() / 3; i++)
		{
			auto v0 = DX::XMLoadFloat3(&mesh.vertices[mesh.indices[i * 3 + 0]].position);
			auto v1 = DX::XMLoadFloat3(&mesh.vertices[mesh.indices[i * 3 + 1]].position);
			auto v2 = DX::XMLoadFloat3(&mesh.vertices[mesh.indices[i * 3 + 2]].position);

			auto m0 = DX::XMVectorScale(DX::XMVectorAdd(v0, v1), 0.5f);
			auto m1 = DX::XMVectorScale(DX::XMVectorAdd(v1, v2), 0.5f);
			auto m2 = DX::XMVectorScale(DX::XMVectorAdd(v2, v0), 0.5f);

			DX::XMStoreFloat3(&new_mesh.vertices[i * 6 + 0].position, v0); // 0
			DX::XMStoreFloat3(&new_mesh.vertices[i * 6 + 1].position, v1); // 1
			DX::XMStoreFloat3(&new_mesh.vertices[i * 6 + 2].position, v2); // 2
			DX::XMStoreFloat3(&new_mesh.vertices[i * 6 + 3].position, m0); // 3
			DX::XMStoreFloat3(&new_mesh.vertices[i * 6 + 4].position, m1); // 4
			DX::XMStoreFloat3(&new_mesh.vertices[i * 6 + 5].position, m2); // 5

			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 0));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 3));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 5));

			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 5));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 4));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 2));

			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 3));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 1));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 4));

			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 3));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 4));
			new_mesh.indices.push_back(static_cast<uint16_t>(i * 6 + 5));
		}

		mesh = std::move(new_mesh);
	}


	GeometryInstance::GeometryInstance(GeometryInstance* geometry)
	{
		if (geometry)
		{
			this->d3dVtxShader = std::move(geometry->d3dVtxShader);
			this->d3dPxlShader = std::move(geometry->d3dPxlShader);
			this->d3dInputLayout = std::move(geometry->d3dInputLayout);
		}
	}

	void GeometryInstance::CreateBuffers(ID3D11Device* d3ddevice, bool custom_constants)
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
				Mesh.indices,
				D3D11_BIND_INDEX_BUFFER,
				d3dIndices.ReleaseAndGetAddressOf()
			)
		);

		// Create constant buffer for position
		if (!custom_constants)
			d3dConstants_WRP.Create(d3ddevice);

		// Dispose of old buffer
		Mesh = {};
	}


	void GeometryInstance::CreateShaders(ID3D11Device* d3ddevice, const wchar_t* vs_shader, const wchar_t* ps_shader)
	{
		DX::ComPtr<ID3DBlob> shader_blob;
		// Create pixel shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(ps_shader, shader_blob.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreatePixelShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					d3dPxlShader.ReleaseAndGetAddressOf()
				)
			);
		}

		// Create vertex shader
		{
			DX::ThrowIfFailed(
				D3DReadFileToBlob(vs_shader, shader_blob.ReleaseAndGetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateVertexShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					d3dVtxShader.ReleaseAndGetAddressOf()
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
				d3dInputLayout.ReleaseAndGetAddressOf()
			)
		);
	}


	void GeometryInstance::Bind(ID3D11DeviceContext* d3dcontext)
	{
		d3dcontext->IASetIndexBuffer(d3dIndices.Get(), DXGI_FORMAT_R16_UINT, 0);
		d3dcontext->IASetInputLayout(d3dInputLayout.Get());
		d3dcontext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		uint32_t strides[]{ sizeof(MeshData_t::verticies_type) };
		uint32_t offsets[]{ 0 };

		d3dcontext->IASetVertexBuffers(0, 1, d3dVerticies.GetAddressOf(), strides, offsets);

		d3dcontext->VSSetShader(d3dVtxShader.Get(), nullptr, 0);

		if (d3dConstants_WRP.GetBuffer())
		{
			ID3D11Buffer* buffer[] = { d3dConstants_WRP.GetBuffer() };
			d3dcontext->VSSetConstantBuffers(0, 1, buffer);
		}

		d3dcontext->PSSetShader(d3dPxlShader.Get(), nullptr, 0);
	}
}