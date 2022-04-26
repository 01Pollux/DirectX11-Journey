#pragma once

#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class ComputeShader_VecAdd : public ISandbox
	{
	public:
		static constexpr uint32_t NumVecs = 32;

		struct VecAdd_t
		{
			DX::XMFLOAT3 v0;
			DX::XMFLOAT2 v1;
		};

		ComputeShader_VecAdd(DX::DeviceResources* d3dres);

		virtual void OnFrame(uint64_t);

		static const char* GetName() noexcept
		{
			return "CS: Vec Add";
		}

	private:
		bool m_Processed{};

		DX::ComPtr<ID3D11ComputeShader> m_VecAddCS;
		DX::ComPtr<ID3D11UnorderedAccessView> m_VecAddUAV;
		DX::ComPtr<ID3D11Buffer> m_VecAddUAV_buffer, m_VecAddUAV_buffertmp;
		DX::ComPtr<ID3D11ShaderResourceView> m_VecA, m_VecB;
	};
}