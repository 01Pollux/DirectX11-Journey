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

				blend_desc.AlphaToCoverageEnable = true;
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
						m_AlphaToCoverage.GetAddressOf()
					)
				);
			}
		}

		void SetAlphaToCoverageTransparent(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			constexpr float blend_factor[4]{};
			d3dcontext->OMSetBlendState(state ? m_AlphaToCoverage.Get() : nullptr, blend_factor, static_cast<uint32_t>(-1));
		}

		void SetAlphaTransparent(ID3D11DeviceContext* d3dcontext, bool state = true)
		{
			constexpr float blend_factor[4]{};
			d3dcontext->OMSetBlendState(state ? m_AlphaTransparent.Get() : nullptr, blend_factor, static_cast<uint32_t>(-1));
		}

	private:
		DX::ComPtr<ID3D11BlendState> m_AlphaToCoverage;
		DX::ComPtr<ID3D11BlendState> m_AlphaTransparent;
	};
}
