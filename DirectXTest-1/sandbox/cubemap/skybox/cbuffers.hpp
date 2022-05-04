#pragma once

#include <DirectXMath.h>
#include "sandbox/render/cbuffer.hpp"

namespace Pleiades::Sandbox::SkyboxCubeMapDemo
{
	struct WorldConstantBuffers_t
	{
		DirectX::XMMATRIX WorldViewProj;
	};

	struct CBuffers_t
	{
		RenderConstantBuffer_t Buffers;

		WorldConstantBuffers_t WorldConstants{ };

		CBuffers_t(DX::DeviceResources* d3dres) :
			Buffers(d3dres)
		{
			uint8_t* buffers[]{ std::bit_cast<uint8_t*>(&WorldConstants) };
			size_t buffer_size[]{ sizeof(WorldConstants) };
			bool dynamics[]{ true };

			Buffers.CreateBuffers(
				1,
				buffers,
				buffer_size,
				dynamics
			);
		}
	};
}