
#include "../shortcut.hpp"

#include <set>
#include <map>

namespace ImGui { extern ImGuiKeyData* GetKeyData(ImGuiKey key); }

namespace imcxx::misc
{
#ifndef IMCXX_MISC_NO_SHORTCUT_DEMO
	void render_shortcuts_demo();
#endif

	struct shortcut_info
	{
		std::vector<shortcuts::keys_t> Keys;
		std::function<void(bool*)> Callback;
		shortcuts_type Type;
		bool Render{};

		shortcut_info(
			std::function<void(bool*)>&& callback,
			shortcuts_type type
		) : 
			Callback(std::move(callback)),
			Type(type)
		{}
	};

	struct shortcuts_override_context_t
	{
		std::map<std::string, shortcut_info> BoundedKeys;
	};
	static std::vector<shortcuts_override_context_t> current_override_context{ {} };
	static size_t highest_current_context{ 0 };


	struct key_and_time
	{
		float Time;
		ImGuiKey Key;

		[[nodiscard]]
		constexpr auto operator>(const key_and_time& other) const noexcept
		{
			return Time > other.Time;
		}
	};

	[[nodiscard]]
	static shortcuts::keys_t collect_imgui_active_keys();
	void handle_shortcuts(shortcut_info& keys_info, const shortcuts::keys_t& active_keys);

	void shortcuts::handle_callbacks(size_t context_idx)
	{
#ifndef IMCXX_MISC_NO_SHORTCUT_DEMO
		render_shortcuts_demo();
#endif
		auto& context = current_override_context[context_idx];
		auto active_keys = collect_imgui_active_keys();

		for (auto& iter : context.BoundedKeys)
		{
			shortcut_info& keys_info = iter.second;
			handle_shortcuts(keys_info, active_keys);
			
			if (keys_info.Render)
				keys_info.Callback(&keys_info.Render);
		}
	}

	IMGUI_API size_t shortcuts::push_context() 
	{
		if (highest_current_context < current_override_context.size())
			current_override_context.emplace_back();
		
		return ++highest_current_context;
	}

	IMGUI_API void shortcuts::pop_context()
	{
		IM_ASSERT(highest_current_context != 0);
		--highest_current_context;
	}

	IMGUI_API void shortcuts::bind(const char* id, std::initializer_list<keys_t> trigger_keys, shortcuts_type type, std::function<void(bool*)> callback)
	{
		auto& ctx = current_override_context[highest_current_context];
		auto [keys_info, inserted] = ctx.BoundedKeys.emplace(id, shortcut_info{std::move(callback), type });
		if (inserted)
		{
			auto& keys = keys_info->second.Keys;
			keys.insert(keys.end(), trigger_keys);
		}
	}

	IMGUI_API bool shortcuts::is_bound(const char* id, keys_t trigger_keys)
	{
		const auto& ctx = current_override_context[highest_current_context];
		auto iter = ctx.BoundedKeys.find(id);
		if (iter == ctx.BoundedKeys.end())
			return false;

		if (trigger_keys[0])
		{
			bool found = false;
			for (auto& key : iter->second.Keys)
			{
				for (auto c : trigger_keys)
				{
					if (!c)
						break;
					else if (c == key[0] || c == key[1] || c == key[2])
					{
						found = true;
						break;
					}
				}
				if (found)
					break;
			}
			if (!found)
				return false;
		}

		return true;
	}

	IMGUI_API void shortcuts::unbind(const char* id, keys_t trigger_keys)
	{
		auto& ctx = current_override_context[highest_current_context];
		auto iter = ctx.BoundedKeys.find(id);
		if (iter == ctx.BoundedKeys.end())
			return;
		

		if (trigger_keys[0])
		{
			auto& keys = iter->second.Keys;
			for (auto key_iter = keys.begin(); key_iter != keys.end(); key_iter++)
			{
				if (*key_iter == trigger_keys)
				{
					if (keys.size() == 1)
						ctx.BoundedKeys.erase(iter);
					else
						keys.erase(key_iter);
					break;
				}
			}
		}
		else ctx.BoundedKeys.erase(iter);
	}


	static shortcuts::keys_t collect_imgui_active_keys()
	{
		std::set<key_and_time, std::greater<>> res;
		for (ImGuiKey i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_COUNT; i++)
		{
			if (ImGui::IsKeyDown(i) || ImGui::IsKeyReleased(i))
				res.insert({ ImGui::GetKeyData(i)->DownDuration, i });
		}

		auto iter = res.begin();
		shortcuts::keys_t keys;
		for (size_t i = 0; i < 3; i++)
		{
			if (iter == res.end())
			{
				keys[i] = 0;
			}
			else
			{
				keys[i] = iter->Key;
				if (ImGui::IsKeyReleased(iter->Key))
					iter = res.end();
				else
					++iter;
			}
		}
		return keys;
	}

	void handle_shortcuts(shortcut_info& keys_info, const shortcuts::keys_t& active_keys)
	{
		if (active_keys[0])
		{
			for (const auto& reg_keys : keys_info.Keys)
			{
				if (active_keys == reg_keys)
				{
					if (keys_info.Type == shortcuts_type::bind_persist)
						keys_info.Render = true;
					else
					{
						for (int i = 2; i >= 0; i--)
						{
							if (reg_keys[i] && ImGui::IsKeyReleased(reg_keys[i]))
							{
								keys_info.Render = !keys_info.Render;
								break;
							}
						}
					}
					break;
				}
			}
		}
	}
}