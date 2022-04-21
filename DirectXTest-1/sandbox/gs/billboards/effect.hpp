#pragma once

#include "sandbox/helper.hpp"
#include "utils/sandbox.hpp"
#include <bitset>

namespace Pleiades::Sandbox::GSBillboardsDemoFx
{
	class EffectManager
	{
	public:
		struct WorldConstantBuffer
		{
			DX::XMMATRIX World;
			DX::XMMATRIX ViewProj;

			Material	 Material;
			DirectionalLight Light;

			DX::XMFLOAT3 EyePosition;
		};
		static_assert((sizeof(WorldConstantBuffer) % 16) == 0, "Invalid size for d3d11 constant buffers");

		EffectManager(ID3D11Device* d3ddevice, const WorldConstantBuffer& data = {}) :
			m_Buffer(data)
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.ByteWidth = sizeof(m_Buffer);

			D3D11_SUBRESOURCE_DATA subres_data{};
			subres_data.pSysMem = &m_Buffer;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_data,
					m_d3dBuffer.GetAddressOf()
				)
			);
		}

		void SetWorld(const DX::XMMATRIX& world)
		{
			m_Buffer.World = world;
			m_DirtyFlags[0] = true;
		}

		void SetViewProj(const DX::XMMATRIX& view_proj)
		{
			m_Buffer.ViewProj = view_proj;
			m_DirtyFlags[0] = true;
		}

		void SetWorldEyePosition(const DX::XMFLOAT3& position)
		{
			m_Buffer.EyePosition = position;
			m_DirtyFlags[0] = true;
		}
		
		void SetMaterial(const Material& mat)
		{
			m_Buffer.Material = mat;
			m_DirtyFlags[0] = true;
		}

		void SetLight(const DirectionalLight& light)
		{
			m_Buffer.Light = light;
			m_DirtyFlags[0] = true;
		}

		void Bind(ID3D11DeviceContext* d3dcontext)
		{
			d3dcontext->VSSetConstantBuffers(
				0, 1, m_d3dBuffer.GetAddressOf()
			);
			d3dcontext->PSSetConstantBuffers(
				0, 1, m_d3dBuffer.GetAddressOf()
			);
			d3dcontext->GSSetConstantBuffers(
				0, 1, m_d3dBuffer.GetAddressOf()
			);
		}

		void Update(ID3D11DeviceContext* d3dcontext)
		{
			if (m_DirtyFlags[0])
			{
				m_DirtyFlags[0] = false;
				d3dcontext->UpdateSubresource(
					m_d3dBuffer.Get(),
					0,
					nullptr,
					&m_Buffer,
					sizeof(m_Buffer),
					0
				);
			}
		}

		[[nodiscard]]
		WorldConstantBuffer& Buffer() noexcept
		{
			return m_Buffer;
		}

	private:
		WorldConstantBuffer m_Buffer;

		DX::ComPtr<ID3D11Buffer> m_d3dBuffer;
		std::bitset<1> m_DirtyFlags;
	};
}
