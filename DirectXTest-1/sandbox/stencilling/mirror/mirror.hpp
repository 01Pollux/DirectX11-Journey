#pragma once

#include "effect.hpp"
#include "render_states.hpp"
#include "sandbox/geometry_factory.hpp"
#include "sandbox/waves.hpp"

namespace Pleiades::Sandbox
{
	class MirrorSkullWorld : public ISandbox
	{
	public:
		using EffectManager = StencilMirrorDemo::EffectManager;
		using BlendRenderState = StencilMirrorDemo::BlendRenderState;

		MirrorSkullWorld(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t frame_time) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Mirror + skull + world";
		}

	private:
		void InitializeWorldInfo();
		void InitializeForD3D();

		static GeometryInstance::MeshData_t CreateWalls();
		static GeometryInstance::MeshData_t CreateMirror();

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

		void DrawWorld();
		void DrawSkull();
		void DrawMirror_Stencil();
		void DrawSkull_Reflection();
		void DrawMirror(bool stencil);

	private:
		float m_CamPosition[3]{ 2.26f, -1.4f, 10.3f };
		float m_CamRotation[3]{ -.29f, .17f, 0.f };

		DX::XMMATRIX m_ViewProjection;

		EffectManager m_Effects;
		BlendRenderState m_BlendRenderState;

		GeometryInstance m_WallGeometry;
		GeometryInstance m_SkullGeometry;
		GeometryInstance m_MirrorGeometry;

		GeoInfo_t m_Wall;
		GeoInfo_t m_Skull;
		GeoInfo_t m_Mirror;

		DX::ComPtr<ID3D11ShaderResourceView> m_WallTexture;
		DX::ComPtr<ID3D11ShaderResourceView> m_SkullTexture;
		DX::ComPtr<ID3D11ShaderResourceView> m_MirrorTexture;
	};
}