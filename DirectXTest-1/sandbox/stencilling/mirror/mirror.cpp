
#include "utils/pch.hpp"
#include "mirror.hpp"

#include "imgui/imgui.hpp"
#include "DirectXTK/DDSTextureLoader.h"

#include <random>

namespace Pleiades::Sandbox
{
	MirrorSkullWorld::MirrorSkullWorld(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_Effects(d3dres->GetD3DDevice(), GetDefaultTexture(d3dres), GetDefaultWolrdConstants()),
		m_BlendRenderState(d3dres)
	{
		InitializeWorldInfo();

		InitializeForD3D();
	}


	void MirrorSkullWorld::OnFrame(uint64_t ticks)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Effects.SetWorldEyePosition({ m_CamPosition[0], m_CamPosition[1], m_CamPosition[2] });

	}


	void MirrorSkullWorld::OnImGuiDraw()
	{
		static bool window_open[2]{};

		bool update = ImGui::DragFloat3("Position", m_CamPosition);
		update |= ImGui::DragFloat3("Rotation", m_CamRotation);

		if (update)
			UpdateViewProjection();

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


	void MirrorSkullWorld::InitializeWorldInfo()
	{
		UpdateViewProjection();

		m_Wall.World = DX::XMMatrixScaling(2.f, 2.f, 2.f) * DX::XMMatrixTranslation(8.0f, 5.0f, 0.f);
		m_Wall.Material.Ambient = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		m_Wall.Material.Diffuse = DX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_Wall.Material.Specular = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 16.0f);

	}


	void MirrorSkullWorld::InitializeForD3D()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		m_WallGeometry.PushMesh(CreateWalls());
		m_WallGeometry.CreateBuffers(d3ddevice, true);
		m_WallGeometry.CreateShaders(d3ddevice, L"resources/blending/env_vs.cso", L"resources/blending/env_ps.cso");

		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				d3ddevice,
				L"resources/stecilling/brick01.dds",
				nullptr,
				m_WallTexture.GetAddressOf()
			)
		);


		m_SkullGeometry.PushMesh(GeometryFactory::CreateFromTxt("sandbox/from_file/skull.txt"));
		m_SkullGeometry.CreateBuffers(d3ddevice, true);
		m_SkullGeometry.CreateShaders(d3ddevice, L"resources/blending/env_vs.cso", L"resources/blending/env_ps.cso");

		m_SkullTexture = nullptr;


		m_MirrorGeometry.PushMesh(CreateMirror());
		m_SkullGeometry.CreateBuffers(d3ddevice, true);
		m_SkullGeometry.CreateShaders(d3ddevice, L"resources/blending/env_vs.cso", L"resources/blending/env_ps.cso");

		DX::ThrowIfFailed(
			DX::CreateDDSTextureFromFile(
				d3ddevice,
				L"resources/waves_texture/water2.dds",
				nullptr,
				m_MirrorTexture.GetAddressOf()
			)
		);
	}


	
	GeometryInstance::MeshData_t MirrorSkullWorld::CreateWalls()
	{
		// Create and specify geometry.  For this sample we draw a floor
		// and a wall with a mirror on it.  We put the floor, wall, and
		// mirror geometry in one vertex buffer.
		//
		//   |--------------|
		//   |              |
		//   |----|----|----|
		//   |Wall|Mirr|Wall|
		//   |    | or |    |
		//   /--------------/
		//  /   Floor      /
		// /--------------/

		GeometryInstance::MeshData_t v;

		v.vertices.reserve(24);

		// Floor: Observe we tile texture coordinates.
		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 0.0f, -10.0f), DX::XMFLOAT3(0.0f, 1.0f, 0.0f), DX::XMFLOAT2(0.0f, 4.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 1.0f, 0.0f), DX::XMFLOAT2(0.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 1.0f, 0.0f), DX::XMFLOAT2(4.0f, 0.0f));

		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 0.0f, -10.0f), DX::XMFLOAT3(0.0f, 1.0f, 0.0f), DX::XMFLOAT2(0.0f, 4.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 1.0f, 0.0f), DX::XMFLOAT2(4.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 0.0f, -10.0f), DX::XMFLOAT3(0.0f, 1.0f, 0.0f), DX::XMFLOAT2(4.0f, 4.0f));

		// Wall: Observe we tile texture coordinates, and that we
		// leave a gap in the middle for the mirror.
		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 2.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(-2.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.5f, 0.0f));

		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 2.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(-2.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.5f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(-2.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.5f, 2.0f));

		v.vertices.emplace_back(DX::XMFLOAT3(2.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 2.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(2.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(2.0f, 0.0f));

		v.vertices.emplace_back(DX::XMFLOAT3(2.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 2.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(2.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(2.0f, 2.0f));

		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 1.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 6.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 6.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(6.0f, 0.0f));

		v.vertices.emplace_back(DX::XMFLOAT3(-3.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 1.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 6.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(6.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(7.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(6.0f, 1.0f));

		// Lazy man of using index buffer
		for (size_t i = 0; i < v.vertices.size(); i++)
			v.indices.push_back(static_cast<uint16_t>(i));

		return v;
	}


	GeometryInstance::MeshData_t MirrorSkullWorld::CreateMirror()
	{
		// Create and specify geometry.  For this sample we draw a floor
		// and a wall with a mirror on it.  We put the floor, wall, and
		// mirror geometry in one vertex buffer.
		//
		//   |--------------|
		//   |              |
		//   |----|----|----|
		//   |Wall|Mirr|Wall|
		//   |    | or |    |
		//   /--------------/
		//  /   Floor      /
		// /--------------/

		GeometryInstance::MeshData_t v;

		v.vertices.reserve(6);

		v.vertices.emplace_back(DX::XMFLOAT3(-2.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 1.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(-2.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(2.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(1.0f, 0.0f));

		v.vertices.emplace_back(DX::XMFLOAT3(-2.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(0.0f, 1.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(2.5f, 4.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(1.0f, 0.0f));
		v.vertices.emplace_back(DX::XMFLOAT3(2.5f, 0.0f, 0.0f), DX::XMFLOAT3(0.0f, 0.0f, -1.0f), DX::XMFLOAT2(1.0f, 1.0f));

		// Lazy man of using index buffer
		for (size_t i = 0; i < v.vertices.size(); i++)
			v.indices.push_back(static_cast<uint16_t>(i));

		return v;
	}

	auto MirrorSkullWorld::GetDefaultWolrdConstants() ->
		EffectManager::WorldConstantBuffer
	{
		EffectManager::WorldConstantBuffer info{};

		info.World = info.WorldViewProj = info.WorldInvTranspose = DX::XMMatrixIdentity();

		info.Light.Ambient = DX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		info.Light.Diffuse = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Specular = DX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Light.Direction = { .57735f, -.57735f, .57735f };

		return info;
	}


	auto MirrorSkullWorld::GetDefaultTexture(DX::DeviceResources* d3dres) ->
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