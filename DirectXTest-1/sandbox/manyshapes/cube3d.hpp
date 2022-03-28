#pragma once

#include "utils/pch.hpp"
#include "shapes.hpp"

namespace Pleiades::Sandbox
{
	class RenderableCube3D : public IRenderableShape
	{
	public:
		RenderableCube3D(DX::DeviceResources* d3dres, const char* name);

		void Render(uint64_t ticks) override;

	private:
		void InitializeBuffers();
		void InitializeShaders();

		void Rotate();

	private:
		float m_CurAngle{};

		DX::ComPtr<ID3D11Buffer>		m_CubeIBuffer, m_CubeVBuffer;
		DX::ComPtr<ID3D11Buffer>		m_CubeCBuffer_SingleColor, m_CubeCBuffer_Position;
		DX::ComPtr<ID3D11InputLayout>	m_CubeIL;
		DX::ComPtr<ID3D11VertexShader>	m_CubeVS;
		DX::ComPtr<ID3D11PixelShader>	m_CubePS;
	};
}