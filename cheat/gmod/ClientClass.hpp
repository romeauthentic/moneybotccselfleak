#pragma once
#include "Valve/dt_common.h"
#include "Valve/dt_recv.h"


class ClientClass {
public:
	void* m_create_fn;
	void* m_createevent_fn;
	char* m_name;
	RecvTable* m_rt_table;
	ClientClass* m_next;
	int m_class_id;
};