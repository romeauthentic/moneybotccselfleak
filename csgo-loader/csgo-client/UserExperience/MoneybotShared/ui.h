#pragma once

#include "ui_base_item.h"
#include "ui_menu.h"
#include "ui_form.h"
#include "ui_render.h"
#include "ui_checkbox.h"
#include "ui_tab_manager.h"
#include "ui_slider.h"
#include "ui_dropdown.h"
#include "ui_key_picker.h"
#include "ui_button.h"
#include "ui_color_picker.h"
#include "ui_label.h"
#include "ui_text_input.h"
#include "ui_progressbar.h"

namespace ui {
	auto menu = std::make_shared< ui::c_menu >(0, 0, 500, 400, "moneybot $", "");

	static void render() {
		static bool was_setup = false;
		if(!was_setup) {
			menu = std::make_shared< ui::c_menu >(0, 0, 450, 375, "moneybot $", "");

			//auto login_form = menu->add_item(std::make_shared< ui::c_form >(120, 20, 190, 115, "login")); {
			//	login_form->add_item(std::make_shared< ui::base_item >(0, 0, 0, 3));
			//	login_form->add_item(std::make_shared< ui::c_button >(15, 0, 140, 18, "submit", []() {
			//		/*
			//			execute your code to log in here
			//		*/
			//	}));
			//}

			auto wait_form = menu->add_item(std::make_shared<ui::c_form>(120, 20, 190, 115, "please wait"));
			{
				wait_form->add_item(std::make_shared<ui::c_label>(33, 35, "waiting for response..."));
			}
			wait_form->set_cond([] {
				return UserInterface->m_Data.m_ExecutionState == UserExperience::EXECUTION_WAITING; 
			});

			auto error_form = menu->add_item(std::make_shared<ui::c_form>(120, 20, 190, 115, "error"));
			{
				error_form->add_item(std::make_shared<ui::c_label>(30, 15, "hardware id mismatch."))->set_cond([] {
					return UserInterface->m_Data.m_Error == UserExperience::ERROR_INVALID_HWID;
				});
				error_form->add_item(std::make_shared<ui::c_label>(28, 15, "failed to verify session."))->set_cond([] {
					return UserInterface->m_Data.m_Error == UserExperience::ERROR_GENERIC_ERROR;
				});

				error_form->add_item(std::make_shared<ui::c_button>(15, 20, 140, 20, "okay", [] { 
					ExitProcess(0);
				}));
			}
			error_form->set_cond([] {
				return UserInterface->m_Data.m_ExecutionState == UserExperience::EXECUTION_ERROR;
			});

			auto login_form = menu->add_item(std::make_shared<ui::c_form>(120, 20, 190, 135, "log in"));
			{
				login_form->add_item(std::make_shared< ui::c_text_input >(15, 0, 140, "username:", 128, UserInterface->m_Data.m_Username, false));
				login_form->add_item(std::make_shared< ui::c_text_input >(15, 0, 140, "password:", 128, UserInterface->m_Data.m_Password, true));

				login_form->add_item(std::make_shared<ui::c_label>(15, 0, "warning: caps lock is on!"))->set_cond([] {
					return GetKeyState(VK_CAPITAL);
				});

				login_form->add_item(std::make_shared<ui::base_item>(0, 0, 0, 3))->set_cond([] {
					return !GetKeyState(VK_CAPITAL);
				});

				login_form->add_item(std::make_shared<ui::c_button>(15, 0, 140, 20, "log in", [] {
					UserInterface->m_Data.m_ExecutionState = UserExperience::EXECUTION_WAITING;
				}));
			}
			login_form->set_cond([] {
				return UserInterface->m_Data.m_ExecutionState == UserExperience::EXECUTION_LOG_IN;
			});

			auto choose_form = menu->add_item(std::make_shared<ui::c_form>(120, 20, 190, 115, "select a cheat"));
			{
				// Change these if you want to.
				static std::vector<dropdowns::dropdown_item_t<uint16_t>> m_beta_items{
					{ "cs:go (release)", 0 },
					{ "cs:go (beta)", 1 },
					{ "cs:go (debug)", 2 },
				};

				static std::vector<dropdowns::dropdown_item_t<uint16_t>> m_normal_items{
					{ "cs:go", 0 },
				};

				choose_form->add_item(std::make_shared<ui::c_dropdown<uint16_t>>(15, 0, 140, "cheat selection:", &UserInterface->m_Data.m_SelectedGame, &m_beta_items))->set_cond([] {
					return UserInterface->m_Data.m_SpecialAccess;
				});
				choose_form->add_item(std::make_shared<ui::c_dropdown<uint16_t>>(15, 0, 140, "cheat selection:", &UserInterface->m_Data.m_SelectedGame, &m_normal_items))->set_cond([] {
					return !UserInterface->m_Data.m_SpecialAccess;
				});

				choose_form->add_item(std::make_shared<ui::base_item>(0, 0, 0, 3));

				choose_form->add_item(std::make_shared<ui::c_button>(15, 0, 140, 20, "inject", [] {
					UserInterface->m_Data.m_ExecutionState = UserExperience::EXECUTION_WAITING;
				}));
			}
			choose_form->set_cond([] {
				return UserInterface->m_Data.m_ExecutionState == UserExperience::EXECUTION_CHOOSE;
			});


			was_setup = true;
		}
		else {
			render_item(menu.get());
		}
	}
}