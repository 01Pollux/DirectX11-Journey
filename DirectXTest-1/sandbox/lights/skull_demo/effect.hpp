#pragma once

#include "../helper.hpp"
#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class EffectManager
	{
	public:
		struct WorldConstantBuffer
		{
			DX::XMMATRIX WorldViewProj;
			DX::XMMATRIX World;
			DX::XMMATRIX WorldInvTranspose;
			DX::XMFLOAT3 EyePosition;
			int			 LightCount;

			Material	 Material;

			DirectionalLight Lights[3];
		};
		static_assert((sizeof(WorldConstantBuffer) % 16) == 0, "Invalid size for d3d11 constant buffers");

		EffectManager() = default;
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
			m_BufferDirty = true;
		}

		void SetWorldViewProj(const DX::XMMATRIX& world_view_proj)
		{
			m_Buffer.WorldViewProj = world_view_proj;
			m_BufferDirty = true;
		}

		void SetWorldInvTranspose(const DX::XMMATRIX& world_inv_transpose)
		{
			m_Buffer.WorldInvTranspose = world_inv_transpose;
			m_BufferDirty = true;
		}

		void SetWorldEyePosition(const DX::XMFLOAT3& position)
		{
			m_Buffer.EyePosition = position;
			m_BufferDirty = true;
		}
		
		void SetMaterial(const Material& mat)
		{
			m_Buffer.Material = mat;
			m_BufferDirty = true;
		}

		void SetLight(size_t light_idx, const DirectionalLight& light)
		{
			m_Buffer.Lights[light_idx] = light;
			m_BufferDirty = true;
		}

		void SetLightCount(int light_count)
		{
			m_Buffer.LightCount = light_count;
			m_BufferDirty = true;
		}

		void Bind(ID3D11DeviceContext* d3dcontext)
		{
			d3dcontext->VSSetConstantBuffers(
				0, 1, m_d3dBuffer.GetAddressOf()
			);
			d3dcontext->PSSetConstantBuffers(
				0, 1, m_d3dBuffer.GetAddressOf()
			);
		}

		void Update(ID3D11DeviceContext* d3dcontext)
		{
			if (m_BufferDirty)
			{
				m_BufferDirty = false;
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
		bool m_BufferDirty{};
	};


#define ASSERT_FXC(Name, Offset, Size)												\
	static_assert(offsetof(EffectManager::WorldConstantBuffer, Name) == Offset);	\
	static_assert(sizeof(EffectManager::WorldConstantBuffer::Name) == Size)

	ASSERT_FXC(WorldViewProj, 0, 64);
	ASSERT_FXC(World, 64, 64);
	ASSERT_FXC(WorldInvTranspose, 128, 64);

	ASSERT_FXC(EyePosition, 192, 12);
	ASSERT_FXC(LightCount, 204, 4);
	ASSERT_FXC(Material, 208, 64);
	ASSERT_FXC(Lights, 272, 192);
}
