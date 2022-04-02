
#include "utils/pch.hpp"
#include "simple_plane.hpp"

namespace Pleiades::Sandbox
{
	void SimplePlane::BuildPlaneMesh()
	{
		// build verticies
		BuildPlaneVertices();

		// build colors
		for (auto& mesh : m_PlaneMesh.vertices)
		{
			float y = mesh.position.y = GetHeight(mesh.position.x, mesh.position.z);

			if (y < -10.0f)
			{
				// Sandy beach color.
				mesh.color = DX::XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
			}
			else if (y < 5.0f)
			{
				// Light yellow-green.
				mesh.color = DX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
			}
			else if (y < 12.0f)
			{
				// Dark yellow-green.
				mesh.color = DX::XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
			}
			else if (y < 20.0f)
			{
				// Dark brown.
				mesh.color = DX::XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
			}
			else
			{
				// White snow.
				mesh.color = DX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}

		// build indicies
		BuildPlaneIndicies();
	}

	void SimplePlane::BuildPlaneVertices()
	{
		constexpr float plane_width = 160.f;
		constexpr float plane_height = 160.f;

		constexpr size_t columns = 32u;
		constexpr size_t rows = 32u;

		//size_t vertex_count = (columns - 1) * (rows - 1);
		//size_t triangle_count = vertex_count * 2;

		float x_step = 1.f / columns;
		float z_step = 1.f / rows;

		float dx = plane_width * x_step;
		float dz = plane_height * z_step;

		float neg_half_width = plane_width * -0.5f;
		float half_height = plane_width * 0.5f;

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

		m_PlaneMesh.vertices.clear();
		m_PlaneMesh.vertices.reserve(rows * columns);

		for (size_t x = 0; x < rows; x++)
		{
			float cur_x = x * dx;
			for (size_t z = 0; z < columns; z++)
			{
				float cur_z = z * dz;
				m_PlaneMesh.vertices.emplace_back(DX::XMFLOAT3(neg_half_width + cur_x, 0.f, half_height - cur_z));
			}
		}
	}

	void SimplePlane::BuildPlaneIndicies()
	{
		constexpr size_t columns = 32u;
		constexpr size_t rows = 32u;

		m_PlaneMesh.indicies.clear();
		m_PlaneMesh.indicies.reserve((rows - 1) * (columns - 1));

		for (uint16_t x = 0; x < rows - 1; x++)
		{
			for (uint16_t y = 0; y < columns - 1; y++)
			{
				m_PlaneMesh.indicies.push_back(x * rows + columns);
				m_PlaneMesh.indicies.push_back(x * rows + columns + 1);
				m_PlaneMesh.indicies.push_back((x + 1) * rows + columns);

				m_PlaneMesh.indicies.push_back((x + 1) * rows + columns);
				m_PlaneMesh.indicies.push_back(x * rows + columns + 1);
				m_PlaneMesh.indicies.push_back((x + 1) * rows + columns + 1);
			}
		}
	}
}