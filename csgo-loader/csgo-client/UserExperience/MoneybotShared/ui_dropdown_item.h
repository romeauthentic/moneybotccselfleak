#pragma once

#include "ui_base_item.h"

namespace ui
{
	namespace dropdowns
	{
		template < typename t = int >
		struct dropdown_item_t {
			const char* m_name;
			t			m_value;
		};
	}
}