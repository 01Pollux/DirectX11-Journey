#pragma once

#include <array>
#include "utils/sandbox.hpp"

namespace Pleiades::Sandbox
{
	class IRenderableShape
	{
	public:
		IRenderableShape(const char* name, const std::array<float, 3>& draw_offset, DX::DeviceResources* d3dres) noexcept :
			m_Name(name), m_DrawOffset(draw_offset), m_D3DResources(d3dres)
		{}

		virtual ~IRenderableShape() = default;
		virtual void Render(uint64_t ticks) = 0;
		virtual void ImGuiRender() {}

		[[nodiscard]]
		float* GetDrawOffset() noexcept
		{
			return m_DrawOffset.data();
		}

		[[nodiscard]]
		const char* GetName() const noexcept
		{
			return m_Name;
		}

	protected:
		const char* m_Name;
		std::array<float, 3> m_DrawOffset;

		[[nodiscard]]
		const auto& GetDeviceResources() const noexcept
		{
			return m_D3DResources;
		}

	private:
		DX::DeviceResources* m_D3DResources;
	};

	class ManyShapes : public ISandbox
	{
	public:
		[[nodiscard]]
		static constexpr const char* GetName() noexcept
		{
			return "Many shapes";
		}

		ManyShapes(DX::DeviceResources* d3dres);

		void OnFrame(uint64_t ts) override;
		void OnImGuiDraw() override;

	private:
		std::vector<
			std::pair<
			std::unique_ptr<IRenderableShape> /*shape*/,
			bool /*should render*/
			>
		> m_Shapes;
	};
}