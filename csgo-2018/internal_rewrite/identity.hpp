#pragma once
#include "sdk.hpp"

namespace features
{
	class c_identity {
	private:
		void clantag_changer( );
		void name_changer( );
	public:
		void operator()(  ) {
			clantag_changer( );
			name_changer( );
		}
	};
}