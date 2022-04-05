#pragma once

#include "utils/sandbox.hpp"
#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	class PlaneAndSphere : public ISandbox
	{
	public:
		PlaneAndSphere(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Plane + sphere";
		}

	private:
		void BuildManyMeshes();

		void UpdateScene();

	private:
		// Plane
		float m_RotationOffset[3]{ -0.41f, 0.06f, 0.f, };
		float m_DrawOffset[3]{ 0.f, -32.f, 367.f };
		float m_PlaneSize[2]{ 160.f, 160.f };
		int32_t m_RowCols[2]{ 50, 50 };

		// Sphere
		float m_SphereRadius = 10.f;
		int32_t m_SlicesStacks[2]{ 20, 20 };

		std::unique_ptr<GeometryInstance> m_ManyShapes;
	};
}