#pragma once

#include "utils/pch.hpp"
#include "shapes.hpp"

namespace Pleiades::Sandbox
{
	class RenderableCylinder : public IRenderableShape
	{
	public:
		struct Vertex_t
		{
			DX::XMFLOAT4 Position;
		};

		struct ConstantPosition_t
		{
			DirectX::XMMATRIX Transformation;
		};

		struct SingleColorInSquare_t
		{
			float Color[4];
		};

	public:
		RenderableCylinder(DX::DeviceResources* d3dres, const char* name);

		void Render(uint64_t ticks) override;
		void ImGuiRender() override;

	private:
		void BuildCylinderMesh();
		void InitializeBuffers();
		void InitializeShaders();
		
		void UpdateScene();

	private:
		float m_RotationOffset[3]{ -0.41f, 0.06f, 0.f, };
		float m_CylinderRadius[2]{ 0.5f, 0.3f };
		int32_t m_SlicesStacks[2]{ 20, 20 };
		float m_CylinderHeight = 3.f;

		GeometryFactory::MeshData_t		m_CylinderMesh;

		DX::ComPtr<ID3D11Buffer>		m_CylinderIndicies, m_CylinderVerticies;
		DX::ConstantBuffer<DX::XMMATRIX>m_CylinderConstants_WRP;

		DX::ComPtr<ID3D11InputLayout>	m_CylinderInputLayout;
		DX::ComPtr<ID3D11VertexShader>	m_CylinderVtxShader;
		DX::ComPtr<ID3D11PixelShader>	m_CylinderPxlShader;
	};
}