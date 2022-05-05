#pragma once

#include "sandbox/render/renderable.hpp"

#include "directx/Camera.hpp"

namespace Pleiades::Sandbox::SkyboxCubeMapDemo
{
	class Skybox : public IRenderable
	{
	public:
		struct VertexInput_t
		{
			// WorldPosition
			DX::XMFLOAT3 Position;
		};

		Skybox(
			DX::Camera* camera,
			DX::DeviceResources* d3dres,
			const wchar_t* cubetexture_path,
			float radius
		);

	protected:
		void BeginDraw(RenderConstantBuffer_t& cbuffer) override;
		void EndDraw(RenderConstantBuffer_t& cbuffer) override;

	private:
		void CreateBuffers();
		void CreateShaders();
		void CreateResources(
			const wchar_t* cubetexture_path
		);

		void UpdateWorld(RenderConstantBuffer_t& cbuffer);

	private:
		DX::ComPtr<ID3D11Buffer> m_d3dVB_Sky, m_d3dIB_Sky;

		DX::ComPtr<ID3D11VertexShader> m_d3dVS_Sky;
		DX::ComPtr<ID3D11InputLayout> m_d3dIL_Sky;

		DX::ComPtr<ID3D11PixelShader> m_d3dPS_Sky;
		
		DX::ComPtr<ID3D11ShaderResourceView> m_d3dSRV_SkyBox;
		DX::ComPtr<ID3D11SamplerState>		 m_d3dSampler_Sky;

		DX::ComPtr<ID3D11DepthStencilState>  m_d3dLessEqualDDS;
	};
}