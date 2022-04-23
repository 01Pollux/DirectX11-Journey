
#include "utils/pch.hpp"
#include "vec_add.hpp"

#include <fstream>
#include <format>

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	ComputeShader_VecAdd::ComputeShader_VecAdd(DX::DeviceResources* d3dres) :
		ISandbox(d3dres)
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		// create compute shader
		{
			DX::ComPtr<ID3DBlob> shader_blob;

			DX::ThrowIfFailed(
				D3DReadFileToBlob(
					L"resources/cs/vec_add_cs.cso",
					shader_blob.GetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateComputeShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_VecAddCS.GetAddressOf()
				)
			);
		}

		// create structuerd buffers for output 
		// + temp buffer to transfer data from gpu to cpu
		{
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.ByteWidth = NumVecs * sizeof(VecAdd_t);
			buffer_desc.StructureByteStride = sizeof(VecAdd_t);
			buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					nullptr,
					m_VecAddUAV_buffer.GetAddressOf()
				)
			);

			buffer_desc.Usage = D3D11_USAGE_STAGING;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			buffer_desc.BindFlags = 0;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					nullptr,
					m_VecAddUAV_buffertmp.GetAddressOf()
				)
			);
		}

		// create uav for the output buffer
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
			uav_desc.Format = DXGI_FORMAT_UNKNOWN;
			uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav_desc.Buffer.NumElements = NumVecs;

			DX::ThrowIfFailed(
				d3ddevice->CreateUnorderedAccessView(
					m_VecAddUAV_buffer.Get(),
					&uav_desc,
					m_VecAddUAV.GetAddressOf()
				)
			);
		}


		// create structured buffers for vecA and vecB
		{
			DX::ComPtr<ID3D11Buffer> d3dbuffer;
			VecAdd_t a[NumVecs], b[NumVecs];

			for (uint32_t i = 0; i < NumVecs; i++)
			{
				float f = float(i) + 1.f;
				a[i].v0 = { f, f + 1, f + 2 };
				a[i].v1 = { f + 3, f + 4 };

				b[i].v0 = { f * 2, f * 2 + 1, f * 2 + 2 };
				b[i].v1 = { f * 2 + 3, f * 2 + 4 };
			}


			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth = sizeof(a);
			buffer_desc.StructureByteStride = sizeof(VecAdd_t);
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;


			D3D11_SHADER_RESOURCE_VIEW_DESC sv_desc{};
			sv_desc.Format = DXGI_FORMAT_UNKNOWN;
			sv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			sv_desc.BufferEx.NumElements = NumVecs;


			D3D11_SUBRESOURCE_DATA subres_desc{};
			subres_desc.pSysMem = a;


			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_desc,
					d3dbuffer.GetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateShaderResourceView(
					d3dbuffer.Get(),
					&sv_desc,
					m_VecA.GetAddressOf()
				)
			);


			subres_desc.pSysMem = b;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&subres_desc,
					d3dbuffer.ReleaseAndGetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateShaderResourceView(
					d3dbuffer.Get(),
					&sv_desc,
					m_VecB.GetAddressOf()
				)
			);

			std::ofstream file("resources/cs/vec_add_ex.txt");

			for (uint32_t i = 0; i < NumVecs; i++)
			{
				file <<
					std::format(
						"[ v0: <{:.2f}, {:.2f}, {:.2f}>, v1: <{:.2f}, {:.2f}> ]\n",
						a[i].v0.x, a[i].v0.y, a[i].v0.z,
						a[i].v1.x, a[i].v1.y
					);
			}

			file << "\n\n\n";

			for (uint32_t i = 0; i < NumVecs; i++)
			{
				file <<
					std::format(
						"[ v0: <{:.2f}, {:.2f}, {:.2f}>, v1: <{:.2f}, {:.2f}> ]\n",
						b[i].v0.x, b[i].v0.y, b[i].v0.z,
						b[i].v1.x, b[i].v1.y
					);
			}
		}
	}

	void ComputeShader_VecAdd::OnFrame(uint64_t)
	{
		if (m_Processed)
			return;
		m_Processed = true;

		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->CSSetShader(m_VecAddCS.Get(), nullptr, 0);
		d3dcontext->VSSetShader(nullptr, nullptr, 0);
		d3dcontext->PSSetShader(nullptr, nullptr, 0);

		d3dcontext->CSSetUnorderedAccessViews(0, 1, m_VecAddUAV.GetAddressOf(), nullptr);

		ID3D11ShaderResourceView* vec[]{ m_VecA.Get(), m_VecB.Get() };
		d3dcontext->CSSetShaderResources(0, 2, vec);

		d3dcontext->Dispatch(1, 1, 1);

		d3dcontext->CopyResource(m_VecAddUAV_buffertmp.Get(), m_VecAddUAV_buffer.Get());

		D3D11_MAPPED_SUBRESOURCE mapped_vecadd;
		DX::ThrowIfFailed(
			d3dcontext->Map(
				m_VecAddUAV_buffertmp.Get(),
				0,
				D3D11_MAP_READ,
				0,
				&mapped_vecadd
			)
		);

		{
			VecAdd_t* vecadd = std::bit_cast<VecAdd_t*>(mapped_vecadd.pData);
			std::ofstream file("resources/cs/vec_add.txt");

			for (uint32_t i = 0; i < NumVecs; i++, vecadd++)
			{
				file << 
					std::format(
						"[ v0: <{:.2f}, {:.2f}, {:.2f}>, v1: <{:.2f}, {:.2f}> ]\n",
						vecadd->v0.x, vecadd->v0.y, vecadd->v0.z,
						vecadd->v1.x, vecadd->v1.y
					);
			}
		}

		d3dcontext->Unmap(m_VecAddUAV_buffertmp.Get(), 0);
	}
}