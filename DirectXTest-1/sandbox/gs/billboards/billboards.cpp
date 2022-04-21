
#include "utils/pch.hpp"
#include "billboards.hpp"

#include "imgui/imgui.hpp"
#include "DirectXTK/DDSTextureLoader.h"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	GSBillboards::GSBillboards(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_Effects(d3dres->GetD3DDevice(), GetDefaultWolrdConstants()),
		m_BlendRenderState(d3dres)
	{
		InitializeWorldInfo();
		InitializeForD3D();
	}


	void GSBillboards::OnFrame(uint64_t)
	{
		m_Effects.SetWorldEyePosition({ m_CamPosition[0], m_CamPosition[1], m_CamPosition[2] });
		DrawCube();
	}


	void GSBillboards::OnImGuiDraw()
	{
		static bool window_open[2]{};

		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);

		if (update)
			UpdateViewProjection();

		ImGui::Checkbox("Skull", &window_open[0]);
		if (window_open[0])
		{
			if (ImGui::Begin("Skull", &window_open[0]))
			{
				ImGui::DragFloat3("Position", &m_Sphere.World.r[3].m128_f32[0]);
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


	void GSBillboards::InitializeWorldInfo()
	{
		UpdateViewProjection();

		m_Sphere.Material.Ambient = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		m_Sphere.Material.Diffuse = DX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_Sphere.Material.Specular = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 16.0f);
	}


	void GSBillboards::InitializeForD3D()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		m_SphereGeometry.PushMesh(GeometryFactory::CreateGeoSphere(0, 1.f));
		m_SphereGeometry.CreateBuffers(d3ddevice, true);
		m_SphereGeometry.CreateShaders(d3ddevice, L"resources/gs/subdiv/env_vs.cso", L"resources/gs/subdiv/env_ps.cso");

		DX::ComPtr<ID3DBlob> gs_shader;
		DX::ThrowIfFailed(
			D3DReadFileToBlob(L"resources/gs/env_gs.cso", gs_shader.GetAddressOf())
		);

		DX::ThrowIfFailed(
			d3ddevice->CreateGeometryShader(
				gs_shader->GetBufferPointer(),
				gs_shader->GetBufferSize(),
				nullptr,
				m_GSTriangle.GetAddressOf()
			)
		);
	}


	auto GSBillboards::GetDefaultWolrdConstants() ->
		EffectManager::WorldConstantBuffer
	{
		EffectManager::WorldConstantBuffer info{};

		info.World = info.WorldViewProj = info.WorldInvTranspose = DX::XMMatrixIdentity();

		info.Light.Ambient = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		info.Light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Direction = { .147f, -.567f, -.033f };

		return info;
	}
}