#include "utils\pch.hpp"
#include "sandbox.hpp"

#include "imgui/imgui.hpp"

#include "sandbox/manyshapes/shapes.hpp"
#include "sandbox/resolution_changer/resolution_changer.hpp"
#include "sandbox/plane/simple_plane.hpp"

namespace Pleiades
{
	void SandboxHolder::Init(DX::DeviceResources* res) noexcept
	{
		m_DeviceRes = res;

		AddSample<Sandbox::ManyShapes>();
		AddSample<Sandbox::ResolutionCChanger_ImGui>();
		AddSample<Sandbox::SimplePlane>();

		Set(Sandbox::ManyShapes::GetName());
	}

	void SandboxHolder::OnImGuiDraw()
	{
		if (!ImGui::Begin("Sanboxes"))
		{
			ImGui::End();
			return;
		}

		if (m_CurrentSanbox)
		{
			if (ImGui::Button("<--"))
				m_CurrentSanbox.reset();
			else m_CurrentSanbox->OnImGuiDraw();
		}
		else
		{
			for (auto& test : m_SandboxFactory)
			{
				if (ImGui::Button(test.first.c_str()))
				{
					m_CurrentSanbox.reset(test.second(m_DeviceRes));
					break;
				}
			}
		}
		ImGui::End();
	}

	void SandboxHolder::OnFrame(uint64_t ticks)
	{
		if (m_CurrentSanbox)
			m_CurrentSanbox->OnFrame(ticks);
	}
}