#ifndef XPARGROUP_H
#define XPARGROUP_H

#include "xPar.h"
#include "xParCmd.h"

class xParGroup
{
private:
	xPar* m_root;
	xPar* m_dead;
	int32 m_num_of_particles;
	bool m_alive;
	bool m_killWhenDead;
	bool m_active;
	bool m_visible;
	bool m_culled;
	bool m_priority;
	bool m_flags;
	bool m_regidx;
	xParGroup* m_next;
	xParGroup* m_prev;
	void(*draw)(void*, xParGroup*);
	xParCmdTex* m_cmdTex;
};

#endif