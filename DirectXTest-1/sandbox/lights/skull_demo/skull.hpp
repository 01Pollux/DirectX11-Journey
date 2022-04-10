#pragma once

#include "effect.hpp"
#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	struct GeoInfo_t
	{
		Material Material;
		DX::XMMATRIX World;

		void Bind(DX::DeviceResources* d3dres, EffectManager& fxmgr, const DX::XMMATRIX& view_proj)
		{
			auto world_transposed = DX::XMMatrixTranspose(World);

			fxmgr.SetWorld(world_transposed);
			fxmgr.SetWorldInvTranspose(DX::XMMatrixInverse(nullptr, world_transposed));
			fxmgr.SetWorldViewProj(DX::XMMatrixTranspose(World * view_proj));
			fxmgr.SetMaterial(Material);
			fxmgr.Update(d3dres->GetD3DDeviceContext());
		}
	};

	class LitSkullDemo : public ISandbox
	{
	public:
		LitSkullDemo(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Lit skull demo";
		}

	private:
		void InitializeShapesMesh();
		void InitializeSkullMesh();

		void InitializeBuffers();
		void InitializeShaders();

		static EffectManager::WorldConstantBuffer GetDefaultWolrdConstants();
		void InitializeShapes();

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
		std::vector<GeometryInstance::callback_type> m_GeometryCallbacks;

		GeometryInstance m_ShapesGeometry;
		GeometryInstance m_SkullGeometry;

		EffectManager m_Effects;
		float m_CamPosition[3]{ 0.f, -2.73f, 34.3f };
		float m_CamRotation[3]{ -.33f, -.3f, 0.f };
		DX::XMMATRIX m_ViewProjection;

		GeoInfo_t m_Plane;
		GeoInfo_t m_Skull;
		GeoInfo_t m_Cylinder[4];
		GeoInfo_t m_Sphere[4];
	};
}