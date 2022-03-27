#pragma once

#include <vector>
#include <functional>
#include <array>

#include "../scopes.hpp"

namespace imcxx::misc
{
	enum class shortcuts_type : char
	{
		bind_persist,
		bind_default
	};

	/// <summary>
	/// Create or query a notification, configurable by 'popup_info'
	/// </summary>
	class shortcuts : public scope_wrap<shortcuts, imcxx::scope_traits::no_dtor, false>
	{
	public:
		using keys_t = std::array<uint16_t, 3>;

		class context_guard;

		/// <summary>
		/// Process all of the popups that were pressed
		/// </summary>
		IMGUI_API static void handle_callbacks(size_t override_ctx = 0);

		/// <summary>
		/// create a new shortcuts context and set it to default
		/// </summary>
		/// <returns></returns>
		IMGUI_API static size_t push_context();

		/// <summary>
		/// create a new shortcuts context and set it to default
		/// </summary>
		/// <returns></returns>
		IMGUI_API static void pop_context();

		/// <summary>
		/// Bind a shortcut to an user callback
		/// </summary>
		IMGUI_API static void bind(const char* id, std::initializer_list<keys_t> multiple_keys, shortcuts_type type, std::function<void(bool*)> callback);

		/// <summary>
		/// Bind a shortcut to an user callback
		/// </summary>
		static void bind(const char* id, keys_t trigger_keys, shortcuts_type type, std::function<void(bool*)> callback)
		{
			bind(id, { trigger_keys }, type, std::move(callback));
		}

		/// <summary>
		/// Check if the keys are bounded
		/// </summary>
		IMGUI_API static bool is_bound(const char* id, keys_t trigger_keys);

		/// <summary>
		/// Unbind a shortcut from user callback
		/// </summary>
		IMGUI_API static void unbind(const char* id, keys_t trigger_keys);

		/// <summary>
		/// Unbind a shortcut from user callback
		/// </summary>
		static void unbind(const char* id)
		{
			unbind(id, {});
		}
	};

	/// <summary>
	/// Help class for RAII shortcut's context
	/// </summary>
	class shortcuts::context_guard
	{
	public:
		context_guard() :
			m_Idx(push_context())
		{}

		~context_guard()
		{
			pop_context();
		}

		[[nodiscard]]
		operator size_t() const noexcept
		{
			return m_Idx;
		}

		[[nodiscard]]
		size_t get() const noexcept
		{
			return m_Idx;
		}

		void call()
		{
			handle_callbacks(get());
		}

	private:
		size_t m_Idx;
	};
}
