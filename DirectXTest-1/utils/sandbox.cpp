#include "utils\pch.hpp"
#include "sandbox.hpp"

#include "imgui/imgui.hpp"

#include "sandbox/manyshapes/shapes.hpp"

#include "sandbox/resolution_changer/resolution_changer.hpp"

#include "sandbox/plane/simple_plane.hpp"
#include "sandbox/plane/plane_and_sphere.hpp"

#include "sandbox/from_file/skull.hpp"
#include "sandbox/from_file/car.hpp"

#include "sandbox/lights/basic_demo/light.hpp"
#include "sandbox/lights/skull_demo/skull.hpp"

#include "sandbox/textures/box/box_texture.hpp"
#include "sandbox/textures/box_multiple/box_texture.hpp"
#include "sandbox/textures/waves/waves_texture.hpp"

namespace Pleiades
{
	void SandboxHolder::Init(DX::DeviceResources* res) noexcept
	{
		m_DeviceRes = res;
		m_States = std::make_unique<DX::CommonStates>(res->GetD3DDevice());

		AddSample<Sandbox::ManyShapes>();
		
		AddSample<Sandbox::ResolutionCChanger_ImGui>();
		
		AddSample<Sandbox::SimplePlane>();
		AddSample<Sandbox::PlaneAndSphere>();
		
		AddSample<Sandbox::SkullFromFile>();
		AddSample<Sandbox::CarFromFile>();
		
		AddSample<Sandbox::LightDemo>();
		AddSample<Sandbox::LitSkullDemo>();

		AddSample<Sandbox::TexturedBox>();
		AddSample<Sandbox::MultipleTexturedBox>();
		AddSample<Sandbox::WavesTextured>();

		Set(Sandbox::MultipleTexturedBox::GetName());
	}

	void SandboxHolder::OnImGuiDraw()
	{
		if (!ImGui::Begin("Sanboxes"))
		{
			ImGui::End();
			return;
		}

		static bool wire_frame = false;
		if (ImGui::Button(wire_frame ? "Fill mode" : "Wire frame"))
		{
			wire_frame = !wire_frame;
			if (wire_frame)
			{
				m_DeviceRes->GetD3DDeviceContext()->RSSetState(
					m_States->Wireframe()
				);
			}
			else
			{
				m_DeviceRes->GetD3DDeviceContext()->RSSetState(
					m_States->CullClockwise()
				);
			}
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