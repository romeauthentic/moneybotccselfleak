#pragma once

#include "vector.hpp"

struct te_tf_particle_effects_control_point_t {
    int m_eParticleAttachment;
    vec3_t m_vecOffset{};
};

struct te_tf_particle_effects_colors_t {
    vec3_t m_vecColor1;
    vec3_t m_vecColor2;
};

class CEffectData {
public:
    vec3_t m_vOrigin;
    vec3_t m_vStart;
    vec3_t m_vNormal;
    vec3_t m_vAngles;
    int m_fFlags;
    int m_nEntIndex;
    float m_flScale;
    float m_flMagnitude;
    float m_flRadius;
    int m_nAttachmentIndex;
    short m_nSurfaceProp;

    int m_nMaterial;
    int m_nDamageType;
    int m_nHitBox;

    unsigned char m_nColor;

    bool m_bCustomColors;
    te_tf_particle_effects_colors_t m_CustomColors;

    bool m_bControlPoint1;
    te_tf_particle_effects_control_point_t m_ControlPoint1;

public:
    CEffectData( ) {
		m_vOrigin = { 0.f, 0.f, 0.f };
		m_vStart = { 0.f, 0.f, 0.f };
		m_vNormal = { 0.f, 0.f, 0.f };
		m_vAngles = { 0.f, 0.f, 0.f };

        m_fFlags = 0;
        m_nEntIndex = 0;
        m_flScale = 1.f;
        m_nAttachmentIndex = 0;
        m_nSurfaceProp = 0;

        m_flMagnitude = 0.0f;
        m_flRadius = 0.0f;

        m_nMaterial = 0;
        m_nDamageType = 0;
        m_nHitBox = 0;

        m_nColor = 0;

        m_bCustomColors = false;
        m_CustomColors.m_vecColor1 = { 1.f, 1.f, 1.f };
        m_CustomColors.m_vecColor2 = { 1.f, 1.f, 1.f };

        m_bControlPoint1 = false;
        m_ControlPoint1.m_eParticleAttachment = 0;
    }

    int GetEffectNameIndex( ) {
        return m_iEffectName;
    }

private:
    int m_iEffectName;
};

class IEffects {
public:
    virtual ~IEffects( ) {};

    virtual void Beam( const vec3_t &Start, const vec3_t &End, int nModelIndex,
                       int nHaloIndex, unsigned char frameStart, unsigned char frameRate,
                       float flLife, unsigned char width, unsigned char endWidth, unsigned char fadeLength,
                       unsigned char noise, unsigned char red, unsigned char green,
                       unsigned char blue, unsigned char brightness, unsigned char speed ) = 0;

    //-----------------------------------------------------------------------------
    // Purpose: Emits smoke sprites.
    // Input  : origin - Where to emit the sprites.
    //			scale - Sprite scale * 10.
    //			framerate - Framerate at which to animate the smoke sprites.
    //-----------------------------------------------------------------------------
    virtual void Smoke( const vec3_t &origin, int modelIndex, float scale, float framerate ) = 0;

    virtual void Sparks( const vec3_t &position, int nMagnitude = 1, int nTrailLength = 1, const vec3_t *pvecDir = NULL ) = 0;

    virtual void Dust( const vec3_t &pos, const vec3_t &dir, float size, float speed ) = 0;

    virtual void MuzzleFlash( const vec3_t &vecOrigin, const vec3_t &vecAngles, float flScale, int iType ) = 0;

    // like ricochet, but no sound
    virtual void MetalSparks( const vec3_t &position, const vec3_t &direction ) = 0;

    virtual void EnergySplash( const vec3_t &position, const vec3_t &direction, bool bExplosive = false ) = 0;

    virtual void Ricochet( const vec3_t &position, const vec3_t &direction ) = 0;

    // FIXME: Should these methods remain in this interface? Or go in some 
    // other client-server neutral interface?
    virtual float Time( ) = 0;
    virtual bool IsServer( ) = 0;

    // Used by the playback system to suppress sounds
    virtual void SuppressEffectsSounds( bool bSuppress ) = 0;
};
