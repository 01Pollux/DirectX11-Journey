
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
		//size_t vertex_count = (columns - 1) * (rows - 1);
		//size_t triangle_count = vertex_count * 2;

		float x_step = 1.f / m_RowCols[1];
		float z_step = 1.f / m_RowCols[0];

		float dx = m_PlaneSize[0] * x_step;
		float dz = m_PlaneSize[1] * z_step;

		float neg_half_width = m_PlaneSize[0] * -0.5f;
		float half_height = m_PlaneSize[1] * 0.5f;

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
		m_PlaneMesh.vertices.reserve(m_RowCols[0] * m_RowCols[1]);

		for (size_t z = 0; z < m_RowCols[0]; z++)
		{
			float cur_z = half_height - z * dz;
			for (size_t x = 0; x < m_RowCols[1]; x++)
			{
				float cur_x = neg_half_width + x * dx;
				m_PlaneMesh.vertices.emplace_back(DX::XMFLOAT3{ cur_x, 0.f, cur_z }, DX::XMFLOAT4{});
			}
		}
	}

	void SimplePlane::BuildPlaneIndicies()
	{
		m_PlaneMesh.indicies.clear();

		uint32_t cols = m_RowCols[1], rows = m_RowCols[0];
		m_PlaneMesh.indicies.reserve(6 * (cols - 1) * (rows - 1));

		for (uint16_t y = 0; y < rows - 1; y++)
		{
			for (uint16_t x = 0; x < cols - 1; x++)
			{
				m_PlaneMesh.indicies.push_back(static_cast<uint16_t>(y * cols			+ x));
				m_PlaneMesh.indicies.push_back(static_cast<uint16_t>(y * cols			+ x + 1));
				m_PlaneMesh.indicies.push_back(static_cast<uint16_t>((y + 1) * cols		+ x));

				m_PlaneMesh.indicies.push_back(static_cast<uint16_t>((y + 1) * cols		+ x));
				m_PlaneMesh.indicies.push_back(static_cast<uint16_t>(y * cols			+ x + 1));
				m_PlaneMesh.indicies.push_back(static_cast<uint16_t>((y + 1) * cols		+ x + 1));
			}
		}
	}
}