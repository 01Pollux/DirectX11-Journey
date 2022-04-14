#pragma once

#include "effect.hpp"
#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	class MultipleTexturedBox : public ISandbox
	{
	public:
		using EffectManager = TextureBoxMutliple::EffectManager;

		MultipleTexturedBox(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Box multi-textured";
		}

	private:
		void BuildBoxMesh();

		static EffectManager::WorldConstantBuffer GetDefaultWolrdConstants();
		static EffectManager::NonNumericConstants GetDefaultTexture(DX::DeviceResources* d3dres);

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
		float m_BoxSize[3]{ 2.f, 2.f, 2.f };
		float m_CamPosition[3]{ .26f, .18f, 11.24f };
		float m_CamRotation[3]{ -.33f, -.49f, .2f };

		DX::XMMATRIX m_ViewProjection;

		EffectManager m_Effects;
		std::unique_ptr<GeometryInstance> m_BoxGeometry;
		GeoInfo_t m_Box;
	};
}