/******************************************************************************/
/*!
\file		ControlFlowNode.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Control flow node logic base class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// control flow node
	class ControlFlowNode : public BehaviorNode
	{
	protected:

		/* methods */

		// Only run when initializng the node
		virtual void OnInitial(NodeData *nodedata_ptr) = 0;
		// only run when entering the node
		virtual Status OnEnter(NodeData *nodedata_ptr) = 0;
		// only run when exiting the node
		virtual void OnExit(NodeData *nodedata_ptr) = 0;
		// run every frame
		virtual Status OnUpdate(float dt, NodeData *nodedata_ptr) = 0;
		// Only run when node is in suspended
		virtual Status OnSuspend(NodeData *nodedata_ptr) = 0;

		// Verify the node
		virtual bool Verify(void) override;

		// Reset child return status
		void ResetChildReturnStatus(NodeData *nodedata_ptr);

		// Run child (put it on stack, and return the stack number)
		unsigned RunChild(int child_index, bool on_same_stack, bool stay_stack, NodeData *nodedata_ptr);
	};
}