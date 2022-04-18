
#include "utils/pch.hpp"
#include "mirror.hpp"


namespace Pleiades::Sandbox
{
	void MirrorSkullWorld::DrawWorld()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_Effects.SetTexture(m_WallTexture.Get());
		m_Effects.Bind(d3dcontext);

		m_WallGeometry.Bind(d3dcontext);
		m_Wall.Bind(d3dres, m_Effects, m_ViewProjection);

		m_WallGeometry.Draw(d3dcontext);
	}

	void MirrorSkullWorld::DrawSkull()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_Effects.SetTexture(m_SkullTexture.Get());
		m_Effects.Bind(d3dcontext);

		m_SkullGeometry.Bind(d3dcontext);
		m_Skull.Bind(d3dres, m_Effects, m_ViewProjection);

		m_SkullGeometry.Draw(d3dcontext);
	}
	
	void MirrorSkullWorld::DrawMirror_Stencil()
	{
	}
	
	void MirrorSkullWorld::DrawSkull_Reflection()
	{
	}
	
	void MirrorSkullWorld::DrawMirror()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_BlendRenderState.SetAlphaTransparent(d3dcontext);

		m_Effects.SetTexture(m_MirrorTexture.Get());
		m_Effects.Bind(d3dcontext);

		m_MirrorGeometry.Bind(d3dcontext);
		m_Mirror.Bind(d3dres, m_Effects, m_ViewProjection);

		m_MirrorGeometry.Draw(d3dcontext);

		m_BlendRenderState.SetAlphaTransparent(d3dcontext, false);
	}
}
