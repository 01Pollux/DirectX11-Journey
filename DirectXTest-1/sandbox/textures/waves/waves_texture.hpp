#pragma once

#include "effect.hpp"
#include "sandbox/geometry_factory.hpp"
#include "sandbox/waves.hpp"

namespace Pleiades::Sandbox
{
	class WavesTextured : public ISandbox
	{
	public:
		using EffectManager = WavesTextureEffect::EffectManager;

		WavesTextured(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t frame_time) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Waves textured";
		}

	private:
		void BuildLandAndWavesMesh();
		void UpdateWavesMesh(uint64_t ticks);

		static EffectManager::WorldConstantBuffer GetDefaultWolrdConstants();
		EffectManager::NonNumericConstants GetDefaultTexture(DX::DeviceResources* d3dres);

		void UpdateViewProjection()
		{
			m_ViewProjection =
				DX::XMMatrixRotationRollPitchYaw(m_CamRotation[0], m_CamRotation[1], m_CamRotation[2]) *
				DX::XMMatrixTranslation(m_CamPosition[0], m_CamPosition[1], m_CamPosition[2]) *
				DX::XMMatrixPerspectiveLH(
					DX::XM_PIDIV4, GetDeviceResources()->GetAspectRatio(), 1.f, 1000.f
				);
		}

	private:
		float m_CamPosition[3]{ .26f, .18f, 11.24f };
		float m_CamRotation[3]{ -.33f, -.49f, .2f };

		DX::XMMATRIX m_ViewProjection;

		EffectManager m_Effects;

		GeometryInstance m_LandGeometry;
		GeometryInstance m_WavesGeometry;
		Waves m_WavesMesh;

		GeoInfo_t m_Land;
		GeoInfo_t m_Waves;

		DX::ComPtr<ID3D11ShaderResourceView> m_LandTexture;
		DX::ComPtr<ID3D11ShaderResourceView> m_WavesTexture;
	};
}