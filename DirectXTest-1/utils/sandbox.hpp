#pragma once

#include <memory>
#include <map>
#include <string>


#include "directx/DeviceResources.hpp"
#include "utils/StepTimer.hpp"


namespace Pleiades
{
	class ISandbox
	{
	public:
		ISandbox(DX::DeviceResources* d3dres) noexcept :
			m_DeviceResource(d3dres)
		{}

		virtual ~ISandbox() = default;

		virtual void OnFrame(uint64_t) { }
		virtual void OnImGuiDraw() {}

		static const char* GetName() noexcept
		{
			return nullptr;
		}

		[[nodiscard]]
		DX::DeviceResources* GetDeviceResources() noexcept
		{
			return m_DeviceResource;
		}

	public:
		ISandbox(const ISandbox&) = delete;	ISandbox& operator=(const ISandbox&) = delete;
		ISandbox(ISandbox&&) = delete;		ISandbox& operator=(ISandbox&&) = delete;

	private:
		DX::DeviceResources* m_DeviceResource;
	};

	class SandboxHolder
	{
	public:
		using sanbox_factory_fn = ISandbox* (*)(DX::DeviceResources*);
		using sanbox_container_t = std::map<std::string, sanbox_factory_fn>;

		template<typename _SampleTy>
		void AddSample()
		{
			m_SandboxFactory.emplace(
				_SampleTy::GetName(),
				[](DX::DeviceResources* res) { return static_cast<ISandbox*>(new _SampleTy(res)); }
			);
		}

		void Init(DX::DeviceResources* res) noexcept;
		void OnImGuiDraw();
		void OnFrame(uint64_t ticks);

	private:
		void Set(const char* name)
		{
			auto iter = m_SandboxFactory.find(name);
			if (iter != m_SandboxFactory.end())
				m_CurrentSanbox.reset(iter->second(m_DeviceRes));
		}

		DX::DeviceResources* m_DeviceRes;

		sanbox_container_t m_SandboxFactory;
		std::unique_ptr<ISandbox> m_CurrentSanbox{};
	};
}