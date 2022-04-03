
#include "utils/pch.hpp"
#include "utils/sandbox.hpp"

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
			const float cur_y = -height * 0.5f + i * stack_height;
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
}