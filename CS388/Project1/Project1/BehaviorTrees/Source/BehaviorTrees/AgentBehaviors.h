/******************************************************************************/
/*!
\file		AgentBehaviors.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Agent behaviors functions.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// Get furthest agent from npc.
	GameObject *GetFarthestAgent(GameObject *npc);

	// Check if the target is near the position.
	bool IsNear(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &target, float nearDist = 1.0f / 100.0f);

	// Return random position.
	D3DXVECTOR3 RandomPosition(void);

	// Set tiny speed.
	void SetTinySpeed(GameObject *npc);
}