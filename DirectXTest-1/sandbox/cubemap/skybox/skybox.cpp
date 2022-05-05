
#include "utils/pch.hpp"
#include "skybox.hpp"

#include "sky.hpp"

namespace Pleiades::Sandbox
{
	SkyboxCubeMap::SkyboxCubeMap(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),

		m_Skybox(std::make_unique<SkyboxCubeMapDemo::Skybox>(&m_Camera, d3dres, L"resources/skybox/grasscube1024.dds", 5'000.f)),

		m_Camera(d3dres),
		m_CBuffers(d3dres)
	{
	}


	void SkyboxCubeMap::OnFrame(uint64_t ticks)
	{
		m_Camera.update(ticks);

		m_CBuffers.Buffers.SetShaders<ConsantBufferShader::VS>(0, 0, 1);
		m_CBuffers.Buffers.SetShaders<ConsantBufferShader::PS>(0, 0, 1);
		
		m_Skybox->Draw(m_CBuffers.Buffers);
	}
}