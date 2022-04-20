
#include "utils/pch.hpp"
#include "subdivison.hpp"


namespace Pleiades::Sandbox
{
	void GSSubdivisonDemo::DrawCube()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		d3dcontext->GSSetShader(m_GSTriangle.Get(), nullptr, 0);
		m_Effects.Bind(d3dcontext);

		m_CubeGeometry.Bind(d3dcontext);
		m_Cube.Bind(d3dres, m_Effects, m_ViewProjection);

		m_CubeGeometry.Draw(d3dcontext);
	}
}
