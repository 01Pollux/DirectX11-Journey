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
			using indices_type = uint16_t;

			std::vector<verticies_type> vertices;
			std::vector<indices_type> indices;
		};

		struct MeshOffset_t
		{
			size_t Vtx;
			size_t Idx;
		};

		MeshData_t Mesh;
		std::vector<MeshOffset_t> MeshSizes;

		DX::ComPtr<ID3D11Buffer>		 d3dIndices, d3dVerticies;
		DX::ConstantBuffer<DX::XMMATRIX> d3dConstants_WRP;

		DX::ComPtr<ID3D11InputLayout>	d3dInputLayout;
		DX::ComPtr<ID3D11VertexShader>	d3dVtxShader;
		DX::ComPtr<ID3D11PixelShader>	d3dPxlShader;

		GeometryInstance(GeometryInstance*);

		void CreateBuffers(ID3D11Device* d3ddevice);
		
		void CreateShaders(ID3D11Device* d3ddevice);

		void Bind(ID3D11DeviceContext* d3dcontext);
		
		void PushMesh(MeshData_t&& mesh)
		{
			MeshSizes.emplace_back(
				mesh.vertices.size(),
				mesh.indices.size()
			);

			Mesh.vertices.insert(
				Mesh.vertices.end(),
				std::make_move_iterator(mesh.vertices.begin()),
				std::make_move_iterator(mesh.vertices.end())
			);

			Mesh.indices.insert(
				Mesh.indices.end(),
				std::make_move_iterator(mesh.indices.begin()),
				std::make_move_iterator(mesh.indices.end())
			);
		}
		
		void PushMesh(const MeshData_t& mesh)
		{
			MeshSizes.emplace_back(
				mesh.vertices.size(),
				mesh.indices.size()
			);

			Mesh.vertices.insert(
				Mesh.vertices.end(),
				mesh.vertices.begin(),
				mesh.vertices.end()
			);


			Mesh.indices.insert(
				Mesh.indices.end(),
				mesh.indices.begin(),
				mesh.indices.end()
			);
		}

		void Draw(ID3D11DeviceContext* d3dcontext)
		{
			size_t vtx_offset{}, idx_offset{};
			for (const auto& [vtx_size, idx_size] : MeshSizes)
			{
				d3dcontext->DrawIndexed(
					static_cast<uint32_t>(idx_size),
					static_cast<uint32_t>(idx_offset),
					static_cast<uint32_t>(vtx_offset)
				);

				idx_offset += idx_size;
				vtx_offset += vtx_size;
			}
		}
	};

	class GeometryFactory
	{
	public:
		using MeshData_t = GeometryInstance::MeshData_t;

		static MeshData_t CreatePlane(
			uint32_t rows,
			uint32_t columns,
			float width,
			float height
		)
		{
			MeshData_t mesh;
			CreatePlaneVertices(mesh, rows, columns, width, height);
			CreatePlaneIndices(mesh, rows, columns);
			return mesh;
		}

		static void CreatePlane(
			std::unique_ptr<GeometryInstance>& geometry,
			uint32_t rows,
			uint32_t columns,
			float width,
			float height
		)
		{
			geometry = std::make_unique<GeometryInstance>(geometry.get());
			geometry->PushMesh(CreatePlane(rows, columns, width, height));
		}


		static MeshData_t CreateCylinder(
			uint32_t slices,
			uint32_t stacks,
			float bottomn_radius,
			float top_radius,
			float height
		)
		{
			MeshData_t mesh;
			CreateCylinderVerticesAndIndices(mesh, slices, stacks, bottomn_radius, top_radius, height);
			return mesh;
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
			geometry->PushMesh(CreateCylinder(slices, stacks, bottomn_radius, top_radius, height));
		}


		static MeshData_t CreateSphere(
			uint32_t slices,
			uint32_t stacks,
			float radius
		)
		{
			MeshData_t mesh;
			CreateSphereVertices(mesh, slices, stacks, radius);
			CreateSphereIndices(mesh, slices, stacks);
			return mesh;
		}

		static void CreateSphere(
			std::unique_ptr<GeometryInstance>& geometry,
			uint32_t slices,
			uint32_t stacks,
			float radius
		)
		{
			geometry = std::make_unique<GeometryInstance>(geometry.get());
			geometry->PushMesh(CreateSphere(slices, stacks, radius));
		}


		static MeshData_t CreateGeoSphere(
			uint32_t num_divisions,
			float radius
		)
		{
			MeshData_t mesh;
			CreateSphereVerticesAndIndices(mesh, num_divisions, radius);
			return mesh;
		}

		static void CreateGeoSphere(
			std::unique_ptr<GeometryInstance>& geometry,
			uint32_t num_divisions,
			float radius
		)
		{
			geometry = std::make_unique<GeometryInstance>(geometry.get());
			geometry->PushMesh(CreateGeoSphere(num_divisions, radius));
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

		static void CreatePlaneIndices(
			MeshData_t& mesh,
			uint32_t rows,
			uint32_t columns
		);


		static void CreateCylinderVerticesAndIndices(
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

		static void CreateCylinderIndices(
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

		static void CreateSphereIndices(
			MeshData_t& mesh,
			uint32_t slices,
			uint32_t stacks
		);


		static void CreateSphereVerticesAndIndices(
			MeshData_t& mesh,
			uint32_t num_divisions,
			float radius
		);

		static void SubdivideMesh(MeshData_t& mesh);
	};
}