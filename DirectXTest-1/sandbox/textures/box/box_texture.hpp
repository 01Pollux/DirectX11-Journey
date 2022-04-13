#pragma once

#include "effect.hpp"
#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	class TexturedBox : public ISandbox
	{
	public:
		using EffectManager = TextureBox::EffectManager;

		TexturedBox(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Box textured";
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
		float m_CamPosition[3]{ -1.5f, -2.55f, 30.39f };
		float m_CamRotation[3]{ -.33f, -.49f, 0.01f };

		DX::XMMATRIX m_ViewProjection;

		EffectManager m_Effects;
		std::unique_ptr<GeometryInstance> m_BoxGeometry;
		GeoInfo_t m_Box;
	};
}