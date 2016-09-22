/******************************************************************************/
/*!
\file		TinyCustomData.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Custom data for tiny.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// store tiny info
	struct TinyCustomData : public AgentAbstractData
	{
		/* variable */

		GameObject *m_npc;				// agent's game object pointer

		// for mouse click only, change later
		bool m_mouseClick;
		D3DXVECTOR3 m_mousePos;

		/* constructors/destructor */

		// Default constructor
		TinyCustomData();

		/* methods */

		// Handle messages
		virtual void OnMessage(void) override;
	};
}