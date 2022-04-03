#pragma once

#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class SimplePlane : public ISandbox
	{
	public:
		SimplePlane(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;
		void OnImGuiDraw() override;

		static const char* GetName() noexcept
		{
			return "Simple plane";
		}

	private:
		void BuildPlaneMesh();
		void InitializeBuffers();
		void InitializeShaders();

		void UpdateScene();

	private:
		float m_RotationOffset[3]{ -0.41f, 0.06f, 0.f, };
		float m_DrawOffset[3]{ 0.f, -32.f, 367.f };
		float m_PlaneSize[2]{ 160.f, 160.f };
		int32_t m_RowCols[2]{ 50, 50 };

		GeometryFactory::MeshData_t		m_PlaneMesh;

		DX::ComPtr<ID3D11Buffer>		m_PlaneIndicies, m_PlaneVerticies;
		DX::ConstantBuffer<DX::XMMATRIX>m_PlaneConstants_WRP;

		DX::ComPtr<ID3D11InputLayout>	m_PlaneInputLayout;
		DX::ComPtr<ID3D11VertexShader>	m_PlaneVtxShader;
		DX::ComPtr<ID3D11PixelShader>	m_PlanePxlShader;
	};
}