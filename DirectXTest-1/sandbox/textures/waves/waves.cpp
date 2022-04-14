//***************************************************************************************
// Waves.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "utils/pch.hpp"
#include "waves.hpp"

using namespace DX;

namespace Pleiades
{
	uint32_t Waves::RowCount()const
	{
		return m_NumRows;
	}

	uint32_t Waves::ColumnCount()const
	{
		return m_NumCols;
	}

	uint32_t Waves::VertexCount()const
	{
		return m_VertexCount;
	}

	uint32_t Waves::TriangleCount()const
	{
		return m_TriangleCount;
	}

	float Waves::Width()const
	{
		return m_NumCols * m_SpatialStep;
	}

	float Waves::Depth()const
	{
		return m_NumRows * m_SpatialStep;
	}

	void Waves::Init(uint32_t m, uint32_t n, float dx, float dt, float speed, float damping)
	{
		m_NumRows = m;
		m_NumCols = n;

		m_VertexCount = m * n;
		m_TriangleCount = (m - 1) * (n - 1) * 2;

		m_TimeStep = dt;
		m_SpatialStep = dx;

		float d = damping * dt + 2.0f;
		float e = (speed * speed) * (dt * dt) / (dx * dx);
		m_K1 = (damping * dt - 2.0f) / d;
		m_K2 = (4.0f - 8.0f * e) / d;
		m_K3 = (2.0f * e) / d;

		m_PrevSolution = std::make_unique<XMFLOAT3[]>(m * n);
		m_CurrSolution = std::make_unique<XMFLOAT3[]>(m * n);
		m_Normals = std::make_unique<XMFLOAT3[]>(m * n);

		// Generate grid vertices in system memory.

		float halfWidth = (n - 1) * dx * 0.5f;
		float halfDepth = (m - 1) * dx * 0.5f;
		for (uint32_t i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dx;
			for (uint32_t j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dx;

				m_PrevSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
				m_CurrSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
				m_Normals[i * n + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}
		}
	}

	void Waves::Update(float dt)
	{
		static float t = 0;

		// Accumulate time.
		t += dt;

		// Only update the simulation at the specified time step.
		if (t >= m_TimeStep)
		{
			// Only update interior points; we use zero boundary conditions.
			for (uint32_t i = 1; i < m_NumRows - 1; ++i)
			{
				for (uint32_t j = 1; j < m_NumCols - 1; ++j)
				{
					// After this update we will be discarding the old previous
					// buffer, so overwrite that buffer with the new update.
					// Note how we can do this inplace (read/write to same element) 
					// because we won't need prev_ij again and the assignment happens last.

					// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
					// Moreover, our +z axis goes "down"; this is just to 
					// keep consistent with our row indices going down.

					m_PrevSolution[i * m_NumCols + j].y =
						m_K1 * m_PrevSolution[i * m_NumCols + j].y +
						m_K2 * m_CurrSolution[i * m_NumCols + j].y +
						m_K3 * (m_CurrSolution[(i + 1) * m_NumCols + j].y +
							m_CurrSolution[(i - 1) * m_NumCols + j].y +
							m_CurrSolution[i * m_NumCols + j + 1].y +
							m_CurrSolution[i * m_NumCols + j - 1].y);
				}
			}

			// We just overwrote the previous buffer with the new data, so
			// this data needs to become the current solution and the old
			// current solution becomes the new previous solution.
			std::swap(m_PrevSolution, m_CurrSolution);

			t = 0.0f; // reset time

			//
			// Compute normals using finite difference scheme.
			//
			for (uint32_t i = 1; i < m_NumRows - 1; ++i)
			{
				for (uint32_t j = 1; j < m_NumCols - 1; ++j)
				{
					float l = m_CurrSolution[i * m_NumCols + j - 1].y;
					float r = m_CurrSolution[i * m_NumCols + j + 1].y;
					float t = m_CurrSolution[(i - 1) * m_NumCols + j].y;
					float b = m_CurrSolution[(i + 1) * m_NumCols + j].y;
					m_Normals[i * m_NumCols + j].x = -r + l;
					m_Normals[i * m_NumCols + j].y = 2.0f * m_SpatialStep;
					m_Normals[i * m_NumCols + j].z = b - t;

					XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&m_Normals[i * m_NumCols + j]));
					XMStoreFloat3(&m_Normals[i * m_NumCols + j], n);
				}
			}
		}
	}

	void Waves::Disturb(uint32_t i, uint32_t j, float magnitude)
	{
		// Don't disturb boundaries.
		assert(i > 1 && i < m_NumRows - 2);
		assert(j > 1 && j < m_NumCols - 2);

		float halfMag = 0.5f * magnitude;

		// Disturb the ijth vertex height and its neighbors.
		m_CurrSolution[i * m_NumCols + j].y += magnitude;
		m_CurrSolution[i * m_NumCols + j + 1].y += halfMag;
		m_CurrSolution[i * m_NumCols + j - 1].y += halfMag;
		m_CurrSolution[(i + 1) * m_NumCols + j].y += halfMag;
		m_CurrSolution[(i - 1) * m_NumCols + j].y += halfMag;
	}
}
