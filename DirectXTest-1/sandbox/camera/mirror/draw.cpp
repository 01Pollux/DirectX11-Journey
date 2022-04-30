
#include "utils/pch.hpp"
#include "mirror.hpp"


namespace Pleiades::Sandbox
{
	void CamMirrorSkullWorld::DrawWorld()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_Effects.SetTexture(m_WallTexture.Get());
		m_Effects.Bind(d3dcontext);

		m_WallGeometry.Bind(d3dcontext);
		m_Wall.Bind(d3dres, m_Effects, DX::XMMatrixTranspose(m_Camera.get_viewprojection()));

		m_WallGeometry.Draw(d3dcontext);
	}


	void CamMirrorSkullWorld::DrawSkull()
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_Effects.SetTexture(m_SkullTexture.Get());
		m_Effects.Bind(d3dcontext);

		m_SkullGeometry.Bind(d3dcontext);
		m_Skull.Bind(d3dres, m_Effects, DX::XMMatrixTranspose(m_Camera.get_viewprojection()));

		m_SkullGeometry.Draw(d3dcontext);
	}

	
	void CamMirrorSkullWorld::DrawMirror_Stencil()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_BlendRenderState.SetNoDraw(d3dcontext);
		m_BlendRenderState.SetStencilMask(d3dcontext);

		DrawMirror(true);
		
		m_BlendRenderState.SetStencilMask(d3dcontext, false);
		m_BlendRenderState.SetNoDraw(d3dcontext, false);
	}
	
	
	void CamMirrorSkullWorld::DrawSkull_Reflection()
	{
		DX::XMMATRIX reflection_plane = DX::XMMatrixReflect(DX::XMVectorSet(0.f, 0.f, 1.f, 0.f));
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		DX::XMMATRIX old_skull = m_Skull.World;
		DX::XMMATRIX old_world = m_Wall.World;
		DX::XMFLOAT3 old_light_dir = m_Effects.Buffer().Light.Direction;

		DX::XMStoreFloat3(
			&m_Effects.Buffer().Light.Direction,
			DX::XMVector2TransformNormal(DX::XMLoadFloat3(&old_light_dir), reflection_plane)
		);
		m_Skull.World *= reflection_plane;
		m_Wall.World *= reflection_plane;

		m_BlendRenderState.ApplyStencilMask(d3dcontext);
		m_BlendRenderState.SetReverseCull(d3dcontext);

		DrawWorld();
		DrawSkull();

		m_BlendRenderState.SetReverseCull(d3dcontext, false);
		m_BlendRenderState.ApplyStencilMask(d3dcontext, false);

		m_Skull.World = old_skull;
		m_Wall.World = old_world;
		m_Effects.Buffer().Light.Direction = old_light_dir;
	}
	

	void CamMirrorSkullWorld::DrawMirror(bool stencil)
	{
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		if (!stencil)
			m_BlendRenderState.SetAlphaTransparent(d3dcontext);

		m_Effects.SetTexture(m_MirrorTexture.Get());
		m_Effects.Bind(d3dcontext);

		m_MirrorGeometry.Bind(d3dcontext);
		m_Mirror.Bind(d3dres, m_Effects, DX::XMMatrixTranspose(m_Camera.get_viewprojection()));

		m_MirrorGeometry.Draw(d3dcontext);

		if (!stencil)
			m_BlendRenderState.SetAlphaTransparent(d3dcontext, false);
	}


	void CamMirrorSkullWorld::DrawSkull_Shadow()
	{
		
		auto d3dres = GetDeviceResources();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		m_BlendRenderState.SetStencilBlendOnce(d3dcontext);
		m_BlendRenderState.SetAlphaTransparent(d3dcontext);

		auto old_skull_info = m_Skull;

		m_Skull.World *=
			DX::XMMatrixShadow(DX::XMVectorSet(0.f, 1.f, 0.f, 0.f), DX::XMVectorNegate(DX::XMLoadFloat3(&m_Effects.Buffer().Light.Direction))) *
			DX::XMMatrixTranslation(0.f, .001f, 0.f);

		m_Skull.Material.Ambient = DX::XMVectorSet(0.f, 0.f, 0.f, 1.f);
		m_Skull.Material.Diffuse = DX::XMVectorSet(0.f, 0.f, 0.f, .5f);
		m_Skull.Material.Specular = DX::XMVectorSet(0.f, 0.f, 0.f, 16.f);

		DrawSkull();

		m_Skull = old_skull_info;

		m_BlendRenderState.SetAlphaTransparent(d3dcontext, false);
		m_BlendRenderState.SetStencilBlendOnce(d3dcontext, false);
	}
}
