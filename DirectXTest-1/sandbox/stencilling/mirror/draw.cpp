
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
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_BlendRenderState.SetNoDraw(d3dcontext);
		m_BlendRenderState.SetStencilMask(d3dcontext);

		DrawMirror(true);
		
		m_BlendRenderState.SetStencilMask(d3dcontext, false);
		m_BlendRenderState.SetNoDraw(d3dcontext, false);
	}
	
	
	void MirrorSkullWorld::DrawSkull_Reflection()
	{
		DX::XMMATRIX reflection_plane = DX::XMMatrixReflect(DX::XMVectorSet(0.f, 0.f, 1.f, 0.f));
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		DX::XMMATRIX old_world = m_Skull.World;
		DX::XMFLOAT3 old_light_dir = m_Effects.Buffer().Light.Direction;

		DX::XMStoreFloat3(
			&m_Effects.Buffer().Light.Direction,
			DX::XMVector2TransformNormal(DX::XMLoadFloat3(&old_light_dir), reflection_plane)
		);
		m_Skull.World *= reflection_plane;

		m_BlendRenderState.ApplyStencilMask(d3dcontext);
		m_BlendRenderState.SetReverseCull(d3dcontext);

		DrawSkull();

		m_BlendRenderState.SetReverseCull(d3dcontext, false);
		m_BlendRenderState.ApplyStencilMask(d3dcontext, false);

		m_Skull.World = old_world;
		m_Effects.Buffer().Light.Direction = old_light_dir;
	}
	

	void MirrorSkullWorld::DrawMirror(bool stencil)
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		if (!stencil)
			m_BlendRenderState.SetAlphaTransparent(d3dcontext);

		m_Effects.SetTexture(m_MirrorTexture.Get());
		m_Effects.Bind(d3dcontext);

		m_MirrorGeometry.Bind(d3dcontext);
		m_Mirror.Bind(d3dres, m_Effects, m_ViewProjection);

		m_MirrorGeometry.Draw(d3dcontext);

		if (!stencil)
			m_BlendRenderState.SetAlphaTransparent(d3dcontext, false);
	}
}
