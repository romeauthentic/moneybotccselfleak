#pragma once
#include "util.hpp"

class IVRenderView {
public:
	void SetBlend( float blend ) {
		return util::get_vfunc< 4, void >( this, blend );
	}

	float GetBlend( ) {
		return util::get_vfunc< 5, float >( this );
	}

	void SetColorModulation( float const* blend ) {
		return util::get_vfunc< 6, void >( this, blend );
	}

	void GetColorModulation( float* blend ) {
		return util::get_vfunc< 7, void >( this, blend );
	}
};