
#include "utils/pch.hpp"
#include "renderable.hpp"

#include <d3dcompiler.h>

namespace Pleiades::Sandbox
{
	void IRenderable::ReadShader(
		_In_ const wchar_t* file,
		_Outptr_ ID3DBlob** buffer
	)
	{
		DX::ThrowIfFailed(
			D3DReadFileToBlob(
				file,
				buffer
			)
		);
	}


	void IRenderable::Draw(RenderConstantBuffer_t& cbuffer)
	{
		auto d3dcontext = GetDeviceResources()->GetD3DDeviceContext();

		this->BeginDraw(cbuffer);

		if (std::holds_alternative<DrawInfoIndexed_t>(m_DrawInfo))
		{
			const auto& draw_info = std::get<DrawInfoIndexed_t>(m_DrawInfo);
			d3dcontext->DrawIndexed(draw_info.IndexCount, draw_info.StartIndexPos, draw_info.StartVertexPos);
		}
		else if (std::holds_alternative<DrawInfoIndexedInstanced_t>(m_DrawInfo))
		{
			const auto& draw_info = std::get<DrawInfoIndexedInstanced_t>(m_DrawInfo);
			d3dcontext->DrawIndexedInstanced(draw_info.IndexCount, draw_info.InstanceCount, draw_info.StartIndexPos, draw_info.StartInstancePos, draw_info.StartVertexPos);
		}
		else if (std::holds_alternative<DrawInfoGeneric_t>(m_DrawInfo))
		{
			const auto& draw_info = std::get<DrawInfoGeneric_t>(m_DrawInfo);
			d3dcontext->Draw(draw_info.VertexCount, draw_info.StartVertexPos);
		}
		else if (std::holds_alternative<DrawInfoInstanced_t>(m_DrawInfo))
		{
			const auto& draw_info = std::get<DrawInfoInstanced_t>(m_DrawInfo);
			d3dcontext->DrawInstanced(draw_info.InstanceCount, draw_info.VertexCount, draw_info.StartInstancePos, draw_info.StartVertexPos);
		}

		this->EndDraw(cbuffer);
	}
}