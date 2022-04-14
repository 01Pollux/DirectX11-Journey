
#include "utils/pch.hpp"
#include "box_texture.hpp"

#include "imgui/imgui.hpp"
#include "DirectXTK/DDSTextureLoader.h"

namespace Pleiades::Sandbox
{
	MultipleTexturedBox ::MultipleTexturedBox (DX::DeviceResources* d3dres) : 
		ISandbox(d3dres),
		m_Effects(d3dres->GetD3DDevice(), GetDefaultTexture(d3dres), GetDefaultWolrdConstants())
	{
		UpdateViewProjection();
		BuildBoxMesh();

		m_Box.Material.Ambient = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		m_Box.Material.Diffuse = DX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_Box.Material.Specular = DX::XMVectorSet(0.6f, 0.6f, 0.6f, 16.0f);

		m_BoxGeometry->CreateBuffers(GetDeviceResources()->GetD3DDevice(), true);
		m_BoxGeometry->CreateShaders(GetDeviceResources()->GetD3DDevice(), L"resources/box_texture/box_multiple_vs.cso", L"resources/box_texture/box_multiple_ps.cso");
	}


	void MultipleTexturedBox ::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();
		
		m_Effects.SetWorldEyePosition({ m_CamPosition[0], m_CamPosition[1], m_CamPosition[2] });
		m_Effects.Bind(d3dcontext);

		m_BoxGeometry->Bind(d3dcontext);
		m_Effects.SetMaterial(m_Box.Material);
		m_Box.Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
		m_BoxGeometry->Draw(d3dcontext);
	}


	void MultipleTexturedBox ::OnImGuiDraw()
	{
		if (ImGui::Button("Update"))
		{
			BuildBoxMesh();
			m_BoxGeometry->CreateBuffers(GetDeviceResources()->GetD3DDevice(), true);
		}

		static bool window_open[2]{};

		ImGui::DragFloat3("Draw size", m_BoxSize);
		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);

		if (update)
			UpdateViewProjection();

		ImGui::Checkbox("Box", &window_open[0]);
		if (window_open[0])
		{
			if (ImGui::Begin("Box", &window_open[0], ImGuiWindowFlags_AlwaysAutoResize))
			{
				for (auto [name, vec_color] : {
					std::pair{ "Ambient", &m_Box.Material.Ambient },
					std::pair{ "Diffuse", &m_Box.Material.Diffuse }
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

				float color[4]{
						DX::XMVectorGetX(m_Box.Material.Specular),
						DX::XMVectorGetY(m_Box.Material.Specular),
						DX::XMVectorGetZ(m_Box.Material.Specular),
						DX::XMVectorGetW(m_Box.Material.Specular)
				};
				if (ImGui::DragFloat4("Specular", color))
					m_Box.Material.Specular = DX::XMVectorSet(color[0], color[1], color[2], color[3]);

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


	void MultipleTexturedBox ::BuildBoxMesh()
	{
		GeometryFactory::CreateBox(
			m_BoxGeometry,
			m_BoxSize[0],
			m_BoxSize[1],
			m_BoxSize[2]
		);
	}


	auto MultipleTexturedBox ::GetDefaultWolrdConstants() ->
		EffectManager::WorldConstantBuffer
	{
		EffectManager::WorldConstantBuffer info{};

		info.World = info.WorldViewProj = info.WorldInvTranspose = DX::XMMatrixIdentity();

		info.Light.Ambient = DX::XMVectorSet(1.f, 1.f, 1.f, 1.0f);
		info.Light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Direction = { .57735f, -.57735f, .57735f };

		return info;
	}
	

	auto MultipleTexturedBox ::GetDefaultTexture(DX::DeviceResources* d3dres) ->
		EffectManager::NonNumericConstants
	{
		auto d3ddevice = d3dres->GetD3DDevice();
		EffectManager::NonNumericConstants info;


		const wchar_t* texture_paths[]{
			L"resources/box_texture/flare.dds",
			L"resources/box_texture/flarealpha.dds"
		};

		for (size_t i = 0; i < 2; i++)
		{
			DX::ComPtr<ID3D11Texture2D> cur_texture;
			DX::ThrowIfFailed(
				DX::CreateDDSTextureFromFile(
					d3ddevice,
					texture_paths[i],
					nullptr,
					info.Texture[i].GetAddressOf()
				)
			);
		}

		CD3D11_SAMPLER_DESC sampler_desc(CD3D11_DEFAULT{});
		sampler_desc.AddressU = sampler_desc.AddressV = sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		DX::ThrowIfFailed(
			d3ddevice->CreateSamplerState(
				&sampler_desc,
				info.Sampler.GetAddressOf()
			)
		);

		return info;
	}
}