#include <UserExperience/UserInterface.hpp>

#include <UserExperience/MoneybotShared/d3d_sprite.hpp>
#include <UserExperience/MoneybotShared/window.hpp>
#include <UserExperience/MoneybotShared/ui.h>

// Global accessor for the user interface.
UserExperience::UserInterfacePtr UserInterface = std::make_unique<UserExperience::UserInterface>();

namespace UserExperience
{
	void OnDirectXFrame()
	{
		if(g_d3d.run_frame(g_window.m_d3d_device))
		{
			g_d3d.begin();
			for(auto& it : d3d::sprites)
			{
				it->begin(g_window.m_d3d_device);
			}

			static auto last_time = GetTickCount() * 0.001f;
			auto cur_time = GetTickCount() * 0.001f;

			auto deltatime = cur_time - last_time;

			last_time = cur_time;

			constexpr float anim_step = 1.0f / 15.f;
			static float anim_time = 0.f;
			static bool flip = false;
			if(anim_time == 1.0f)
			{
				flip = true;
			}
			if(anim_time == 0.f)
			{
				flip = false;
			}

			if(flip) anim_time = std::clamp(anim_time - anim_step * deltatime, 0.f, 1.0f);
			else anim_time = std::clamp(anim_time + anim_step * deltatime, 0.f, 1.0f);

			ui::set_animtime(anim_time);
			ui::render();

			RECT cur_rect{ };
			GetWindowRect(g_window.get_hwnd(), &cur_rect);

			g_d3d.end();
			for(auto& it : d3d::sprites)
			{
				it->end();
			}
		}
	}

	bool UserInterface::Start()
	{
		bool result = g_window.create();

		if(result)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			g_window.add_on_frame(&OnDirectXFrame);
		}

		m_Data.m_Ready = result;

		return result;
	}

	void UserInterface::RunUiFrame() { g_window.on_frame(); }
	HWND UserInterface::GetWindow()  { return g_window.get_hwnd(); }
}