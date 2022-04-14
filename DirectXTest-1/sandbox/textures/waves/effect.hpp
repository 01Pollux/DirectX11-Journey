#pragma once

#include "sandbox/helper.hpp"
#include "utils/sandbox.hpp"
#include <bitset>

namespace Pleiades::Sandbox::WavesTextureEffect
{
	class EffectManager
	{
	public:
		struct NonNumericConstants
		{
			DX::ComPtr<ID3D11ShaderResourceView> Texture;
			DX::ComPtr<ID3D11SamplerState> Sampler;
		};

		struct WorldConstantBuffer
		{
			DX::XMMATRIX WorldViewProj;
			DX::XMMATRIX World;
			DX::XMMATRIX WorldInvTranspose;

			Material	 Material;

			DirectionalLight Light;

			DX::XMFLOAT3 EyePosition;
		};
		static_assert((sizeof(WorldConstantBuffer) % 16) == 0, "Invalid size for d3d11 constant buffers");

		EffectManager(ID3D11Device* d3ddevice, NonNumericConstants&& shader_info, const WorldConstantBuffer& data = {}) :
			m_Buffer(data), m_NBuffer(std::move(shader_info))
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

		void SetWorldViewProj(const DX::XMMATRIX& world_view_proj)
		{
			m_Buffer.WorldViewProj = world_view_proj;
			m_DirtyFlags[0] = true;
		}

		void SetWorldInvTranspose(const DX::XMMATRIX& world_inv_transpose)
		{
			m_Buffer.WorldInvTranspose = world_inv_transpose;
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

		void SetTexture(ID3D11ShaderResourceView* texture)
		{
			m_NBuffer.Texture = texture;
		}

		void Bind(ID3D11DeviceContext* d3dcontext)
		{
			d3dcontext->VSSetConstantBuffers(
				0, 1, m_d3dBuffer.GetAddressOf()
			);
			d3dcontext->PSSetConstantBuffers(
				0, 1, m_d3dBuffer.GetAddressOf()
			);

			d3dcontext->PSSetShaderResources(
				0, 1, m_NBuffer.Texture.GetAddressOf()
			);
			d3dcontext->PSSetSamplers(
				0, 1, m_NBuffer.Sampler.GetAddressOf()
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
		NonNumericConstants& NonNumericBuffer() noexcept
		{
			return m_NBuffer;
		}

		[[nodiscard]]
		WorldConstantBuffer& Buffer() noexcept
		{
			return m_Buffer;
		}

	private:
		WorldConstantBuffer m_Buffer;
		NonNumericConstants m_NBuffer;

		DX::ComPtr<ID3D11Buffer> m_d3dBuffer;
		std::bitset<1> m_DirtyFlags;
	};
}
