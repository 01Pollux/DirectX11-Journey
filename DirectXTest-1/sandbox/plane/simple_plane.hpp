#pragma once

#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class SimplePlane : public ISandbox
	{
	public:
		struct MeshData_t
		{
			std::vector<DX::VertexPositionColor> vertices;
			std::vector<unsigned short> indicies;
		};

		SimplePlane(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;

		static const char* GetName() noexcept
		{
			return "Simple plane";
		}

	private:
		void BuildPlaneMesh();
		void BuildPlaneVertices();
		void BuildPlaneIndicies();

		static constexpr float GetHeight(float x, float z) noexcept
		{
			return .3f * (z * std::sinf(x * .1f) + x * std::cosf(z * .1f));
		}

		MeshData_t m_PlaneMesh;
	};
}