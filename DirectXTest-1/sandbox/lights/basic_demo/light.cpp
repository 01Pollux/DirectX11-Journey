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
		},

		m_SphereMat{
			.Ambient = { 0.48f, 1.f, 0.46f, 1.0f },
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

		m_WorldConstantBuffer.WorldInvTranspose = m_WorldConstantBuffer.WorldViewProj = DX::XMMatrixIdentity();
		m_WorldConstantBuffer.Material = m_LandMat;

		BuildMeshes();
		InitializeBuffers();
	}


	void LightDemo::OnFrame(uint64_t)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();


		SetMaterial(m_LandMat);
		UpdateScene(true);
		m_Land->Bind(d3dcontext);
		m_Land->Draw(d3dcontext);

		SetMaterial(m_SphereMat);
		UpdateScene(false);
		m_Sphere->Bind(d3dcontext);
		m_Sphere->Draw(d3dcontext);
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

		// TODO: use sphere
		GeometryFactory::CreatePlane(
			m_Sphere,
			50, 50,
			160.f, 160.f
		);

		auto d3ddevice = GetDeviceResources()->GetD3DDevice();

		m_Land->CreateBuffers(d3ddevice, true);
		m_Land->CreateShaders(d3ddevice, L"resources/lights/light_test_vs.cso", L"resources/lights/light_test_ps.cso");

		m_Sphere->CreateBuffers(d3ddevice, true);
		m_Sphere->CreateShaders(d3ddevice, L"resources/lights/light_test_vs.cso", L"resources/lights/light_test_ps.cso");
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


	void LightDemo::UpdateScene(bool is_land)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		ID3D11Buffer* buffers[]{ m_d3dWorldConstantBuffer.Get(), m_d3dLightConstantBuffer.Get() };
		d3dcontext->VSSetConstantBuffers(
			0, static_cast<uint32_t>(std::size(buffers)), buffers
		);

		d3dcontext->PSSetConstantBuffers(
			0, static_cast<uint32_t>(std::size(buffers)), buffers
		);


		size_t offset = is_land ? 0 : 1;

		m_WorldConstantBuffer.World = DirectX::XMMatrixTranspose(
			DX::XMMatrixRotationRollPitchYaw(
				m_RotationOffset[offset][0],
				m_RotationOffset[offset][1],
				m_RotationOffset[offset][2]
			) *
			DX::XMMatrixTranslation(
				m_DrawOffset[offset][0],
				m_DrawOffset[offset][1],
				m_DrawOffset[offset][2]
			) *
			DirectX::XMMatrixPerspectiveLH(
				0.25f * 3.14f, GetDeviceResources()->GetAspectRatio(), 1.f, 1000.f
			)
		);

		m_WorldConstantBuffer.WorldInvTranspose = DX::XMMatrixInverse(nullptr, m_WorldConstantBuffer.World);

		d3dcontext->UpdateSubresource(
			m_d3dWorldConstantBuffer.Get(),
			0,
			nullptr,
			&m_WorldConstantBuffer,
			sizeof(m_WorldConstantBuffer),
			1
		);
	}


	void LightDemo::RenderWorldEdit()
	{
		ImGui::PushID("World");

		ImGui::Text("World constants");
		ImGui::DragFloat3("Draw offset (land)", m_DrawOffset[0]);
		ImGui::DragFloat3("Draw offset (sphere)", m_DrawOffset[1]);
		ImGui::DragFloat3("Rotation offset (land)", m_RotationOffset[0]);
		ImGui::DragFloat3("Rotation offset (sphere)", m_RotationOffset[1]);

		size_t i = 0;
		for (auto& [name, color_vec] : {
			std::pair{ "Ambient (land)", &m_LandMat.Ambient },
			std::pair{ "Diffuse (land)", &m_LandMat.Diffuse },
			std::pair{ "Specular (land)", &m_LandMat.Specular },
			std::pair{ "Reflect (land)", &m_LandMat.Reflect },


			std::pair{ "Ambient (sphere)", &m_SphereMat.Ambient },
			std::pair{ "Diffuse (sphere)", &m_SphereMat.Diffuse },
			std::pair{ "Specular (sphere)", &m_SphereMat.Specular },
			std::pair{ "Reflect (sphere)", &m_SphereMat.Reflect }
			})
		{
			if (!(i++ % 4))
				ImGui::Separator();

			float color_arr[]{
					DX::XMVectorGetX(*color_vec),
					DX::XMVectorGetY(*color_vec),
					DX::XMVectorGetZ(*color_vec),
					DX::XMVectorGetW(*color_vec)
			};
			if (ImGui::ColorEdit4(name, color_arr))
				*color_vec = DX::XMVectorSet(color_arr[0], color_arr[1], color_arr[2], color_arr[3]);
		}

		ImGui::PopID();
	}


	void LightDemo::RenderLightsEdit()
	{
		ImGui::PushID("Light");

		ImGui::Text("Light constants");
		bool update = ImGui::DragFloat3("Draw offset", &m_LightConstantBuffer.WorldEyePosition.x);
		update |= ImGui::DragInt("Type", reinterpret_cast<int*>(&m_LightConstantBuffer.LightType), 1.f, 0, 2);

		ImGui::Separator();

		for (auto& [type, name, color_vec] : {
			std::tuple{ LightType::Directional, "Ambient", &m_LightConstantBuffer.DirLight.Ambient },
			std::tuple{ LightType::Directional, "Diffuse", &m_LightConstantBuffer.DirLight.Diffuse },
			std::tuple{ LightType::Directional, "Specular", &m_LightConstantBuffer.DirLight.Specular },


			std::tuple{ LightType::PointLight, "Ambient", &m_LightConstantBuffer.PtLight.Ambient },
			std::tuple{ LightType::PointLight, "Diffuse", &m_LightConstantBuffer.PtLight.Diffuse },
			std::tuple{ LightType::PointLight, "Specular", &m_LightConstantBuffer.PtLight.Specular },


			std::tuple{ LightType::SpotLight, "Ambient", &m_LightConstantBuffer.SpLight.Ambient },
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

		switch (m_LightConstantBuffer.LightType)
		{
		case LightType::Directional:
		{
			update |= ImGui::DragFloat3("Direction", &m_LightConstantBuffer.DirLight.Direction.x);
			break;
		}
		case LightType::PointLight:
		{
			update |= ImGui::DragFloat3("Position", &m_LightConstantBuffer.PtLight.Position.x);
			update |= ImGui::DragFloat3("Attenuation", &m_LightConstantBuffer.PtLight.Attenuation.x);
			update |= ImGui::DragFloat("Range", &m_LightConstantBuffer.PtLight.Range);
			break;
		}
		case LightType::SpotLight:
		{
			update |= ImGui::DragFloat3("Position", &m_LightConstantBuffer.SpLight.Position.x);
			update |= ImGui::DragFloat3("Attenuation", &m_LightConstantBuffer.SpLight.Attenuation.x);
			update |= ImGui::DragFloat3("Direction", &m_LightConstantBuffer.SpLight.Direction.x);
			update |= ImGui::DragFloat("Range", &m_LightConstantBuffer.SpLight.Range);
			break;
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


	void LightDemo::SetMaterial(const Material& material)
	{
		m_WorldConstantBuffer.Material = material;
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
