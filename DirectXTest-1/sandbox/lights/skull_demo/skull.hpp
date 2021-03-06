#pragma once

#include "effect.hpp"
#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	class LitSkullDemo : public ISandbox
	{
	public:
		using EffectManager = LitDemo::EffectManager;

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
		int m_LightCount = 1;
		float m_CamPosition[3]{ -1.5f, -2.55f, 30.39f };
		float m_CamRotation[3]{ -.33f, -.49f, 0.01f };
		DX::XMMATRIX m_ViewProjection;

		GeoInfo_t m_Plane;
		GeoInfo_t m_Skull;
		GeoInfo_t m_Cylinder[4];
	};
}