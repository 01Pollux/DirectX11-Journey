
#include "utils/pch.hpp"
#include "instancing_culling.hpp"

#include "sandbox/geometry_factory.hpp"

namespace Pleiades::Sandbox
{
	void InstancedFrustum::OnFrame(uint64_t ticks)
	{
		m_Camera.update(ticks);
		this->WriteInstances();
		this->Bind();
		this->Draw();
	}


	void InstancedFrustum::OnImGuiDraw()
	{

	}


	void InstancedFrustum::Bind()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		d3dcontext->IASetInputLayout(m_d3dInputLayout.Get());
		d3dcontext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		ID3D11Buffer* buffers[]{ m_d3dSkullVB.Get(), m_d3dInstWorldVB.Get() };
		uint32_t strides[]{ sizeof(GeometryInstance::MeshData_t::verticies_type), sizeof(InstancedData) };
		uint32_t offsets[2]{ };

		d3dcontext->IASetVertexBuffers(0, 2, buffers, strides, offsets);

		d3dcontext->VSSetShader(m_d3dVertexShader.Get(), nullptr, 0);
		d3dcontext->PSSetShader(m_d3dPixelShader.Get(), nullptr, 0);

		m_EffectManager.Bind(d3dcontext);

		m_EffectManager.SetMaterial(m_SkullMat);
		m_EffectManager.SetWorldEyePosition(m_Camera.position());
		m_EffectManager.SetViewProj(m_Camera.get_viewprojection());

		m_EffectManager.Update(d3dcontext);
	}


	void InstancedFrustum::WriteInstances()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();
		m_InstanceCount = 0;

		if (false)
		{

		}
		else
		{
			D3D11_MAPPED_SUBRESOURCE subres_data;
			DX::ThrowIfFailed(
				d3dcontext->Map(
					m_d3dInstWorldVB.Get(),
					0,
					D3D11_MAP_WRITE_DISCARD,
					0,
					&subres_data
				)
			);

			m_InstanceCount = static_cast<uint32_t>(m_InstancedWorld.size());
			std::copy_n(m_InstancedWorld.data(), m_InstanceCount, reinterpret_cast<InstancedData*>(subres_data.pData));
			d3dcontext->Unmap(m_d3dInstWorldVB.Get(), 0);
		}
	}


	void InstancedFrustum::Draw()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->DrawIndexedInstanced(
			m_IndexCount,
			m_InstanceCount,
			0,
			0,
			0
		);
	}
}