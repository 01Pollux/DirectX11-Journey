#pragma once 

#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class RandCSLength : public ISandbox
	{
	public:
		RandCSLength(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;

		static const char* GetName() noexcept
		{
			return "Random CS Length";
		}

	private:
		static std::vector<std::array<float, 3>> RandomValues();

		void InitializeBuffers();

	private:
		bool m_Processed{};

		DX::ComPtr<ID3D11ComputeShader> m_VecLengthCS;
		DX::ComPtr<ID3D11ShaderResourceView> m_VecInView;
		DX::ComPtr<ID3D11UnorderedAccessView> m_VecOutView;
		DX::ComPtr<ID3D11Buffer> m_VecOutBuf, m_VecCPU;
	};
}