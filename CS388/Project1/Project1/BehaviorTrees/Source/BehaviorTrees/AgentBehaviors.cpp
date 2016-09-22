/******************************************************************************/
/*!
\file		AgentBehaviors.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Agent behaviors functions.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           GetFarthestAgent

Description:    Get furthest agent from npc.

Arguments:      npc:			npc object.

Returns:        GameObject*:	pointer of the object of furthest agent.
*---------------------------------------------------------------------------*/
GameObject *BT::GetFarthestAgent(GameObject *npc)
{
	float farthestDistance = 0.0f;
	GameObject *farthestGameObject = nullptr;
	objectID npc_id = npc->GetID();
	dbCompositionList list;
	g_database.ComposeList(list, OBJECT_NPC);

	dbCompositionList::iterator i;
	for (i = list.begin(); i != list.end(); ++i)
	{
		if ((*i)->GetID() != npc_id)
		{
			D3DXVECTOR3 npcPos = (*i)->GetBody().GetPos();
			D3DXVECTOR3 myPos = npc->GetBody().GetPos();
			D3DXVECTOR3 diff = npcPos - myPos;
			float distance = D3DXVec3Length(&diff);

			if (farthestGameObject)
			{
				if (distance > farthestDistance)
				{
					farthestDistance = distance;
					farthestGameObject = *i;
				}
			}
			else
			{
				farthestDistance = distance;
				farthestGameObject = *i;
			}
		}
	}

	return farthestGameObject;
}

/*--------------------------------------------------------------------------*
Name:           IsNear

Description:    Check if the target is near the position.

Arguments:      pos:		current position.
				target:		target position.
				nearDist:	how much distance is considered as near.

Returns:        true:		current position is near target.
				false:		current position is not near target.
*---------------------------------------------------------------------------*/
bool BT::IsNear(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &target, float nearDist)
{
	return
		(abs(pos.x - target.x) < nearDist) &&
		(abs(pos.y - target.y) < nearDist) &&
		(abs(pos.z - target.z) < nearDist);
}

/*--------------------------------------------------------------------------*
Name:           RandomPosition

Description:    Return random position.

Arguments:      None.

Returns:        D3DXVECTOR3:	random position.
*---------------------------------------------------------------------------*/
// Return random position.
D3DXVECTOR3 BT::RandomPosition(void)
{
	D3DXVECTOR3 target(0, 0, 0);

	target.x = g_random.RangeFloat();
	target.z = g_random.RangeFloat();

	return target;
}

/*--------------------------------------------------------------------------*
Name:           SetTinySpeed

Description:    Set tiny speed.

Arguments:      npc:		tiny game object.

Returns:        None.
*---------------------------------------------------------------------------*/
void BT::SetTinySpeed(GameObject *npc)
{
	switch (npc->GetSpeedStatus())
	{
	case TinySpeedStatus::TS_JOG:
	{
		npc->GetMovement().SetJogSpeed();
		npc->GetMovement().SetTarget(npc->GetTargetPOS());
	}
		break;

	case TinySpeedStatus::TS_WALK:
	{
		npc->GetMovement().SetWalkSpeed();
		npc->GetMovement().SetTarget(npc->GetTargetPOS());
	}
		break;

	default:
	case TinySpeedStatus::TS_IDLE:
	{
		npc->GetMovement().SetIdleSpeed();
	}

	}
}
