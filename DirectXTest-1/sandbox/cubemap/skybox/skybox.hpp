#pragma once

#include "utils/sandbox.hpp"
#include "directx/Camera.hpp"

#include "cbuffers.hpp"

namespace Pleiades::Sandbox
{
	class IRenderable;

	class SkyboxCubeMap : public ISandbox
	{
	public:
		SkyboxCubeMap(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;

		static const char* GetName() noexcept
		{
			return "Skybox cubemap";
		}

	private:
		DX::Camera m_Camera;

		SkyboxCubeMapDemo::CBuffers_t m_CBuffers;

		std::unique_ptr<IRenderable> m_Skybox, m_Skull;
	};
}