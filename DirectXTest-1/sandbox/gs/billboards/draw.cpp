
#include "utils/pch.hpp"
#include "billboards.hpp"

namespace Pleiades::Sandbox
{
	void GSBillboardsDemo::DrawBillboard()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_BlendRenderState.SetAlphaTransparent(d3dcontext);

		d3dcontext->GSSetShader(m_d3dPointBillboardGS.Get(), nullptr, 0);
		m_PointBillboardGeometry.Bind(d3dcontext);

		d3dcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_Effects.Bind(d3dcontext);

		for (auto& pt : m_PointBilloards)
		{
			pt.Bind1(d3dres, m_Effects, m_ViewProjection);

			d3dcontext->Draw(
				1,
				0
			);
		}

		d3dcontext->GSSetShader(nullptr, nullptr, 0);

		m_BlendRenderState.SetAlphaTransparent(d3dcontext, false);
	}
}
