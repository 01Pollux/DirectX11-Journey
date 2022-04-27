#pragma once

#include "utils/pch.hpp"
#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class BezierTesselation : public ISandbox
	{
	public:
		static constexpr uint32_t NumVecs = 32;

		struct VertexInput_t
		{
			DX::XMFLOAT3 PosL;
		};

		struct WorldConstant_t
		{
			DX::XMMATRIX World = DX::XMMatrixIdentity();
			DX::XMMATRIX ViewProj;
			DX::XMFLOAT4 TessFactor{ 25.f, 25.f, 25.f, 25.f, };
			DX::XMFLOAT2 InsideTessFactor{ 15.f, 15.f };
			DX::XMFLOAT3 EyePosition{ 0.f, 0.f, 80.f, };
		};

		BezierTesselation(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Bezier tesselation";
		}

	private:
		void CreateBuffers();
		void CreateShaders();

		void UpdateViewProj()
		{
			m_WorldConstant.ViewProj =
				DX::XMMatrixRotationRollPitchYawFromVector(DX::XMLoadFloat3(&m_Rotation)) *
				DX::XMMatrixTranslationFromVector(DX::XMLoadFloat3(&m_WorldConstant.EyePosition)) *
				DX::XMMatrixPerspectiveLH(
					DX::XM_PIDIV2,
					GetDeviceResources()->GetAspectRatio(),
					1.f, 10000.f
				);

			m_WorldConstant.ViewProj = DX::XMMatrixTranspose(m_WorldConstant.ViewProj);
		}

		void UpdateConstantBuffer();

	private:
		std::array<VertexInput_t, 16>	m_VertexInput;
		DX::XMFLOAT3					m_Rotation{ 0.14f, -2.03f, 0.64f };
		WorldConstant_t					m_WorldConstant;

		DX::ComPtr<ID3D11Buffer>		m_ConstantBuffer;
		DX::ComPtr<ID3D11Buffer>		m_VertexBuffer;

		DX::ComPtr<ID3D11InputLayout>	m_InputLayout;
		DX::ComPtr<ID3D11VertexShader>	m_VertexShader;
		DX::ComPtr<ID3D11HullShader>	m_HullShader;
		DX::ComPtr<ID3D11DomainShader>	m_DomainShader;
		DX::ComPtr<ID3D11PixelShader>	m_PixelShader;
	};
}