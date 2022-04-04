
#include "utils/pch.hpp"
#include "directx/DeviceResources.hpp"

#include "shapes.hpp"

#include "imgui/imgui.hpp"
#include <d3dcompiler.h>

// shapes
#include "cube3d.hpp"
#include "cylinder.hpp"
#include "sphere.hpp"

namespace Pleiades::Sandbox
{
	ManyShapes::ManyShapes(DX::DeviceResources* d3dres) :
		ISandbox(d3dres)
	{
		{
			m_Shapes.emplace_back(
				std::make_unique<RenderableCube3D>(d3dres, "Cube 3D #1"),
				false
			);

			m_Shapes.emplace_back(
				std::make_unique<RenderableCube3D>(d3dres, "Cube 3D #2"),
				false
			);

			m_Shapes.emplace_back(
				std::make_unique<RenderableCylinder>(d3dres, "Cylinder"),
				false
			);
			m_Shapes.emplace_back(
				std::make_unique<RenderableSphere>(d3dres, "Sphere"),
				true
			);
		}
	}

	void ManyShapes::OnFrame(uint64_t ticks)
	{
		for (auto& [shape, should_render] : m_Shapes)
		{
			if (should_render)
				shape->Render(ticks);
		}
	}

	void ManyShapes::OnImGuiDraw()
	{
		for (auto& [shape, should_render] : m_Shapes)
		{
			ImGui::PushID(shape.get());
			ImGui::Checkbox("Enable", &should_render); ImGui::SameLine();
			ImGui::DragFloat3(shape->GetName(), shape->GetDrawOffset());

			if (should_render)
				shape->ImGuiRender();

			ImGui::PopID();
		}
	}
}