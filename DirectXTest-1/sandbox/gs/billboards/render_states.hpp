#pragma once

#include "sandbox/helper.hpp"
#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox::GSBillboardsDemoFx
{
	class BlendRenderState
	{
	public:
		BlendRenderState(DX::DeviceResources* d3dres)
		{
			auto d3ddevice = d3dres->GetD3DDevice();

			{
				CD3D11_RASTERIZER_DESC rasterize_desc(D3D11_DEFAULT);
				rasterize_desc.FrontCounterClockwise = true;

				DX::ThrowIfFailed(
					d3ddevice->CreateRasterizerState(
						&rasterize_desc,
						m_BackCullState.GetAddressOf()
					)
				);
			}
			{
				D3D11_DEPTH_STENCIL_DESC stencil_desc{};

				stencil_desc.DepthEnable = true;
				stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
				stencil_desc.StencilEnable = true;
				stencil_desc.StencilReadMask = stencil_desc.StencilWriteMask = static_cast<uint8_t>(-1);

				stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
				stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				stencil_desc.BackFace = stencil_desc.FrontFace;

				DX::ThrowIfFailed(
					d3ddevice->CreateDepthStencilState(
						&stencil_desc,
						m_StencilMask.GetAddressOf()
					)
				);
			}
			{
				D3D11_DEPTH_STENCIL_DESC stencil_desc{};

				stencil_desc.DepthEnable = true;
				stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
				stencil_desc.StencilEnable = true;
				stencil_desc.StencilReadMask = stencil_desc.StencilWriteMask = static_cast<uint8_t>(-1);

				stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

				stencil_desc.BackFace = stencil_desc.FrontFace;

				DX::ThrowIfFailed(
					d3ddevice->CreateDepthStencilState(
						&stencil_desc,
						m_StencilApplyMask.GetAddressOf()
					)
				);
			}
			{
				D3D11_DEPTH_STENCIL_DESC stencil_desc{};

				stencil_desc.DepthEnable = true;
				stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
				stencil_desc.StencilEnable = true;
				stencil_desc.StencilReadMask = stencil_desc.StencilWriteMask = static_cast<uint8_t>(-1);

				stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
				stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

				stencil_desc.BackFace = stencil_desc.FrontFace;

				DX::ThrowIfFailed(
					d3ddevice->CreateDepthStencilState(
						&stencil_desc,
						m_StencilBlendOnce.GetAddressOf()
					)
				);
			}
			{
				D3D11_BLEND_DESC blend_desc{};

				blend_desc.RenderTarget[0].BlendEnable = true;
				blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				DX::ThrowIfFailed(
					d3ddevice->CreateBlendState(
						&blend_desc,
						m_AlphaTransparent.GetAddressOf()
					)
				);
			}
			{
				D3D11_BLEND_DESC blend_desc{};

				blend_desc.RenderTarget[0].BlendEnable = false;
				blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
				blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blend_desc.RenderTarget[0].RenderTargetWriteMask = 0;

				DX::ThrowIfFailed(
					d3ddevice->CreateBlendState(
						&blend_desc,
						m_NoDrawBlend.GetAddressOf()
					)
				);
			}
		}

		void SetReverseCull(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			d3dcontext->RSSetState(state ? m_BackCullState.Get() : nullptr);
		}

		void SetStencilMask(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			d3dcontext->OMSetDepthStencilState(state ? m_StencilMask.Get() : nullptr, 1);
		}
		
		void ApplyStencilMask(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			d3dcontext->OMSetDepthStencilState(state ? m_StencilApplyMask.Get() : nullptr, 1);
		}

		void SetStencilBlendOnce(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			d3dcontext->OMSetDepthStencilState(state ? m_StencilBlendOnce.Get() : nullptr, 0);
		}

		void SetAlphaTransparent(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			constexpr float blend_factor[4]{};
			d3dcontext->OMSetBlendState(state ? m_AlphaTransparent.Get() : nullptr, blend_factor, static_cast<uint32_t>(-1));
		}

		void SetNoDraw(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			constexpr float blend_factor[4]{};
			d3dcontext->OMSetBlendState(state ? m_NoDrawBlend.Get() : nullptr, blend_factor, static_cast<uint32_t>(-1));
		}

	private:
		DX::ComPtr<ID3D11RasterizerState> m_BackCullState;

		DX::ComPtr<ID3D11DepthStencilState> m_StencilMask;
		DX::ComPtr<ID3D11DepthStencilState> m_StencilApplyMask;
		DX::ComPtr<ID3D11DepthStencilState> m_StencilBlendOnce;

		DX::ComPtr<ID3D11BlendState> m_AlphaTransparent;
		DX::ComPtr<ID3D11BlendState> m_NoDrawBlend;
	};
}
