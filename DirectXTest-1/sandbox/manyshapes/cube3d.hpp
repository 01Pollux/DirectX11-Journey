#pragma once

#include "utils/pch.hpp"
#include "shapes.hpp"

namespace Pleiades::Sandbox
{
	class RenderableCube3D : public IRenderableShape
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
		RenderableCube3D(DX::DeviceResources* d3dres, const char* name);

		void Render(uint64_t ticks) override;

	private:
		void InitializeBuffers();
		void InitializeShaders();

		void Rotate();

	private:
		float m_CurAngle{};

		DX::ComPtr<ID3D11Buffer>				m_CubeIndicies, m_CubeVerticies;

		DX::ComPtr<ID3D11Buffer>				m_CubeCBuffer_SingleColor;
		DX::ConstantBuffer<ConstantPosition_t>	m_CubeCBuffer_Position;

		DX::ComPtr<ID3D11InputLayout>			m_CubeInputLayout;
		DX::ComPtr<ID3D11VertexShader>			m_CubeVtxShader;
		DX::ComPtr<ID3D11PixelShader>			m_CubePxlShader;
	};
}