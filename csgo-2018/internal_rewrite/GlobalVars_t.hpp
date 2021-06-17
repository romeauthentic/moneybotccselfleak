#pragma once

class global_vars_t {
public:
	float m_realtime;
	int m_framecount;
	float m_absframetime;
private:
	float m_absoluteframestarttimestddev;
public:
	float m_curtime;
	float m_frametime;
	int m_maxclients;
	int m_tickcount;
	float m_interval_per_tick;
	float m_interpolation_amount;
	int m_sim_ticks;
	int m_network_protocol;
private:
	char pad00[ 14 ];
};