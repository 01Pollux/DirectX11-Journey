//***************************************************************************************
// Waves.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs the calculations for the wave simulation.  After the simulation has been
// updated, the client must copy the current solution into vertex buffers for rendering.
// This class only does the calculations, it does not do any drawing.
//***************************************************************************************

#pragma once

#include "utils/pch.hpp"

namespace Pleiades
{
	class Waves
	{
	public:
		uint32_t RowCount()const;
		uint32_t ColumnCount()const;
		uint32_t VertexCount()const;
		uint32_t TriangleCount()const;
		float Width()const;
		float Depth()const;

		// Returns the solution at the ith grid point.
		const DX::XMFLOAT3& operator[](int i)const { return m_CurrSolution[i]; }

		// Returns the solution normal at the ith grid point.
		const DX::XMFLOAT3& Normal(int i)const { return m_Normals[i]; }

		void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
		void Update(float dt);
		void Disturb(UINT i, UINT j, float magnitude);

	private:
		uint32_t m_NumRows{};
		uint32_t m_NumCols{};

		uint32_t m_VertexCount{};
		uint32_t m_TriangleCount{};

		// Simulation constants we can precompute.
		float m_K1{};
		float m_K2{};
		float m_K3{};

		float m_TimeStep{};
		float m_SpatialStep{};

		std::unique_ptr<DX::XMFLOAT3[]> m_PrevSolution;
		std::unique_ptr<DX::XMFLOAT3[]> m_CurrSolution;
		std::unique_ptr<DX::XMFLOAT3[]> m_Normals;
	};

}
