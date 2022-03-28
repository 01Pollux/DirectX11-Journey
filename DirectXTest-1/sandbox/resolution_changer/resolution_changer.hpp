#pragma once

#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class ResolutionCChanger_ImGui : public ISandbox
	{
	public:
		ResolutionCChanger_ImGui(DX::DeviceResources* d3dres);

		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Resolution changer (ImGui)";
		}

	private:
		std::vector<DXGI_MODE_DESC> m_DXGIModes;
	};
}