
#include "utils/sandbox.hpp"
#include "directx/Camera.hpp"
#include "effect.hpp"

#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	class PickFromMouse : public ISandbox
	{
	public:
		using EffectManager_t = PickDemo::EffectManager;
		static constexpr uint32_t InvalidPickIndex = std::numeric_limits<uint32_t>::max();
		
		PickFromMouse(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;

		static const char* GetName() noexcept
		{
			return "Mouse pick";
		}

	private:
		void InitializeBuffers();
		void InitializeShaders();
		void InitializeDSS();

		void Bind();
		void Draw();
		void MouseThink();
		void FindToHightlight(float x_v, float y_v);
		void Highlight();

	private:
		DX::Camera m_Camera;
		EffectManager_t m_EffectManager;

		Material m_SkullMat, m_SelectMat;
		GeometryInstance::MeshData_t m_SkullMesh;
		DX::BoundingBox m_SkullBox;
		uint32_t m_HightlightIdx{ InvalidPickIndex };

		DX::ComPtr<ID3D11Buffer> m_d3dSkullVB, m_d3dSkullIB;
		DX::ComPtr<ID3D11VertexShader> m_d3dVertexShader;
		DX::ComPtr<ID3D11InputLayout> m_d3dInputLayout;
		DX::ComPtr<ID3D11PixelShader> m_d3dPixelShader;
		DX::ComPtr<ID3D11DepthStencilState> m_d3dAllowRedraw;
	};
}