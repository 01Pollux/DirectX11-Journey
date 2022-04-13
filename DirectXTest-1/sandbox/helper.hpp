#pragma once

#include "utils/pch.hpp"

namespace Pleiades::Sandbox
{
	struct Material
	{
		DX::XMVECTOR Ambient;
		DX::XMVECTOR Diffuse;
		DX::XMVECTOR Specular;
		DX::XMVECTOR Reflect;
	};

	struct DirectionalLight
	{
		DX::XMVECTOR Ambient;
		DX::XMVECTOR Diffuse;
		DX::XMVECTOR Specular;
		
		DX::XMFLOAT3 Direction;
		float _Pad;
	};

	struct PointLight
	{
		DX::XMVECTOR Ambient;
		DX::XMVECTOR Diffuse;
		DX::XMVECTOR Specular;

		DX::XMFLOAT3 Position;
		float Range;

		DX::XMFLOAT3 Attenuation;
		float _Pad;
	};

	struct SpotLight
	{
		DX::XMVECTOR Ambient;
		DX::XMVECTOR Diffuse;
		DX::XMVECTOR Specular;

		DX::XMFLOAT3 Position;
		float Range;

		DX::XMFLOAT3 Direction;
		float ExpFactor;

		DX::XMFLOAT3 Attenuation;
		float _Pad;
	};

	struct GeoInfo_t
	{
		Material Material;
		DX::XMMATRIX World{ DX::XMMatrixIdentity() };

		template<typename _DevieRes, typename _EffectMgr>
		void Bind(_DevieRes* d3dres, _EffectMgr& fxmgr, const DX::XMMATRIX& view_proj)
		{
			auto world_transposed = DX::XMMatrixTranspose(World);

			fxmgr.SetWorld(world_transposed);
			fxmgr.SetWorldInvTranspose(DX::XMMatrixInverse(nullptr, world_transposed));
			fxmgr.SetWorldViewProj(DX::XMMatrixTranspose(World * view_proj));
			fxmgr.SetMaterial(Material);
			fxmgr.Update(d3dres->GetD3DDeviceContext());
		}
	};
}