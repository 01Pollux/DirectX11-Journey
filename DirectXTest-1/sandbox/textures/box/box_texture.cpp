
#include "utils/pch.hpp"
#include "box_texture.hpp"

#include "imgui/imgui.hpp"
#include "DirectXTK/DDSTextureLoader.h"

namespace Pleiades::Sandbox
{
	TexturedBox::TexturedBox(DX::DeviceResources* d3dres) : 
		ISandbox(d3dres),
		m_Effects(d3dres->GetD3DDevice(), GetDefaultTexture(d3dres), GetDefaultWolrdConstants())
	{
		UpdateViewProjection();
		BuildBoxMesh();

		m_BoxGeometry->CreateBuffers(GetDeviceResources()->GetD3DDevice(), true);
		m_BoxGeometry->CreateShaders(GetDeviceResources()->GetD3DDevice(), L"resources/box_texture/box_vs.cso", L"resources/box_texture/box_ps.cso");
	}


	void TexturedBox::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();
		
		m_Effects.Bind(d3dcontext);

		m_BoxGeometry->Bind(d3dcontext);
		m_Effects.SetMaterial(m_Box.Material);
		m_Box.Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
		m_BoxGeometry->Draw(d3dcontext);
	}


	void TexturedBox::OnImGuiDraw()
	{
		if (ImGui::Button("Update"))
		{
			BuildBoxMesh();
			m_BoxGeometry->CreateBuffers(GetDeviceResources()->GetD3DDevice());
		}

		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);

		if (update)
			UpdateViewProjection();

		ImGui::DragFloat3("Draw size", m_BoxSize);
	}


	void TexturedBox::BuildBoxMesh()
	{
		GeometryFactory::CreateBox(
			m_BoxGeometry,
			m_BoxSize[0],
			m_BoxSize[1],
			m_BoxSize[2]
		);
	}


	auto TexturedBox::GetDefaultWolrdConstants() ->
		EffectManager::WorldConstantBuffer
	{
		EffectManager::WorldConstantBuffer info{};

		info.Light.Ambient = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		info.Light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Direction = { .57735f, -.57735f, .57735f };


		return info;
	}
	

	auto TexturedBox::GetDefaultTexture(DX::DeviceResources* d3dres) ->
		EffectManager::NonNumericConstants
	{
		auto d3ddevice = d3dres->GetD3DDevice();
		EffectManager::NonNumericConstants info;

		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				d3ddevice,
				L"resources/box_texture/WoodCrate01.dds",
				nullptr,
				info.Texture.GetAddressOf()
			)
		);

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