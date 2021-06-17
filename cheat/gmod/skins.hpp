#pragma once

namespace features
{
	class c_skins {
	private:
		int get_knife_index( );
		const char* get_model_str( );

		void override_knife( );
	public:
		void operator()( );
	};
}