/******************************************************************************/
/*!
\file		L_Idle.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Action: Idle for 1 to 2 seconds.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// node data for L_Idle
	struct L_IdleData : public NodeAbstractData
	{
		float m_timer;		// countdown timer for idle status
	};

	// selector node
	class L_Idle : public LeafNode
	{
	public:
		// Get custom data.
		L_IdleData *GetCustomData(NodeData *nodedata_ptr);

		// Initial custom data.
		void InitialCustomData(NodeData *nodedata_ptr);

		// Set random time on timer.
		void SetRandomTimer(NodeData *nodedata_ptr, float min = 1.0f, float max = 2.0f);

		// If time is up.
		bool IsTimeUp(float dt, NodeData *nodedata_ptr);

	protected:
		// Only run when initializng the node
		virtual void OnInitial(NodeData *nodedata_ptr) override;
		// Only run when entering the node
		virtual Status OnEnter(NodeData *nodedata_ptr) override;
		// Only run when exiting the node
		virtual void OnExit(NodeData *nodedata_ptr) override;
		// Run every frame
		virtual Status OnUpdate(float dt, NodeData *nodedata_ptr) override;
		// Only run when node is in suspended
		virtual Status OnSuspend(NodeData *nodedata_ptr) override;
	};
}