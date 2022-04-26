#pragma once

#include "sandbox/helper.hpp"
#include "utils/sandbox.hpp"
#include <bitset>

namespace Pleiades::Sandbox::BlurredWavesBlendingEffect
{
	class EffectManager
	{
	public:
		struct NonNumericConstants
		{
			DX::ComPtr<ID3D11ShaderResourceView> Texture;
			DX::ComPtr<ID3D11SamplerState> Sampler;

			DX::ComPtr<ID3D11ShaderResourceView> CSOutputTexture;
			DX::ComPtr<ID3D11UnorderedAccessView> CSInputUAV;

			DX::ComPtr<ID3D11ComputeShader> CSHorz, CSVert;
		};

		EffectManager(NonNumericConstants&& shader_info) :
			m_NBuffer(std::move(shader_info))
		{}

		void SetTexture(ID3D11ShaderResourceView* texture)
		{
			m_NBuffer.Texture = texture;
		}

		void Bind(ID3D11DeviceContext* d3dcontext)
		{
			d3dcontext->PSSetShaderResources(
				0, 1, m_NBuffer.Texture.GetAddressOf()
			);
			d3dcontext->PSSetSamplers(
				0, 1, m_NBuffer.Sampler.GetAddressOf()
			);
		}
		
		void Unbind(ID3D11DeviceContext* d3dcontext)
		{
			ID3D11ShaderResourceView* null_shader[1]{};
			ID3D11SamplerState* null_sampler[1]{};
			d3dcontext->PSSetShaderResources(
				0, 1, null_shader
			);
			d3dcontext->PSSetSamplers(
				0, 0, null_sampler
			);
		}

		void BlurInPlace(
			ID3D11DeviceContext* d3dcontext,
			ID3D11ShaderResourceView* input_srv,
			ID3D11UnorderedAccessView* output_uav,
			uint32_t size[2],
			int blur_factor
		)
		{
			const uint32_t width = static_cast<uint32_t>(ceilf(size[0] / 256.f));
			const uint32_t height = static_cast<uint32_t>(ceilf(size[1] / 256.f));

			for (int i = 0; i < blur_factor; i++)
			{
				d3dcontext->CSSetShader(m_NBuffer.CSHorz.Get(), nullptr, 0);

				d3dcontext->CSSetShaderResources(0, 1, &input_srv);
				d3dcontext->CSSetUnorderedAccessViews(0, 1, m_NBuffer.CSInputUAV.GetAddressOf(), nullptr);

				d3dcontext->Dispatch(width, size[1], 1);


				ID3D11ShaderResourceView* empty_shader[1]{};
				d3dcontext->CSSetShaderResources(0, 1, empty_shader);
				ID3D11UnorderedAccessView* empty_views[1]{};
				d3dcontext->CSSetUnorderedAccessViews(0, 1, empty_views, nullptr);


				d3dcontext->CSSetShader(m_NBuffer.CSVert.Get(), nullptr, 0);
				d3dcontext->CSSetShaderResources(0, 1, m_NBuffer.CSOutputTexture.GetAddressOf());
				d3dcontext->CSSetUnorderedAccessViews(0, 1, &output_uav, nullptr);

				d3dcontext->Dispatch(size[0], height, 1);


				d3dcontext->CSSetShaderResources(0, 1, empty_shader);
				d3dcontext->CSSetUnorderedAccessViews(0, 1, empty_views, nullptr);
			}

			d3dcontext->CSSetShader(nullptr, nullptr, 0);
		}

	private:
		NonNumericConstants m_NBuffer;
	};
}
