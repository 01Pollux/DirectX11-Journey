
#include "utils/pch.hpp"
#include "pick.hpp"

#include "sandbox/geometry_factory.hpp"
#include "imgui/imgui.hpp"

namespace Pleiades::Sandbox
{
	void PickFromMouse::OnFrame(uint64_t ticks)
	{
		m_Camera.update(ticks);
		this->Bind();
		this->Draw();
		this->MouseThink();
		this->Highlight();
	}


	void PickFromMouse::Bind()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		d3dcontext->IASetInputLayout(m_d3dInputLayout.Get());
		d3dcontext->IASetIndexBuffer(m_d3dSkullIB.Get(), DXGI_FORMAT_R16_UINT, 0);

		ID3D11Buffer* buffers[]{ m_d3dSkullVB.Get() };
		uint32_t strides[]{ sizeof(GeometryInstance::MeshData_t::verticies_type) };
		uint32_t offsets[1]{ };

		d3dcontext->IASetVertexBuffers(0, 1, buffers, strides, offsets);

		d3dcontext->VSSetShader(m_d3dVertexShader.Get(), nullptr, 0);
		d3dcontext->PSSetShader(m_d3dPixelShader.Get(), nullptr, 0);

		m_EffectManager.Bind(d3dcontext);

		m_EffectManager.SetMaterial(m_SkullMat);
		m_EffectManager.SetWorldEyePosition(m_Camera.position());
		m_EffectManager.SetWorldViewProj(DX::XMMatrixIdentity(), m_Camera.get_tviewprojection());

		m_EffectManager.Update(d3dcontext);
	}


	void PickFromMouse::Draw()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->DrawIndexed(
			uint32_t(m_SkullMesh.indices.size()),
			0,
			0
		);
	}

	
	void PickFromMouse::Highlight()
	{
		if (m_HightlightIdx == this->InvalidPickIndex)
			return;

		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->OMSetDepthStencilState(m_d3dAllowRedraw.Get(), 0);

		m_EffectManager.SetMaterial(m_SelectMat);
		m_EffectManager.Update(d3dcontext);

		d3dcontext->DrawIndexed(
			3,
			m_HightlightIdx,
			0
		);

		d3dcontext->OMSetDepthStencilState(nullptr, 0);
	}


	void PickFromMouse::MouseThink()
	{
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
			return;

		POINT mouse;
		if (!GetCursorPos(&mouse))
			return;

		D3D11_VIEWPORT vp = GetDeviceResources()->GetScreenViewport();

		float x_v = (2.f * (mouse.x / vp.Width) - 1.f);
		float y_v = (-2.f * (mouse.y / vp.Height) + 1.f);

		x_v /= m_Camera.get_projection().r[0].m128_f32[0];
		y_v /= m_Camera.get_projection().r[1].m128_f32[1];

		FindToHightlight(x_v, y_v);
	}


	void PickFromMouse::FindToHightlight(float x_v, float y_v)
	{
		using namespace DX;
		m_HightlightIdx = InvalidPickIndex;

		XMVECTOR ray_origin{};
		XMVECTOR ray_dir = DX::XMVectorSet(x_v, y_v, 1.f, 0.f);

		XMMATRIX to_local = XMMatrixMultiply(
			XMMatrixInverse(nullptr, m_Camera.get_view()),
			XMMatrixInverse(nullptr, XMMatrixIdentity())
		);

		ray_origin = XMVector3TransformCoord(ray_origin, to_local);
		ray_dir = XMVector3Normalize(XMVector3TransformNormal(ray_dir, to_local));

		float dist;
		if (!m_SkullBox.Intersects(ray_origin, ray_dir, dist))
			return;

		dist = std::numeric_limits<float>::infinity();
		float min_dist = dist;

		for (size_t i = 0; i < m_SkullMesh.indices.size() / 3; i++)
		{
			uint16_t t0 = m_SkullMesh.indices[i * 3 + 0];
			uint16_t t1 = m_SkullMesh.indices[i * 3 + 1];
			uint16_t t2 = m_SkullMesh.indices[i * 3 + 2];

			XMVECTOR v0 = XMLoadFloat3(&m_SkullMesh.vertices[t0].position);
			XMVECTOR v1 = XMLoadFloat3(&m_SkullMesh.vertices[t1].position);
			XMVECTOR v2 = XMLoadFloat3(&m_SkullMesh.vertices[t2].position);

			if (!DX::TriangleTests::Intersects(ray_origin, ray_dir, v0, v1, v2, dist))
				continue;

			if (min_dist > dist)
			{
				min_dist = dist;
				m_HightlightIdx = uint16_t(i * 3);
			}
		}
	}
}