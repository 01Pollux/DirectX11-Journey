
#include "utils/pch.hpp"
#include "subdivison.hpp"


namespace Pleiades::Sandbox
{
	void GSSubdivisonDemo::DrawIcosahedron()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_SphereGeometry.Bind(d3dcontext);
		m_Sphere.Bind(d3dres, m_Effects, m_ViewProjection);

		d3dcontext->GSSetShader(m_GSTriangle.Get(), nullptr, 0);
		m_Effects.Bind(d3dcontext);

		m_SphereGeometry.Draw(d3dcontext);
		d3dcontext->GSSetShader(nullptr, nullptr, 0);
	}
}
