#include "utils/pch.hpp"
#include "skull.hpp"

#include <fstream>

using namespace DX;

namespace Pleiades::Sandbox
{
	void LitSkullDemo::InitializeShapesMesh()
	{
		// Create a ground
		m_ShapesGeometry.PushMesh(
			GeometryFactory::CreatePlane(
				60, 40,
				20.f, 30.f
			)
		);
		// Draw the ground one time
		m_GeometryCallbacks.emplace_back(
			1,
			[this](size_t)
			{
				m_Plane.Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
			}
		);


		//// Create a sphere
		//m_ShapesGeometry.PushMesh(
		//	GeometryFactory::CreateSphere(
		//		20, 20,
		//		.5f
		//	)
		//);
		//// Draw the sphere 4 times
		//m_GeometryCallbacks.emplace_back(
		//	4,
		//	[this](size_t index)
		//	{
		//		m_Cylinder[index].Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
		//	}
		//);


		// Create a cylinder
		m_ShapesGeometry.PushMesh(
			GeometryFactory::CreateCylinder(
				20, 20,
				.5f, .3f, 3.f
			)
		);
		// Draw the cylinder 4 times
		m_GeometryCallbacks.emplace_back(
			4,
			[this](size_t index)
			{
				m_Sphere[index].Bind(GetDeviceResources(), m_Effects, m_ViewProjection);
			}
		);
	}


	void LitSkullDemo::InitializeSkullMesh()
	{
		std::ifstream file("sandbox/from_file/skull.txt");

		size_t vertex_count, idx_count;
		std::string ignore;

		file >> ignore >> vertex_count;
		file >> ignore >> idx_count;

		std::getline(file, ignore); // \n
		std::getline(file, ignore); // VertexLisst
		std::getline(file, ignore); // {

		GeometryInstance::MeshData_t mesh;
		mesh.vertices.reserve(vertex_count);
		mesh.indices.resize(idx_count * 3);

		while (vertex_count--)
		{
			float x, y, z, nx, ny, nz;
			file >> x >> y >> z >> nx >> ny >> nz;

			mesh.vertices.emplace_back(
				XMFLOAT3{ x, y, z },
				XMFLOAT3{ nx, ny, nz },
				XMFLOAT2{}
			);
		}

		std::getline(file, ignore); // \n
		std::getline(file, ignore); // }
		std::getline(file, ignore); // TriangleList
		std::getline(file, ignore); // {

		for (size_t i = 0; i < idx_count; i++)
		{
			uint32_t i0, i1, i2;
			file >> i0 >> i1 >> i2;

			mesh.indices[i * 3 + 0] = static_cast<uint16_t>(i0);
			mesh.indices[i * 3 + 1] = static_cast<uint16_t>(i1);
			mesh.indices[i * 3 + 2] = static_cast<uint16_t>(i2);
		}

		m_SkullGeometry.PushMesh(std::move(mesh));
	}


	EffectManager::WorldConstantBuffer LitSkullDemo::GetDefaultWolrdConstants()
	{
		EffectManager::WorldConstantBuffer info{};
		
		info.LightCount = 1;

		info.Lights[0].Ambient = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		info.Lights[0].Diffuse = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Lights[0].Specular = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		info.Lights[0].Direction = { .57735f, -.57735f, .57735f };

		info.Lights[1].Ambient = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		info.Lights[1].Diffuse = XMVectorSet(0.20f, 0.20f, 0.20f, 1.0f);
		info.Lights[1].Specular = XMVectorSet(0.25f, 0.25f, 0.25f, 1.0f);
		info.Lights[1].Direction = { -.57735f, -.57735f, .57735f };

		info.Lights[2].Ambient = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		info.Lights[2].Diffuse = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		info.Lights[2].Specular = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		info.Lights[2].Direction = { 0.f, -.707f, -.707f };

		return info;
	}
	

	void LitSkullDemo::InitializeShapes()
	{
		UpdateViewProjection();
		
		m_Plane.World = XMMatrixIdentity();

		m_Skull.World = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f);

		for (size_t i = 0; i < std::size(m_Cylinder) / 2; i++)
		{
			m_Cylinder[i * 2 + 0].World = XMMatrixTranslation(-5.f, 1.5f, -10.f + i * 5);
			m_Cylinder[i * 2 + 0].World = XMMatrixTranslation(+5.f, 1.5f, -10.f + i * 5);

			m_Sphere[i * 2 + 0].World = XMMatrixTranslation(-5.f, 3.5f, -10.f + i * 5);
			m_Sphere[i * 2 + 1].World = XMMatrixTranslation(+5.f, 3.5f, -10.f + i * 5);
		}


		m_Plane.Material.Ambient = XMVectorSet(.48f, .77f, .46f, 1.f);
		m_Plane.Material.Diffuse = XMVectorSet(.48f, .77f, .46f, 1.f);
		m_Plane.Material.Specular = XMVectorSet(.2f, .2f, .2f, 16.f);

		m_Skull.Material.Ambient = XMVectorSet(.8f, .8f, .8f, 1.f);
		m_Skull.Material.Diffuse = XMVectorSet(.8f, .8f, .8f, 1.f);
		m_Skull.Material.Specular = XMVectorSet(.8f, .8f, .8f, 16.f);

		for (auto& cylinder : m_Cylinder)
		{
			cylinder.Material.Ambient = XMVectorSet(.7f, .85f, .7f, 1.f);
			cylinder.Material.Diffuse = XMVectorSet(.7f, .85f, .7f, 1.f);
			cylinder.Material.Specular = XMVectorSet(.8f, .8f, .8f, 16.f);
		}

		for (auto& sphere : m_Sphere)
		{
			sphere.Material.Ambient = XMVectorSet(.1f, .2f, .3f, 1.f);
			sphere.Material.Diffuse = XMVectorSet(.2f, .4f, .6f, 1.f);
			sphere.Material.Specular = XMVectorSet(.9f, .9f, .9f, 16.f);
		}
	}
}