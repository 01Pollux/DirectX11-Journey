#pragma once

#include "effect.hpp"
#include "render_states.hpp"
#include "sandbox/geometry_factory.hpp"
#include "sandbox/waves.hpp"

namespace Pleiades::Sandbox
{
	class GSBillboards : public ISandbox
	{
	public:
		using EffectManager = GSSubdivisonDemoFx::EffectManager;
		using BlendRenderState = GSSubdivisonDemoFx::BlendRenderState;

		GSBillboards(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t frame_time) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "GS Billboards";
		}

	private:
		void InitializeWorldInfo();
		void InitializeForD3D();

		static EffectManager::WorldConstantBuffer GetDefaultWolrdConstants();

		void UpdateViewProjection()
		{
			m_ViewProjection =
				DX::XMMatrixRotationRollPitchYaw(m_CamRotation[0], m_CamRotation[1], m_CamRotation[2]) *
				DX::XMMatrixTranslation(m_CamPosition[0], m_CamPosition[1], m_CamPosition[2]) *
				DX::XMMatrixPerspectiveLH(
					DX::XM_PIDIV4, GetDeviceResources()->GetAspectRatio(), 1.f, 1000.f
				);
		}

		void DrawCube();

	private:
		float m_CamPosition[3]{ 2.26f, -1.4f, 16.24f };
		float m_CamRotation[3]{ -.29f, .17f, 0.f };

		DX::XMMATRIX m_ViewProjection;

		EffectManager m_Effects;
		BlendRenderState m_BlendRenderState;

		DX::ComPtr<ID3D11GeometryShader> m_GSTriangle;
		uint32_t m_TriangleSubDivisons{ 1 };

		GeometryInstance m_SphereGeometry;
		GeoInfo_t m_Sphere;
	};
}