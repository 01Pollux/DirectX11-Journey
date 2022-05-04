#pragma once

#include "directx/DeviceResources.hpp"
#include "directx/Camera.hpp"

#include "sandbox/geometry_factory.hpp"

#include <variant>

namespace Pleiades::Sandbox
{
	class RenderConstantBuffer_t;

	class IRenderable
	{
	public:
		using MeshData_t = GeometryInstance::MeshData_t;

		struct DrawInfoGeneric_t
		{
			uint32_t VertexCount, StartVertexPos;
		};

		struct DrawInfoIndexed_t
		{
			uint32_t IndexCount, StartIndexPos, StartVertexPos;
		};

		struct DrawInfoIndexedInstanced_t
		{
			uint32_t IndexCount, InstanceCount, StartIndexPos, StartInstancePos, StartVertexPos;
		};

		struct DrawInfoInstanced_t
		{
			uint32_t InstanceCount, VertexCount, StartInstancePos, StartVertexPos;
		};

		using DrawVariant_t = std::variant<
			DrawInfoGeneric_t,
			DrawInfoIndexed_t,
			DrawInfoIndexedInstanced_t,
			DrawInfoInstanced_t
		>;

	public:
		IRenderable(
			DX::Camera* camera,
			DX::DeviceResources* d3dres,
			const MeshData_t& mesh
		) noexcept : 
			m_Camera(camera),
			m_DeviceRes(d3dres),
			m_MeshInfo(mesh)
		{}

		IRenderable(
			DX::Camera* camera,
			DX::DeviceResources* d3dres
		) noexcept :
			m_Camera(camera),
			m_DeviceRes(d3dres)
		{}

		virtual ~IRenderable() = default;
		
		void FreeMeshInfo()
		{
			m_MeshInfo = {};
		}

		void SetDrawInfo(
			DrawVariant_t draw_info
		) noexcept
		{
			m_DrawInfo = std::move(draw_info);
		}
		
		void ReadShader(
			_In_ const wchar_t* file,
			_Outptr_ ID3DBlob** buffer
		);

	public:
		void Draw(RenderConstantBuffer_t& cbuffer);

		[[nodiscard]]
		DX::DeviceResources* GetDeviceResources() const noexcept
		{
			return m_DeviceRes;
		}
		
		[[nodiscard]]
		DX::Camera* GetCamera() const noexcept
		{
			return m_Camera;
		}

		void SetCamera(DX::Camera* camera) noexcept
		{
			m_Camera = camera;
		}

	protected:
		virtual void BeginDraw(RenderConstantBuffer_t&)
		{}

		virtual void EndDraw(RenderConstantBuffer_t&)
		{}

	protected:
		MeshData_t m_MeshInfo;
		DrawVariant_t m_DrawInfo;

	private:
		DX::Camera* m_Camera;
		DX::DeviceResources* m_DeviceRes;
	};
}