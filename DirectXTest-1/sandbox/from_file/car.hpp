#pragma once

#include "utils/sandbox.hpp"
#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	class CarFromFile : public ISandbox
	{
	public:
		CarFromFile(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Car from file";
		}

	private:
		void UpdateScene();

	private:
		float m_RotationOffset[3]{ -0.19f, 0.9f, -0.33f };
		float m_DrawOffset[3]{ 0.43f, -1.22f, 28.11f };

		GeometryInstance m_Car;
	};
}