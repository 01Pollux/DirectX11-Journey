#pragma once

#include "directx/DeviceResources.hpp"
#include "sandbox/geometry_factory.hpp"

#include <map>

namespace Pleiades::Sandbox
{
	enum class ConsantBufferShader
	{
		VS,
		
		HS,
		DS,

		GS,
		CS,

		PS
	};
	
	class RenderConstantBuffer_t
	{
	public:
		RenderConstantBuffer_t(
			_In_ DX::DeviceResources* d3dres,
			_In_ uint8_t* buffer_data,
			_In_ size_t buffer_size,
			_In_ bool dynamic = true
		) :
			m_DeviceRes(d3dres)
		{
			CreateBuffers(1, &buffer_data, &buffer_size, &dynamic);
		}


		template<typename _Ty>
		RenderConstantBuffer_t(
			_In_ DX::DeviceResources* d3dres,
			uint8_t index,
			_In_ const _Ty& data,
			_In_ bool dynamic = true
		) :
			RenderConstantBuffer_t(
				d3dres,
				index,
				&data,
				sizeof(data),
				dynamic
			)
		{}


		RenderConstantBuffer_t(
			_In_ DX::DeviceResources* d3dres
		) noexcept :
			m_DeviceRes(d3dres)
		{}


		void CreateBuffers(
			size_t buffer_count,
			_In_count_c_(buffer_count) uint8_t** buffer_data,
			_In_count_c_(buffer_count) size_t* buffer_size,
			_In_count_c_(buffer_count) bool* dynamic
		)
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			D3D11_SUBRESOURCE_DATA subres_data{};
			D3D11_SUBRESOURCE_DATA* psubres_data;

			for (size_t i = 0; i < buffer_count; i++)
			{
				buffer_desc.ByteWidth = uint32_t(buffer_size[i]);
				buffer_desc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
				buffer_desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

				subres_data.pSysMem = buffer_data;
				psubres_data = buffer_data[i] ? &subres_data : nullptr;

				auto& buffer = m_d3dCBData.emplace_back();

				m_DeviceRes->GetD3DDevice()->CreateBuffer(
					&buffer_desc,
					psubres_data,
					m_d3dCB.emplace_back().GetAddressOf()
				);

				buffer.BufferSize = buffer_size[i];
				buffer.d3dBufferTmp = buffer_data[i];
			}
		}

		
		void MarkDirty(uint32_t buffer_index)
		{
			m_d3dCBData[buffer_index].BufferDirty = true;
		}


		void Update()
		{
			auto d3dcontext = m_DeviceRes->GetD3DDeviceContext();
			auto d3dcbuffer = m_d3dCB.begin();

			for (auto& buffer : m_d3dCBData)
			{
				if (!buffer.BufferDirty)
				{
					d3dcbuffer++;
					continue;
				}

				buffer.BufferDirty = true;
				D3D11_MAPPED_SUBRESOURCE subres_data{};

				DX::ThrowIfFailed(
					d3dcontext->Map(
						d3dcbuffer->Get(),
						0,
						D3D11_MAP_WRITE_DISCARD,
						0,
						&subres_data
					)
				);

				std::copy(
					buffer.d3dBufferTmp,
					buffer.d3dBufferTmp + buffer.BufferSize,
					static_cast<uint8_t*>(subres_data.pData)
				);

				d3dcontext->Unmap(
					d3dcbuffer->Get(),
					0
				);

				++d3dcbuffer;
			}
		}


		template<ConsantBufferShader _Shaders>
		void SetShaders(
			uint32_t start_slot, 
			uint32_t buffer_idx, 
			uint32_t count
		)
		{
			auto d3dcontext = m_DeviceRes->GetD3DDeviceContext();
			auto buffers = reinterpret_cast<ID3D11Buffer* const*>(m_d3dCB.data() + buffer_idx);

			if constexpr (_Shaders == ConsantBufferShader::VS)
				d3dcontext->VSSetConstantBuffers(start_slot, count, buffers);
			else if constexpr (_Shaders == ConsantBufferShader::VS)
				d3dcontext->VSSetConstantBuffers(start_slot, count, buffers);
			else if constexpr (_Shaders == ConsantBufferShader::HS)
				d3dcontext->HSSetConstantBuffers(start_slot, count, buffers);
			else if constexpr (_Shaders == ConsantBufferShader::DS)
				d3dcontext->DSSetConstantBuffers(start_slot, count, buffers);
			else if constexpr (_Shaders == ConsantBufferShader::GS)
				d3dcontext->GSSetConstantBuffers(start_slot, count, buffers);
			else if constexpr (_Shaders == ConsantBufferShader::CS)
				d3dcontext->CSSetConstantBuffers(start_slot, count, buffers);
			else if constexpr (_Shaders == ConsantBufferShader::PS)
				d3dcontext->PSSetConstantBuffers(start_slot, count, buffers);
		}


		[[nodiscard]]
		uint32_t GetHighestIndex() const noexcept
		{
			return uint32_t(m_d3dCB.size());
		}

		template<typename _Ty>
		[[nodiscard]]
		_Ty* Data(size_t buffer_idx)
		{
			return std::bit_cast<_Ty*>(m_d3dCBData[buffer_idx].d3dBufferTmp);
		}

	private:
		struct BufferData_t
		{
			uint8_t*	d3dBufferTmp;
			size_t		BufferSize;
			bool		BufferDirty{};
		};

		std::vector<BufferData_t>			  m_d3dCBData;
		std::vector<DX::ComPtr<ID3D11Buffer>> m_d3dCB;

		DX::DeviceResources* m_DeviceRes;
	};
}