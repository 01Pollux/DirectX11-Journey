#pragma once

#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	

	class SimplePlane : public ISandbox
	{
	public:
		struct MeshData_t
		{
			std::vector<DirectX::VertexPosition> vertices;
			std::vector<unsigned short> indicies;
		};

		SimplePlane(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t) override;

		static const char* GetName() noexcept
		{
			return "Simple plane";
		}

	private:
		void BuildPlane();

		MeshData_t m_MeshData;
	};
}