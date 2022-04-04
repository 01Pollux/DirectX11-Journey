#pragma once

#include "utils/pch.hpp"
#include "directx/DeviceResources.hpp"
#include <DirectXTK/VertexTypes.h>

namespace Pleiades
{
	struct GeometryInstance
	{
		struct MeshData_t
		{
			using verticies_type = DX::VertexPositionColor;
			using indicies_type = uint16_t;

			std::vector<verticies_type> vertices;
			std::vector<indicies_type> indicies;
		};

		MeshData_t Mesh;

		DX::ComPtr<ID3D11Buffer>		 d3dIndicies, d3dVerticies;
		DX::ConstantBuffer<DX::XMMATRIX> d3dConstants_WRP;

		DX::ComPtr<ID3D11InputLayout>	d3dInputLayout;
		DX::ComPtr<ID3D11VertexShader>	d3dVtxShader;
		DX::ComPtr<ID3D11PixelShader>	d3dPxlShader;

		GeometryInstance(GeometryInstance*);

		void CreateBuffers(ID3D11Device* d3ddevice);
		
		void CreateShaders(ID3D11Device* d3ddevice);

		void Bind(ID3D11DeviceContext* d3dcontext);
		
		void Draw(ID3D11DeviceContext* d3dcontext)
		{
			d3dcontext->DrawIndexed(static_cast<uint32_t>(Mesh.indicies.size()), 0, 0);
		}
	};

	class GeometryFactory
	{
	public:
		using MeshData_t = GeometryInstance::MeshData_t;

		static void CreatePlane(
			std::unique_ptr<GeometryInstance>& geometry,
			uint32_t rows,
			uint32_t columns,
			float width,
			float height
		)
		{
			geometry = std::make_unique<GeometryInstance>(geometry.get());

			CreatePlaneVertices(geometry->Mesh, rows, columns, width, height);
			CreatePlaneIndicies(geometry->Mesh, rows, columns);
		}

		static void CreateCylinder(
			std::unique_ptr<GeometryInstance>& geometry,
			uint32_t slices,
			uint32_t stacks,
			float bottomn_radius,
			float top_radius,
			float height
		)
		{
			geometry = std::make_unique<GeometryInstance>(geometry.get());

			CreateCylinderVerticesAndIndicies(geometry->Mesh, slices, stacks, bottomn_radius, top_radius, height);
		}


		static void CreateSphere(
			std::unique_ptr<GeometryInstance>& geometry,
			uint32_t slices,
			uint32_t stacks,
			float height
		)
		{
			geometry = std::make_unique<GeometryInstance>(geometry.get());

			CreateSphereVertices(geometry->Mesh, slices, stacks, height);
			CreateSphereIndicies(geometry->Mesh, slices, stacks);
		}


		static float GetHeight(float x, float z) noexcept
		{
			return .3f * (z * std::sinf(x * .1f) + x * std::cosf(z * .1f));
		}

	private:
		static void CreatePlaneVertices(
			MeshData_t& mesh,
			uint32_t rows,
			uint32_t columns,
			float width,
			float height
		);

		static void CreatePlaneIndicies(
			MeshData_t& mesh,
			uint32_t rows,
			uint32_t columns
		);


		static void CreateCylinderVerticesAndIndicies(
			MeshData_t& mesh,
			uint32_t slices,
			uint32_t stacks,
			float bottomn_radius,
			float top_radius,
			float height
		);

		static void CreateCylinderTopFace(
			MeshData_t& mesh,
			uint32_t slices,
			float top_radius,
			float height
		);


		static void CreateCylinderBottomFace(
			MeshData_t& mesh,
			uint32_t slices,
			float bottomn_radius,
			float height
		);

		static void CreateCylinderIndicies(
			MeshData_t& mesh,
			uint32_t slices,
			uint32_t stacks
		);

		static void CreateSphereVertices(
			MeshData_t& mesh,
			uint32_t slices,
			uint32_t stacks,
			float radius
		);

		static void CreateSphereIndicies(
			MeshData_t& mesh,
			uint32_t slices,
			uint32_t stacks
		);
	};
}