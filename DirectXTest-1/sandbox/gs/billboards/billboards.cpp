
#include "utils/pch.hpp"
#include "billboards.hpp"

#include "imgui/imgui.hpp"
#include "DirectXTK/DDSTextureLoader.h"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	GSBillboardsDemo::GSBillboardsDemo(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_Effects(d3dres->GetD3DDevice(), GetDefaultTextures(d3dres), GetDefaultWolrdConstants()),
		m_BlendRenderState(d3dres)
	{
		InitializeWorldInfo();
		InitializeForD3D();
	}


	void GSBillboardsDemo::OnFrame(uint64_t)
	{
		m_Effects.SetWorldEyePosition({ m_CamPosition[0], m_CamPosition[1], m_CamPosition[2] });
		DrawBillboard();
	}


	void GSBillboardsDemo::OnImGuiDraw()
	{
		static bool window_open[2]{};

		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);
		ImGui::Checkbox("Alpha to coverage", &m_AlphaToCoverage);

		if (update)
			UpdateViewProjection();

		ImGui::Checkbox("Trees", &window_open[0]);
		if (window_open[0])
		{
			if (ImGui::Begin("Trees", &window_open[0]))
			{
				for (auto& pt : m_PointBilloards)
				{
					ImGui::PushID(static_cast<const void*>(&pt));
					ImGui::DragFloat3("Position", &pt.World.r[3].m128_f32[0]);
					ImGui::PopID();
				}
			}
			ImGui::End();
		}

		ImGui::Checkbox("Light", &window_open[1]);
		if (!window_open[1])
			return;

		if (ImGui::Begin("Light", &window_open[1], ImGuiWindowFlags_AlwaysAutoResize))
		{
			for (auto [name, vec_color] : {
					std::pair{ "Ambient", &m_Effects.Buffer().Light.Ambient },
					std::pair{ "Diffuse", &m_Effects.Buffer().Light.Diffuse }
				})
			{
				float color[4]{
					DX::XMVectorGetX(*vec_color),
					DX::XMVectorGetY(*vec_color),
					DX::XMVectorGetZ(*vec_color),
					DX::XMVectorGetW(*vec_color)
				};
				if (ImGui::ColorEdit4(name, color))
					*vec_color = DX::XMVectorSet(color[0], color[1], color[2], color[3]);
			}

			ImGui::DragFloat3("Direction", &m_Effects.Buffer().Light.Direction.x);

			float color[4]{
					DX::XMVectorGetX(m_Effects.Buffer().Light.Specular),
					DX::XMVectorGetY(m_Effects.Buffer().Light.Specular),
					DX::XMVectorGetZ(m_Effects.Buffer().Light.Specular),
					DX::XMVectorGetW(m_Effects.Buffer().Light.Specular)
			};
			if (ImGui::DragFloat4("Specular", color))
				m_Effects.Buffer().Light.Specular = DX::XMVectorSet(color[0], color[1], color[2], color[3]);
		}

		ImGui::End();
	}


	void GSBillboardsDemo::InitializeWorldInfo()
	{
		UpdateViewProjection();

		float z_offset = 0.f, x_offset = 0.f;
		bool swap = false;

		for (auto& pt : m_PointBilloards)
		{
			pt.World = DX::XMMatrixTranslation(x_offset, 0.f, z_offset);
			pt.Material.Ambient = DX::XMVectorSet(1.f, 1.f, 1.f, 1.f);
			pt.Material.Diffuse = DX::XMVectorSet(1.f, 1.f, 1.f, 1.f);
			pt.Material.Specular = DX::XMVectorSet(.2f, .2f, .2f, 16.f);

			x_offset += 3.f;
			z_offset += 2.f;

			if (swap)
				x_offset *= -1.f;

			swap = !swap;
		}
	}


	void GSBillboardsDemo::InitializeForD3D()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		{
			MeshData_t mesh[]
			{
				{
					DX::XMFLOAT3(0.f, 0.f, 0.f),
					DX::XMFLOAT2(2.f, 4.f)
				}
			};
			// Create vertex buffer
			DX::ThrowIfFailed(
				DX::CreateStaticBuffer(
					d3ddevice,
					static_cast<const void*>(mesh),
					std::ssize(mesh),
					sizeof(MeshData_t),
					D3D11_BIND_VERTEX_BUFFER,
					m_PointBillboardGeometry.d3dVerticies.ReleaseAndGetAddressOf()
				)
			);
		}

		{
			DX::ComPtr<ID3DBlob> shader_blob;

			// create vertex shader + input layout
			{
				const D3D11_INPUT_ELEMENT_DESC input_layout[]{
					{ .SemanticName = "Position", .Format = DXGI_FORMAT_R32G32B32_FLOAT, .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT },
					{ .SemanticName = "Size", .Format = DXGI_FORMAT_R32G32_FLOAT, .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT }
				};

				DX::ThrowIfFailed(
					D3DReadFileToBlob(
						L"resources/gs/billboards/env_vs.cso",
						shader_blob.GetAddressOf()
					)
				);

				DX::ThrowIfFailed(
					d3ddevice->CreateVertexShader(
						shader_blob->GetBufferPointer(),
						shader_blob->GetBufferSize(),
						nullptr,
						m_PointBillboardGeometry.d3dVtxShader.GetAddressOf()
					)
				);

				DX::ThrowIfFailed(
					d3ddevice->CreateInputLayout(
						input_layout,
						static_cast<uint32_t>(std::size(input_layout)),
						shader_blob->GetBufferPointer(),
						shader_blob->GetBufferSize(),
						m_PointBillboardGeometry.d3dInputLayout.GetAddressOf()
					)
				);
			}

			// create pixel shader
			{
				DX::ThrowIfFailed(
					D3DReadFileToBlob(
						L"resources/gs/billboards/env_ps.cso",
						shader_blob.ReleaseAndGetAddressOf()
					)
				);

				DX::ThrowIfFailed(
					d3ddevice->CreatePixelShader(
						shader_blob->GetBufferPointer(),
						shader_blob->GetBufferSize(),
						nullptr,
						m_PointBillboardGeometry.d3dPxlShader.GetAddressOf()
					)
				);
			}

			// create geometry shader
			{
				DX::ThrowIfFailed(
					D3DReadFileToBlob(
						L"resources/gs/billboards/env_gs.cso",
						shader_blob.ReleaseAndGetAddressOf()
					)
				);

				DX::ThrowIfFailed(
					d3ddevice->CreateGeometryShader(
						shader_blob->GetBufferPointer(),
						shader_blob->GetBufferSize(),
						nullptr,
						m_d3dPointBillboardGS.GetAddressOf()
					)
				);
			}
		}
	}


	auto GSBillboardsDemo::GetDefaultTextures(DX::DeviceResources* d3dres) ->
		EffectManager::NonNumericBuffer
	{
		auto d3ddevice = d3dres->GetD3DDevice();
		auto d3dcontext = d3dres->GetD3DDeviceContext();

		EffectManager::NonNumericBuffer info;

		{
			DX::ComPtr<ID3D11Texture2D> tree_textures[NumTextures], texture_arr;

			for (auto tree_texture = tree_textures; auto texture_path : {
					L"resources/gs/billboards/tree2.dds",
					L"resources/gs/billboards/tree1.dds",
					L"resources/gs/billboards/tree2.dds",
					L"resources/gs/billboards/tree3.dds",
				})
			{
				DX::ThrowIfFailed(
					DX::CreateDDSTextureFromFileEx(
						d3ddevice,
						texture_path,
						0,
						D3D11_USAGE_STAGING,
						0,
						D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE,
						0,
						false,
						reinterpret_cast<ID3D11Resource**>(tree_texture->GetAddressOf()),
						nullptr
					)
				);
				++tree_texture;
			}
			

			D3D11_TEXTURE2D_DESC texture_desc;
			tree_textures[0]->GetDesc(&texture_desc);

			texture_desc.ArraySize = NumTextures;
			texture_desc.Usage = D3D11_USAGE_DEFAULT;
			texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			DX::ThrowIfFailed(
				d3ddevice->CreateTexture2D(
					&texture_desc,
					nullptr,
					texture_arr.GetAddressOf()
				)
			);

			for (uint32_t tex = 0; tex < NumTextures; tex++)
			{
				for (uint32_t mip = 0; mip < texture_desc.MipLevels; mip++)
				{
					D3D11_MAPPED_SUBRESOURCE texture_data;
					DX::ThrowIfFailed(
						d3dcontext->Map(
							tree_textures[tex].Get(),
							mip,
							D3D11_MAP_READ,
							0,
							&texture_data
						)
					);

					d3dcontext->UpdateSubresource(
						texture_arr.Get(),
						D3D11CalcSubresource(
							mip,
							tex,
							texture_desc.MipLevels
						),
						nullptr,
						texture_data.pData,
						texture_data.RowPitch,
						texture_data.DepthPitch
					);

					d3dcontext->Unmap(tree_textures[tex].Get(), mip);
				}
			}

			CD3D11_SHADER_RESOURCE_VIEW_DESC shader_desc(
				D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
				texture_desc.Format,
				0,
				texture_desc.MipLevels,
				0,
				NumTextures
			);

			DX::ThrowIfFailed(
				d3ddevice->CreateShaderResourceView(
					texture_arr.Get(),
					&shader_desc,
					info.Textures.GetAddressOf()
				)
			);
		}



		CD3D11_SAMPLER_DESC sampler_desc(D3D11_DEFAULT);
		DX::ThrowIfFailed(
			d3ddevice->CreateSamplerState(
				&sampler_desc,
				info.Sampler.GetAddressOf()
			)
		);

		return info;
	}


	auto GSBillboardsDemo::GetDefaultWolrdConstants() ->
		EffectManager::WorldConstantBuffer
	{
		EffectManager::WorldConstantBuffer info{};

		info.World = info.ViewProj = DX::XMMatrixIdentity();

		info.Light.Ambient = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		info.Light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Direction = { .147f, -.567f, -.033f };

		return info;
	}
}