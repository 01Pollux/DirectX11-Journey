#pragma once

#include "helper.hpp"
#include "utils/sandbox.hpp"
#include "sandbox/geometry_factory.hpp"


namespace Pleiades::Sandbox
{
	enum class LightType : uint32_t
	{
		Directional,
		PointLight,
		SpotLight
	};

	struct WorldConstantBuffer
	{
		DX::XMMATRIX WorldViewProj;
		DX::XMMATRIX World;
		DX::XMMATRIX WorldInvTranspose;
		Material	 Material;
	};

	struct LightConstantBuffer
	{
		DirectionalLight DirLight;
		PointLight		 PtLight;
		SpotLight		 SpLight;

		DX::XMFLOAT3	 WorldEyePosition;
		LightType		 LightType;
	};

	class LightDemo : public ISandbox
	{
	public:
		LightDemo(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Light demo";
		}

	private:
		void BuildMeshes();
		void InitializeBuffers();

		void UpdateScene(bool is_land);

		void RenderWorldEdit();
		void RenderLightsEdit();

		void SetMaterial(const Material& material);

	private:
		float m_RotationOffset[2][3]{ {-0.34f, 0.f, 0.f}, {-0.34f, 0.f, 0.f} };
		float m_DrawOffset[2][3]{ {0.43f, -1.22f, 300.f}, {0.43f, 31.22f, 300.f} };

		WorldConstantBuffer m_WorldConstantBuffer;
		LightConstantBuffer m_LightConstantBuffer;

		std::unique_ptr<GeometryInstance> m_Land, m_Sphere;
		Material m_LandMat, m_SphereMat;

		DX::ComPtr<ID3D11Buffer> m_d3dWorldConstantBuffer, m_d3dLightConstantBuffer;
	};
}
