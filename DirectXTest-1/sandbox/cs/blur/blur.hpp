#pragma once

#include "effect.hpp"
#include "render_states.hpp"
#include "sandbox/geometry_factory.hpp"
#include "sandbox/waves.hpp"

namespace Pleiades::Sandbox
{
	class BlurredTextureDemo : public ISandbox
	{
	public:
		using EffectManager = BlurredWavesBlendingEffect::EffectManager;
		using BlendRenderState = BlurredWavesBlendingEffect::BlendRenderState;

		BlurredTextureDemo(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t frame_time) override;

		static const char* GetName() noexcept
		{
			return "Blurred demo textured";
		}

	private:
		EffectManager::NonNumericConstants GetDefaultTexture(DX::DeviceResources* d3dres);

		void DrawOffscreen();
		void DrawOnScreen();

	private:
		EffectManager m_Effects;

		GeometryInstance m_FullscreenGeometry, m_FullscreenGeometryFinal;

		DX::ComPtr<ID3D11ShaderResourceView>	m_SomeTextureView;

		DX::ComPtr<ID3D11UnorderedAccessView>	m_OffscreenUAVView;
		DX::ComPtr<ID3D11ShaderResourceView>	m_OffscreenTexView;
		DX::ComPtr<ID3D11RenderTargetView>		m_OffscreenRenderView;
	};
}