
#include "utils/pch.hpp"
#include "rand.hpp"

#include <random>
#include <fstream>
#include <format>

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	RandCSLength::RandCSLength(DX::DeviceResources* d3dres) : 
		ISandbox(d3dres)
	{
		InitializeBuffers();
	}


	void RandCSLength::OnFrame(uint64_t)
	{
		if (m_Processed)
			return;
		m_Processed = true;

		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		d3dcontext->CSSetShader(m_VecLengthCS.Get(), nullptr, 0);

		d3dcontext->CSSetShaderResources(
			0, 1, m_VecInView.GetAddressOf()
		);
		d3dcontext->CSSetUnorderedAccessViews(
			0, 1, m_VecOutView.GetAddressOf(), nullptr
		);

		d3dcontext->Dispatch(1, 1, 1);

		d3dcontext->CSSetShader(nullptr, nullptr, 0);

		d3dcontext->CopyResource(m_VecCPU.Get(), m_VecOutBuf.Get());

		D3D11_MAPPED_SUBRESOURCE subres;
		DX::ThrowIfFailed(
			d3dcontext->Map(
				m_VecCPU.Get(),
				0,
				D3D11_MAP_READ,
				0,
				&subres
			)
		);

		std::ofstream out_file("resources/cs/rand.txt");

		float* data = std::bit_cast<float*>(subres.pData);
		for (size_t i = 0; i < 64; i++, data++)
		{
			out_file <<
				std::format(
					"[ {:.2f} ]\n",
					*data
				);
		}

		d3dcontext->Unmap(m_VecCPU.Get(), 0);
	}


	std::vector<std::array<float, 3>> RandCSLength::RandomValues()
	{
		std::mt19937_64 eng;
		std::uniform_real_distribution<float> rand_fl(0.f, 10.f);

		std::vector<std::array<float, 3>> a;
		a.reserve(64);

		for (size_t i = 0; i < 64; i++)
		{
			a.push_back(
				{ 
					rand_fl(eng),
					rand_fl(eng),
					rand_fl(eng) 
				}
			);
		}

		std::ofstream file("resources/cs/rand_ex.txt");

		for (uint32_t i = 0; i < 64; i++)
		{
			file <<
				std::format(
					"[ <{:.2f}, {:.2f}, {:.2f}> ]\n",
					a[i][0], a[i][1], a[i][2]
				);
		}


		return a;
	}


	void RandCSLength::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		// create compute shader
		{
			DX::ComPtr<ID3DBlob> shader_blob;

			DX::ThrowIfFailed(
				D3DReadFileToBlob(
					L"resources/cs/ex1/typed.cso",
					shader_blob.GetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateComputeShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					m_VecLengthCS.GetAddressOf()
				)
			);
		}

		// Create in buffer + views to them
		D3D11_BUFFER_DESC buffer_desc{};
		{
			auto values = RandomValues();

			buffer_desc.StructureByteStride = sizeof(values[0]);
			buffer_desc.ByteWidth = static_cast<uint32_t>(values.size() * buffer_desc.StructureByteStride);
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			buffer_desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA buffer_data{};
			buffer_data.pSysMem = values.data();

			DX::ComPtr<ID3D11Buffer> buffer;
			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					&buffer_data,
					buffer.GetAddressOf()
				)
			);

			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
			srv_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srv_desc.BufferEx.NumElements = 64;


			DX::ThrowIfFailed(
				d3ddevice->CreateShaderResourceView(
					buffer.Get(),
					&srv_desc,
					m_VecInView.GetAddressOf()
				)
			);
		}

		// Create out buffer + views to them
		{
			buffer_desc.StructureByteStride /= 3;
			buffer_desc.ByteWidth /= 3;

			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					nullptr,
					m_VecOutBuf.GetAddressOf()
				)
			);

			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
			uav_desc.Format = DXGI_FORMAT_R32_FLOAT;
			uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav_desc.Buffer.NumElements = 64;

			DX::ThrowIfFailed(
				d3ddevice->CreateUnorderedAccessView(
					m_VecOutBuf.Get(),
					&uav_desc,
					m_VecOutView.GetAddressOf()
				)
			);
		}

		// create temp buffer from cpu to gpu
		{
			buffer_desc.BindFlags = 0;
			buffer_desc.Usage = D3D11_USAGE_STAGING;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			buffer_desc.MiscFlags = 0;

			DX::ThrowIfFailed(
				d3ddevice->CreateBuffer(
					&buffer_desc,
					nullptr,
					m_VecCPU.GetAddressOf()
				)
			);
		}
	}
}