#include "utils/pch.hpp"
#include "light.hpp"

#include "imgui/imgui.hpp"

namespace Pleiades::Sandbox
{
	LightDemo::LightDemo(DX::DeviceResources* d3dres) :
		ISandbox(d3dres),
		m_LandMat{
			.Ambient = { 0.48f, 0.77f, 0.46f, 1.0f },
			.Diffuse = { 0.48f, 0.77f, 0.46f, 1.0f },
			.Specular = { 0.2f, 0.2f, 0.2f, 16.0f },
		}
	{
		m_LightConstantBuffer.DirLight = {
			.Ambient = { 0.2f, 0.2f, 0.2f, 1.0f },
			.Diffuse = { 0.5f, 0.5f, 0.5f, 1.0f },
			.Specular = { 0.5f, 0.5f, 0.5f, 1.0f },
			.Direction = { 0.57735f, -0.57735f, 0.57735f }
		};

		m_LightConstantBuffer.PtLight = {
			.Ambient = { 0.2f, 0.2f, 0.2f, 1.0f },
			.Diffuse = { 0.5f, 0.5f, 0.5f, 1.0f },
			.Specular = { 0.5f, 0.5f, 0.5f, 1.0f },
			.Range = 25.f,
			.Attenuation = { 0.f, 1.f, 0.f }
		};

		m_LightConstantBuffer.SpLight = {
			.Ambient = { 0.2f, 0.2f, 0.2f, 1.0f },
			.Diffuse = { 0.5f, 0.5f, 0.5f, 1.0f },
			.Specular = { 0.5f, 0.5f, 0.5f, 1.0f },
			.Range = 10'000.f,
			.Direction = {0.57735f, -0.57735f, 0.57735f},
			.ExpFactor = 96.f,
			.Attenuation = { 1.0f, 0.0f, 0.0f },
		};

		m_LightConstantBuffer.WorldEyePosition = { -75.f, 25.f, -60.f };
		m_LightConstantBuffer.LightType = LightType::Directional;


		m_WorldConstantBuffer.WorldInvTranspose =
			m_WorldConstantBuffer.WorldViewProj = DX::XMMatrixIdentity();
		m_WorldConstantBuffer.Material = m_LandMat;

		m_WorldConstantBuffer.World = DirectX::XMMatrixTranspose(
			DX::XMMatrixRotationRollPitchYaw(
				m_RotationOffset[0],
				m_RotationOffset[1],
				m_RotationOffset[2]
			) *
			DX::XMMatrixTranslation(
				m_DrawOffset[0],
				m_DrawOffset[1],
				m_DrawOffset[2]
			) *
			DirectX::XMMatrixPerspectiveLH(
				0.25f * 3.14f, GetDeviceResources()->GetAspectRatio(), 1.f, 1000.f
			)
		);

		BuildMeshes();
		InitializeBuffers();
	}


	void LightDemo::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		m_Land->Bind(d3dcontext);
		UpdateScene();
		m_Land->Draw(d3dcontext);
	}
	
	
	void LightDemo::OnImGuiDraw()
	{
		RenderWorldEdit();
		RenderLightsEdit();
	}
	
	
	void LightDemo::BuildMeshes()
	{
		GeometryFactory::CreatePlane(
			m_Land,
			50, 50,
			160.f, 160.f
		);

		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		m_Land->CreateBuffers(d3ddevice, true);
		m_Land->CreateShaders(d3ddevice, L"resources/lights/light_test_vs.cso", L"resources/lights/light_test_ps.cso");
	}

	void LightDemo::InitializeBuffers()
	{
		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		D3D11_BUFFER_DESC buffer_desc{};

		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.ByteWidth = sizeof(m_LightConstantBuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subres_data{};
		subres_data.pSysMem = &m_LightConstantBuffer;
		
		DX::ThrowIfFailed(
			d3ddevice->CreateBuffer(
				&buffer_desc,
				&subres_data,
				m_d3dLightConstantBuffer.GetAddressOf()
			)
		);


		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.ByteWidth = sizeof(m_WorldConstantBuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;

		subres_data.pSysMem = &m_WorldConstantBuffer;

		DX::ThrowIfFailed(
			d3ddevice->CreateBuffer(
				&buffer_desc,
				&subres_data,
				m_d3dWorldConstantBuffer.GetAddressOf()
			)
		);
	}

	void LightDemo::UpdateScene()
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		ID3D11Buffer* buffers[]{ m_d3dWorldConstantBuffer.Get(), m_d3dLightConstantBuffer.Get() };
		d3dcontext->VSSetConstantBuffers(
			0, static_cast<uint32_t>(std::size(buffers)), buffers
		);

		d3dcontext->PSSetConstantBuffers(
			0, static_cast<uint32_t>(std::size(buffers)), buffers
		);
	}


	void LightDemo::RenderWorldEdit()
	{
		ImGui::PushID("World");

		ImGui::Text("World constants");
		bool update = ImGui::DragFloat3("Draw offset", m_DrawOffset);
		update |= ImGui::DragFloat3("Rotation offset", m_RotationOffset);

		ImGui::PopID();

		if (update)
		{
			m_WorldConstantBuffer.World = DirectX::XMMatrixTranspose(
				DX::XMMatrixRotationRollPitchYaw(
					m_RotationOffset[0],
					m_RotationOffset[1],
					m_RotationOffset[2]
				) *
				DX::XMMatrixTranslation(
					m_DrawOffset[0],
					m_DrawOffset[1],
					m_DrawOffset[2]
				) *
				DirectX::XMMatrixPerspectiveLH(
					0.25f * 3.14f, GetDeviceResources()->GetAspectRatio(), 1.f, 1000.f
				)
			);

			m_WorldConstantBuffer.WorldInvTranspose = DX::XMMatrixInverse(nullptr, m_WorldConstantBuffer.World);

			auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

			d3dcontext->UpdateSubresource(
				m_d3dWorldConstantBuffer.Get(),
				0,
				nullptr,
				&m_WorldConstantBuffer,
				sizeof(m_WorldConstantBuffer),
				1
			);
		}
	}


	void LightDemo::RenderLightsEdit()
	{
		ImGui::PushID("Light");

		ImGui::Text("Light constants");
		bool update = ImGui::DragFloat3("Draw offset", &m_LightConstantBuffer.WorldEyePosition.x);
		update |= ImGui::DragInt("Type", reinterpret_cast<int*>(&m_LightConstantBuffer.LightType), 1.f, 0, 2);

		for (auto& [type, name, color_vec] : {
			std::tuple{ LightType::Directional, "Amboient", &m_LightConstantBuffer.DirLight.Ambient },
			std::tuple{ LightType::Directional, "Diffuse", &m_LightConstantBuffer.DirLight.Diffuse },
			std::tuple{ LightType::Directional, "Specular", &m_LightConstantBuffer.DirLight.Specular },


			std::tuple{ LightType::PointLight, "Amboient", &m_LightConstantBuffer.PtLight.Ambient },
			std::tuple{ LightType::PointLight, "Diffuse", &m_LightConstantBuffer.PtLight.Diffuse },
			std::tuple{ LightType::PointLight, "Specular", &m_LightConstantBuffer.PtLight.Specular },


			std::tuple{ LightType::SpotLight, "Amboient", &m_LightConstantBuffer.SpLight.Ambient },
			std::tuple{ LightType::SpotLight, "Diffuse", &m_LightConstantBuffer.SpLight.Diffuse },
			std::tuple{ LightType::SpotLight, "Specular", &m_LightConstantBuffer.SpLight.Specular }
			})
		{
			if (m_LightConstantBuffer.LightType == type)
			{
				float color_arr[]{
					DX::XMVectorGetX(*color_vec),
					DX::XMVectorGetY(*color_vec),
					DX::XMVectorGetZ(*color_vec),
					DX::XMVectorGetW(*color_vec)
				};
				if (ImGui::ColorEdit4(name, color_arr))
				{
					update = true;
					*color_vec = DX::XMVectorSet(color_arr[0], color_arr[1], color_arr[2], color_arr[3]);
				}
			}
		}
		
		ImGui::PopID();

		if (update)
		{
			auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

			d3dcontext->UpdateSubresource(
				m_d3dLightConstantBuffer.Get(),
				0,
				nullptr,
				&m_LightConstantBuffer,
				sizeof(m_LightConstantBuffer),
				1
			);
		}
	}
}
