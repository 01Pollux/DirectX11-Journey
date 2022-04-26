
#include "utils/pch.hpp"
#include "blur.hpp"

#include "imgui/imgui.hpp"
#include "DirectXTK/DDSTextureLoader.h"

#include <random>
#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	BlurredTextureDemo::BlurredTextureDemo(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_Effects(GetDefaultTexture(d3dres))
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		GeometryInstance::MeshData_t mesh;

		mesh.vertices.emplace_back(
			DX::XMFLOAT3(-1.0f, -1.0f, 0.0f),
			DX::XMFLOAT3(0.f, 0.f, -1.f),
			DX::XMFLOAT2(0.f, 1.f)
		);
		mesh.vertices.emplace_back(
			DX::XMFLOAT3(-1.0f, 1.0f, 0.0f),
			DX::XMFLOAT3(0.f, 0.f, -1.f),
			DX::XMFLOAT2(0.f, 0.f)
		);
		mesh.vertices.emplace_back(
			DX::XMFLOAT3(1.0f, 1.0f, 0.0f),
			DX::XMFLOAT3(0.f, 0.f, -1.f),
			DX::XMFLOAT2(1.f, 0.f)
		);

		mesh.vertices.emplace_back(
			DX::XMFLOAT3(1.0f, -1.0f, 0.0f),
			DX::XMFLOAT3(0.f, 0.f, -1.f),
			DX::XMFLOAT2(1.f, 1.f)
		);

		mesh.indices.push_back(0); mesh.indices.push_back(1); mesh.indices.push_back(2);
		mesh.indices.push_back(0); mesh.indices.push_back(2); mesh.indices.push_back(3);

		m_FullscreenGeometry.PushMesh(std::move(mesh));

		m_FullscreenGeometry.CreateBuffers(d3ddevice, true, true);
		m_FullscreenGeometry.CreateShaders(d3ddevice, L"resources/cs/blur_vs.cso", L"resources/cs/blur_ps.cso");


		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				d3ddevice,
				L"resources/waves_texture/water2.dds",
				nullptr,
				m_SomeTextureView.GetAddressOf()
			)
		);

		{
			D3D11_TEXTURE2D_DESC texture_desc;
			GetDeviceResources()->GetRenderTarget()->GetDesc(&texture_desc);
			texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;

			DX::ComPtr<ID3D11Texture2D> render_tex;
			DX::ThrowIfFailed(
				d3ddevice->CreateTexture2D(
					&texture_desc,
					nullptr,
					render_tex.GetAddressOf()
				)
			);

			D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
			rtv_desc.Format = texture_desc.Format;

			DX::ThrowIfFailed(
				d3ddevice->CreateRenderTargetView(
					render_tex.Get(),
					nullptr,
					m_OffscreenRenderView.GetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateShaderResourceView(
					render_tex.Get(),
					nullptr,
					m_OffscreenTexView.GetAddressOf()
				)
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateUnorderedAccessView(
					render_tex.Get(),
					nullptr,
					m_OffscreenUAVView.GetAddressOf()
				)
			);
		}
	}


	void BlurredTextureDemo::OnFrame(uint64_t)
	{
		GetDeviceResources()->PIXBeginEvent(L"Draw Off screen");
		DrawOffscreen();
		GetDeviceResources()->PIXEndEvent();

		GetDeviceResources()->PIXBeginEvent(L"Draw On screen");
		DrawOnScreen();
		GetDeviceResources()->PIXEndEvent();
	}


	auto BlurredTextureDemo::GetDefaultTexture(DX::DeviceResources* d3dres) ->
		EffectManager::NonNumericConstants
	{
		auto d3ddevice = d3dres->GetD3DDevice();
		EffectManager::NonNumericConstants info;

		CD3D11_SAMPLER_DESC sampler_desc(CD3D11_DEFAULT{});
		sampler_desc.AddressU = sampler_desc.AddressV = sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DX::ThrowIfFailed(
			d3ddevice->CreateSamplerState(
				&sampler_desc,
				info.Sampler.GetAddressOf()
			)
		);

		{
			D3D11_TEXTURE2D_DESC texture_desc;
			d3dres->GetRenderTarget()->GetDesc(&texture_desc);
			texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			texture_desc.Width = 1060;
			texture_desc.Height = 800;

			DX::ComPtr<ID3D11Texture2D> render_tex;
			DX::ThrowIfFailed(
				d3ddevice->CreateTexture2D(
					&texture_desc,
					nullptr,
					render_tex.GetAddressOf()
				)
			);

			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
			srv_desc.Format = texture_desc.Format;
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srv_desc.Texture2D.MostDetailedMip = 0;
			srv_desc.Texture2D.MipLevels = 1;

			DX::ThrowIfFailed(
				d3ddevice->CreateShaderResourceView(
					render_tex.Get(),
					&srv_desc,
					info.CSOutputTexture.GetAddressOf()
				)
			);
			
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
			uav_desc.Format = texture_desc.Format;
			uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uav_desc.Texture2D.MipSlice = 0;

			DX::ThrowIfFailed(
				d3ddevice->CreateUnorderedAccessView(
					render_tex.Get(),
					&uav_desc,
					info.CSInputUAV.GetAddressOf()
				)
			);
		}

		for (auto [shader_path, shader_ptr] : {
				std::pair(L"resources/cs/blur_horz_cs.cso", info.CSHorz.GetAddressOf()),
				std::pair(L"resources/cs/blur_vert_cs.cso", info.CSVert.GetAddressOf())
			})
		{
			DX::ComPtr<ID3DBlob> shader_blob;
			DX::ThrowIfFailed(
				D3DReadFileToBlob(shader_path, shader_blob.GetAddressOf())
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateComputeShader(
					shader_blob->GetBufferPointer(),
					shader_blob->GetBufferSize(),
					nullptr,
					shader_ptr
				)
			);
		}

		return info;
	}


	void BlurredTextureDemo::DrawOffscreen()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();
		auto d3d_rtview = GetDeviceResources()->GetRenderTargetView();
		auto d3d_dsview = GetDeviceResources()->GetDepthStencilView();

		d3dcontext->OMSetRenderTargets(
			1,
			m_OffscreenRenderView.GetAddressOf(),
			d3d_dsview
		);

		d3dcontext->ClearRenderTargetView(
			m_OffscreenRenderView.Get(), DX::Colors::Silver
		);

		d3dcontext->ClearDepthStencilView(
			d3d_dsview,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
		);

		m_FullscreenGeometry.Bind(d3dcontext);
		m_Effects.SetTexture(m_SomeTextureView.Get());

		m_Effects.Bind(d3dcontext);
		m_FullscreenGeometry.Draw(d3dcontext);
		m_Effects.Unbind(d3dcontext);

		d3dcontext->OMSetRenderTargets(
			1,
			&d3d_rtview,
			d3d_dsview
		);
		
		GetDeviceResources()->PIXSetMarker(L"Blur");

		uint32_t size[]{ 1060, 800 };
		m_Effects.BlurInPlace(
			d3dcontext,
			m_OffscreenTexView.Get(),
			m_OffscreenUAVView.Get(),
			size,
			4
		);

		d3dcontext->ClearRenderTargetView(
			d3d_rtview, DX::Colors::Silver
		);

		d3dcontext->ClearDepthStencilView(
			d3d_dsview,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
		);
	}
	
	
	void BlurredTextureDemo::DrawOnScreen()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_FullscreenGeometry.Bind(d3dcontext);
		m_Effects.SetTexture(m_OffscreenTexView.Get());

		m_Effects.Bind(d3dcontext);
		m_FullscreenGeometry.Draw(d3dcontext);
		m_Effects.Unbind(d3dcontext);
	}
}