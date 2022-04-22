
#include "utils/pch.hpp"
#include "billboards.hpp"

namespace Pleiades::Sandbox
{
	void GSBillboardsDemo::DrawBillboard()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		if (m_AlphaToCoverage)
			m_BlendRenderState.SetAlphaToCoverageTransparent(d3dcontext);
		else
			m_BlendRenderState.SetAlphaTransparent(d3dcontext);

		d3dcontext->GSSetShader(m_d3dPointBillboardGS.Get(), nullptr, 0);
		m_PointBillboardGeometry.Bind(d3dcontext);

		d3dcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_Effects.Bind(d3dcontext);

		float x_offset = 0.f, z_offset = 0.f;
		bool swap = false;

		for (uint32_t texid = 0; auto pt : m_PointBilloards)
		{
			for (size_t i = 0; i < 5; i++)
			{
				m_Effects.SetTexId(texid++);
				pt.World *= DX::XMMatrixTranslation(x_offset, 0.f, z_offset);
				pt.Bind1(d3dres, m_Effects, m_ViewProjection);

				d3dcontext->Draw(
					1,
					0
				);

				x_offset += 3.f;
				z_offset += 2.f;

				if (swap)
					x_offset *= -1.f;

				swap = !swap;
			}
		}

		d3dcontext->GSSetShader(nullptr, nullptr, 0);

		m_BlendRenderState.SetAlphaToCoverageTransparent(d3dcontext, false);
	}
}
