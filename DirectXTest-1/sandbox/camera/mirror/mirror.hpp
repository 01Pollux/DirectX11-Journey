#pragma once

#include "effect.hpp"
#include "render_states.hpp"
#include "sandbox/geometry_factory.hpp"
#include "sandbox/waves.hpp"
#include "directx/Camera.hpp"


namespace Pleiades::Sandbox
{
	class CamMirrorSkullWorld : public ISandbox
	{
	public:
		using EffectManager = CamStencilMirrorDemo::EffectManager;
		using BlendRenderState = CamStencilMirrorDemo::BlendRenderState;

		CamMirrorSkullWorld(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t frame_time) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Camera (Mirror + skull + world)";
		}

	private:
		void InitializeWorldInfo();
		void InitializeForD3D();

		static GeometryInstance::MeshData_t CreateWalls();
		static GeometryInstance::MeshData_t CreateMirror();

		static EffectManager::WorldConstantBuffer GetDefaultWolrdConstants();
		static EffectManager::NonNumericConstants GetDefaultTexture(DX::DeviceResources* d3dres);

		void DrawWorld();
		void DrawSkull();
		void DrawMirror_Stencil();
		void DrawSkull_Reflection();
		void DrawMirror(bool stencil);
		void DrawSkull_Shadow();

	private:
		DX::Camera m_Camera;

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