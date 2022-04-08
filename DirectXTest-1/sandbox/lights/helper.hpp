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
}