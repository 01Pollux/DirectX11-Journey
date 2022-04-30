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
#include "sandbox/textures/box_anim/box_texture.hpp"
#include "sandbox/textures/waves/waves_texture.hpp"

#include "sandbox/blend/blend_waves.hpp"

#include "sandbox/stencilling/mirror/mirror.hpp"
#include "sandbox/gs/subdivison/subdivison.hpp"
#include "sandbox/gs/billboards/billboards.hpp"

#include "sandbox/cs/vec_add/vec_add.hpp"
#include "sandbox/cs/blur/blur.hpp"
#include "sandbox/cs/rand/rand.hpp"

#include "sandbox/tesselation/basic/basic_tess.hpp"
#include "sandbox/tesselation/bezier/bezier_tess.hpp"

#include "sandbox/camera/mirror/mirror.hpp"
#include "sandbox/frustum/instancing_culling/instancing_culling.hpp"

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
		AddSample<Sandbox::AnimatedTexturedBox>();
		AddSample<Sandbox::WavesTextured>();

		AddSample<Sandbox::WavesBlendingDemo>();

		AddSample<Sandbox::MirrorSkullWorld>();

		AddSample<Sandbox::GSSubdivisonDemo>();
		AddSample<Sandbox::GSBillboardsDemo>();

		AddSample<Sandbox::ComputeShader_VecAdd>();
		AddSample<Sandbox::BlurredTextureDemo>();
		AddSample<Sandbox::RandCSLength>();

		AddSample<Sandbox::BasicTesselation>();
		AddSample<Sandbox::BezierTesselation>();

		AddSample<Sandbox::InstancedFrustum>();

		Set(Sandbox::CamMirrorSkullWorld::GetName());
		Set(Sandbox::InstancedFrustum::GetName());
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
					nullptr
				);
			}
		}
		ImGui::Spacing();

		if (m_CurrentSanbox)
		{
			if (ImGui::Button("<--"))
				m_CurrentSanbox.reset();
			else m_CurrentSanbox->OnImGuiDraw();
		}
		else
		{
			static ImGuiTextFilter search_filter;
			search_filter.Draw("Samples");

			static std::vector<decltype(m_SandboxFactory)::iterator> entries;
			entries.clear();

			for (auto iter = m_SandboxFactory.begin(); iter != m_SandboxFactory.end(); iter++)
			{
				if (!search_filter.PassFilter(iter->first.c_str(), iter->first.c_str() + iter->first.size()))
					continue;

				entries.emplace_back(iter);
			}

			ImVec2 half_screen = ImGui::GetContentRegionAvail();
			half_screen.x /= 2.f;

			if (ImGui::BeginChild("##First half", half_screen))
			{
				for (size_t i = 0, first_half = entries.size() / 2; i < entries.size(); i++)
				{
					if (i > first_half)
					{
						ImGui::EndChild();

						ImGui::SameLine();
						if (!ImGui::BeginChild("##Second half", half_screen))
							break;
					}

					if (ImGui::Button(entries[i]->first.c_str()))
					{
						m_CurrentSanbox.reset(entries[i]->second(m_DeviceRes));
						break;
					}
				}
			}

			ImGui::EndChild();
		}
		ImGui::End();
	}
	
	void SandboxHolder::OnFrame(uint64_t ticks)
	{
		if (m_CurrentSanbox)
			m_CurrentSanbox->OnFrame(ticks);
	}
}