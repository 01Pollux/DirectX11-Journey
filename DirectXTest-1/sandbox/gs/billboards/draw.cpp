
#include "utils/pch.hpp"
#include "billboards.hpp"

namespace Pleiades::Sandbox
{
	void GSBillboards::DrawCube()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_SphereGeometry.Bind(d3dcontext);
		m_Sphere.Bind(d3dres, m_Effects, m_ViewProjection);

		d3dcontext->GSSetShader(m_GSTriangle.Get(), nullptr, 0);
		m_Effects.Bind(d3dcontext);

		m_SphereGeometry.Draw(d3dcontext);
	}
}
