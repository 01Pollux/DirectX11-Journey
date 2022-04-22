#pragma once

#include "effect.hpp"
#include "render_states.hpp"
#include "sandbox/geometry_factory.hpp"
#include "sandbox/waves.hpp"

namespace Pleiades::Sandbox
{
	class GSBillboardsDemo : public ISandbox
	{
	public:
		static constexpr uint32_t NumTextures = 4u;

		using EffectManager = GSBillboardsDemoFx::EffectManager;
		using BlendRenderState = GSBillboardsDemoFx::BlendRenderState;

		GSBillboardsDemo(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t frame_time) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "GS Billboards";
		}

		struct MeshData_t
		{
			DX::XMFLOAT3 Position;
			DX::XMFLOAT2 Size;
		};

	private:
		void InitializeWorldInfo();
		void InitializeForD3D();

		static EffectManager::NonNumericBuffer GetDefaultTextures(DX::DeviceResources* d3dres);
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

		void DrawBillboard();

	private:
		float m_CamPosition[3]{ 2.26f, -1.4f, 16.24f };
		float m_CamRotation[3]{ -.29f, .17f, 0.f };
		bool m_AlphaToCoverage = DX::DeviceResources::c_MSAAOn;

		DX::XMMATRIX m_ViewProjection;

		EffectManager m_Effects;
		BlendRenderState m_BlendRenderState;

		DX::ComPtr<ID3D11GeometryShader> m_d3dPointBillboardGS;
		GeometryInstance m_PointBillboardGeometry;
		GeoInfo_t		m_PointBilloards[NumTextures];
	};
}