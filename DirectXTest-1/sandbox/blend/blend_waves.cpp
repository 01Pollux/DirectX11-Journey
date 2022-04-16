
#include "utils/pch.hpp"
#include "blend_waves.hpp"

#include "imgui/imgui.hpp"
#include "DirectXTK/DDSTextureLoader.h"

#include <random>

namespace Pleiades::Sandbox
{
	WavesBlendingDemo::WavesBlendingDemo(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_Effects(d3dres->GetD3DDevice(), GetDefaultTexture(d3dres), GetDefaultWolrdConstants()),
		m_BlendRenderState(d3dres)
	{
		m_WavesMesh.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);

		UpdateViewProjection();
		BuildLandAndWavesMesh();

		m_Land.World = DX::XMMatrixScaling(2.f, 2.f, 2.f) * DX::XMMatrixTranslation(8.0f, 5.0f, 0.f);
		m_Land.Material.Ambient = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		m_Land.Material.Diffuse = DX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_Land.Material.Specular = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 16.0f);

		m_LandGeometry.CreateBuffers(GetDeviceResources()->GetD3DDevice(), true);
		m_LandGeometry.CreateShaders(GetDeviceResources()->GetD3DDevice(), L"resources/blending/env_vs.cso", L"resources/blending/env_ps.cso");


		m_Box.World = DX::XMMatrixScaling(15.0f, 15.0f, 15.0f) * DX::XMMatrixTranslation(-9.0f, 10.0f, 5.0f);
		m_Box.Material.Ambient = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		m_Box.Material.Diffuse = DX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_Box.Material.Specular = DX::XMVectorSet(0.4f, 0.4f, 0.4f, 16.f);

		m_BoxGeometry.CreateBuffers(GetDeviceResources()->GetD3DDevice(), true);
		m_BoxGeometry.CreateShaders(GetDeviceResources()->GetD3DDevice(), L"resources/blending/env_vs.cso", L"resources/blending/env_ps.cso");
		

		m_Waves.World = DX::XMMatrixScaling(10.f, 10.f, 10.f) * DX::XMMatrixTranslation(8.0f, 5.0f, -15.0f);
		m_Waves.Material.Ambient = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		m_Waves.Material.Diffuse = DX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_Waves.Material.Specular = DX::XMVectorSet(0.8f, 0.8f, 0.8f, 32.0f);

		m_WavesGeometry.CreateBuffers(GetDeviceResources()->GetD3DDevice(), true, true);
		m_WavesGeometry.CreateShaders(GetDeviceResources()->GetD3DDevice(), L"resources/blending/env_vs.cso", L"resources/blending/env_ps.cso");


		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				GetDeviceResources()->GetD3DDevice(),
				L"resources/waves_texture/grass.dds",
				nullptr,
				m_LandTexture.GetAddressOf()
			)
		);


		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				GetDeviceResources()->GetD3DDevice(),
				L"resources/blending/WireFence.dds",
				nullptr,
				m_BoxTexture.GetAddressOf()
			)
		);

		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				GetDeviceResources()->GetD3DDevice(),
				L"resources/waves_texture/water2.dds",
				nullptr,
				m_WavesTexture.GetAddressOf()
			)
		);
	}


	void WavesBlendingDemo::OnFrame(uint64_t ticks)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Effects.SetWorldEyePosition({ m_CamPosition[0], m_CamPosition[1], m_CamPosition[2] });

		UpdateWavesMesh(ticks);

		for (auto& [geometry, effect_info, texture, cull_on, blend_on] : {
			std::tuple(&m_LandGeometry, &m_Land, m_LandTexture.Get(), false, false),
			std::tuple(&m_BoxGeometry, &m_Box, m_BoxTexture.Get(), true, true),
			std::tuple(&m_WavesGeometry, &m_Waves, m_WavesTexture.Get(), false, true)
			})
		{
			m_BlendRenderState.SetCullMode(d3dcontext, cull_on);
			m_BlendRenderState.SetAlphaBlend(d3dcontext, blend_on);

			geometry->Bind(d3dcontext);
			m_Effects.SetMaterial(effect_info->Material);
			effect_info->Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
			m_Effects.SetTexture(texture);

			m_Effects.Bind(d3dcontext);
			geometry->Draw(d3dcontext);
		}
	}


	void WavesBlendingDemo::OnImGuiDraw()
	{
		static bool window_open[2]{};

		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);

		if (update)
			UpdateViewProjection();

		ImGui::Checkbox("Fog", &window_open[0]);
		if (window_open[0])
		{
			if (ImGui::Begin("Fog", &window_open[1], ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::ColorEdit4("Color", &m_Effects.Buffer().FogColor.x);
				ImGui::DragFloat("Start", &m_Effects.Buffer().FogStartDist);
				ImGui::DragFloat("End", &m_Effects.Buffer().FogEndDist);
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


	void WavesBlendingDemo::BuildLandAndWavesMesh()
	{
		m_LandGeometry.PushMesh(
			GeometryFactory::CreatePlane(
				50, 50,
				160.f, 160.f
			)
		);
		m_BoxGeometry.PushMesh(
			GeometryFactory::CreateBox(
				2.f, 2.f, 2.f
			)
		);

		GeometryFactory::MeshData_t mesh;

		mesh.vertices.resize(m_WavesMesh.VertexCount());
		mesh.indices.resize(m_WavesMesh.TriangleCount() * 3);

		size_t k = 0;
		size_t columns = m_WavesMesh.ColumnCount();
		size_t rows = m_WavesMesh.RowCount();

		for (size_t i = 0; i < rows - 1; ++i)
		{
			for (size_t j = 0; j < columns - 1; ++j)
			{
				mesh.indices[k] = static_cast<uint16_t>(i * columns + j);
				mesh.indices[k + 1] = static_cast<uint16_t>(i * columns + j + 1);
				mesh.indices[k + 2] = static_cast<uint16_t>((i + 1) * columns + j);

				mesh.indices[k + 3] = static_cast<uint16_t>((i + 1) * columns + j);
				mesh.indices[k + 4] = static_cast<uint16_t>(i * columns + j + 1);
				mesh.indices[k + 5] = static_cast<uint16_t>((i + 1) * columns + j + 1);

				k += 6;
			}
		}

		m_WavesGeometry.PushMesh(
			std::move(mesh)
		);


		for (auto& plane_vertex : m_LandGeometry.Mesh.vertices)
		{
			plane_vertex.position.y =
				.3f * (
					plane_vertex.position.z * std::sinf(plane_vertex.position.x * .1f)
					+ plane_vertex.position.x * std::cosf(plane_vertex.position.z * .1f)
					);

			DX::XMFLOAT3 n(
				-.03f * plane_vertex.position.z * cosf(.1f * plane_vertex.position.x) - .3f * cosf(.1f * plane_vertex.position.z),
				1.0f,
				-.3f * sinf(0.1f * plane_vertex.position.x) + 0.03f * plane_vertex.position.x * sinf(0.1f * plane_vertex.position.z)
			);

			DX::XMStoreFloat3(&plane_vertex.normal, DX::XMVector3Normalize(DX::XMLoadFloat3(&n)));
		}
	}


	void WavesBlendingDemo::UpdateWavesMesh(uint64_t ticks)
	{
		static float elapsed = 0;
		float dt = static_cast<float>(StepTimer::TicksToSeconds(ticks));
		if (elapsed > 0.25f)
		{
			elapsed = 0.f;

			std::random_device rdevice;

			uint32_t i = 5 + std::uniform_int_distribution<uint32_t>(0, m_WavesMesh.RowCount() - 10)(rdevice);
			uint32_t j = 5 + std::uniform_int_distribution<uint32_t>(0, m_WavesMesh.ColumnCount() - 10)(rdevice);

			float r = std::uniform_real_distribution<float>(1.f, 2.f)(rdevice);

			m_WavesMesh.Disturb(i, j, r);
		}

		elapsed += dt;

		m_WavesMesh.Update(dt);


		D3D11_MAPPED_SUBRESOURCE vtx_data;
		DX::ThrowIfFailed(
			GetDeviceResources()->GetD3DDeviceContext()->Map(
				m_WavesGeometry.d3dVerticies.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&vtx_data
			)
		);


		auto vtx_mapped = std::bit_cast<GeometryFactory::MeshData_t::verticies_type*>(vtx_data.pData);
		for (UINT i = 0; i < m_WavesMesh.VertexCount(); ++i)
		{
			vtx_mapped[i].position = m_WavesMesh[i];
			vtx_mapped[i].normal = m_WavesMesh.Normal(i);

			// Derive tex-coords in [0,1] from position.
			vtx_mapped[i].textureCoordinate.x = 0.5f + m_WavesMesh[i].x / m_WavesMesh.Width();
			vtx_mapped[i].textureCoordinate.y = 0.5f - m_WavesMesh[i].z / m_WavesMesh.Depth();
		}


		GetDeviceResources()->GetD3DDeviceContext()->Unmap(
			m_WavesGeometry.d3dVerticies.Get(),
			0
		);
	}


	auto WavesBlendingDemo::GetDefaultWolrdConstants() ->
		EffectManager::WorldConstantBuffer
	{
		EffectManager::WorldConstantBuffer info{};

		info.World = info.WorldViewProj = info.WorldInvTranspose = DX::XMMatrixIdentity();

		info.Light.Ambient = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		info.Light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Direction = { .57735f, -.57735f, .57735f };

		DX::XMStoreFloat4(&info.FogColor, DX::Colors::Silver);
		info.FogEndDist = 326.f;
		info.FogStartDist = 261.f;

		return info;
	}


	auto WavesBlendingDemo::GetDefaultTexture(DX::DeviceResources* d3dres) ->
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

		return info;
	}
}