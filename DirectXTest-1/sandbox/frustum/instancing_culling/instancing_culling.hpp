
#include "utils/sandbox.hpp"
#include "directx/Camera.hpp"
#include "effect.hpp"

namespace Pleiades::Sandbox
{
	class InstancedFrustum : public ISandbox
	{
	public:
		struct InstancedData
		{
			DX::XMMATRIX World;
			DX::XMFLOAT4 Color;
		};

		using EffectManager_t = InstFrustumDemo::EffectManager;
		InstancedFrustum(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Instanced frustum";
		}

	private:
		void InitializeInstancedGround();
		void InitializeBuffers();
		void InitializeShaders();

		void Bind();
		void WriteInstances();
		void Draw();

	private:
		DX::Camera m_Camera;
		EffectManager_t m_EffectManager;

		Material m_SkullMat;

		uint32_t m_IndexCount{}, m_InstanceCount{};
		std::vector<InstancedData> m_InstancedWorld;

		DX::ComPtr<ID3D11Buffer> m_d3dSkullVB, m_d3dInstWorldVB, m_IndexBuffer;
		DX::ComPtr<ID3D11VertexShader> m_d3dVertexShader;
		DX::ComPtr<ID3D11InputLayout> m_d3dInputLayout;
		DX::ComPtr<ID3D11PixelShader> m_d3dPixelShader;
	};
}