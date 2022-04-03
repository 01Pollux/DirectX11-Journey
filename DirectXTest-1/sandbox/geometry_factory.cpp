
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

		mesh.indicies.reserve(6u * (columns - 1) * (rows - 1));

		for (uint16_t y = 0; y < rows - 1; y++)
		{
			for (uint16_t x = 0; x < columns - 1; x++)
			{
				mesh.indicies.push_back(static_cast<uint16_t>(y * columns + x));
				mesh.indicies.push_back(static_cast<uint16_t>(y * columns + x + 1));
				mesh.indicies.push_back(static_cast<uint16_t>((y + 1) * columns + x));

				mesh.indicies.push_back(static_cast<uint16_t>((y + 1) * columns + x));
				mesh.indicies.push_back(static_cast<uint16_t>(y * columns + x + 1));
				mesh.indicies.push_back(static_cast<uint16_t>((y + 1) * columns + x + 1));
			}
		}
	}



	void GeometryFactory::CreateCylinderVertices(
		MeshData_t& mesh, 
		uint32_t slices, 
		uint32_t stacks, 
		float bottomn_radius, 
		float top_radius, 
		float height
	)
	{
		const float stack_height = height / stacks;
		const float radius_step = (top_radius - bottomn_radius) / stacks;

		const float theta = DX::XM_2PI / slices;

		mesh.vertices.clear();
		mesh.vertices.reserve(static_cast<size_t>(stacks - 1) * slices);

		for (uint32_t i = 0; i < stacks; i++)
		{
			float cur_y = -height / 2 + i * stack_height;
			float cur_radius = bottomn_radius + i * radius_step;

			for (uint32_t j = 0; j <= slices; j++)
			{
				const float c = std::cosf(j * theta);
				const float s = std::sinf(j * theta);

				mesh.vertices.emplace_back(
					DX::XMFLOAT3{ c * cur_radius, cur_y, s * cur_radius },
					DX::XMFLOAT4(DX::Colors::Magenta)
				);
			}
		}
	}

	void GeometryFactory::CreateCylinderIndicies(
		MeshData_t& mesh, 
		uint32_t slices, 
		uint32_t stacks
	)
	{
		mesh.indicies.clear();
		mesh.indicies.reserve(static_cast<size_t>(stacks - 1) * slices);

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