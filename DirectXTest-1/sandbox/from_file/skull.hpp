#pragma once

#include "utils/sandbox.hpp"
#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	class SkullFromFile : public ISandbox
	{
	public:
		SkullFromFile(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Skull from file";
		}

	private:
		void UpdateScene();

	private:
		float m_RotationOffset[3]{ -0.59f, 0.67f, 0.f };
		float m_DrawOffset[3]{ 0.43f, -1.63f, 19.76f };

		GeometryInstance m_Skull;
	};
}