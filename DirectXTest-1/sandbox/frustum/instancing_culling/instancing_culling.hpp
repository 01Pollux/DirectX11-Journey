
#include "utils/sandbox.hpp"
#include "directx/Camera.hpp"
#include "effect.hpp"

#include <variant>

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

		void MakeAABB();
		void MakeOBB();
		void MakeSphere();

		void ProcessAABB(InstancedData* instanced_data);
		void ProcessOBB(InstancedData* instanced_data);
		void ProcessSphere(InstancedData* instanced_data);

	private:
		DX::Camera m_Camera;
		EffectManager_t m_EffectManager;

		Material m_SkullMat;
		std::variant<
			DX::BoundingBox, 
			DX::BoundingOrientedBox, 
			DX::BoundingSphere
		> m_SkullBoundings;
		std::vector<DX::VertexPositionNormalTexture> m_SkullPositions;

		uint32_t m_IndexCount{}, m_InstanceCount{};
		std::vector<InstancedData> m_InstancedWorld;

		DX::ComPtr<ID3D11Buffer> m_d3dSkullVB, m_d3dInstWorldVB, m_IndexBuffer;
		DX::ComPtr<ID3D11VertexShader> m_d3dVertexShader;
		DX::ComPtr<ID3D11InputLayout> m_d3dInputLayout;
		DX::ComPtr<ID3D11PixelShader> m_d3dPixelShader;

		bool m_FrustumCulling{ true };
	};
}