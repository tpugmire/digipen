/******************************************************************************/
/*!
\file		Decorator.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Decorator node logic base class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* protected methods */

/*--------------------------------------------------------------------------*
Name:           Verify

Description:    Verify the node.

Arguments:      None.

Returns:        true:	valid node.
				false:	invalid node.
*---------------------------------------------------------------------------*/
bool DecoratorNode::Verify(void)
{
	// by definition decorator node only has 1 child

	if (m_children.size() == 1)
		return true;

	return false;
}

/*--------------------------------------------------------------------------*
Name:           ResetChildReturnStatus

Description:    Reset child return status.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void DecoratorNode::ResetChildReturnStatus(NodeData *nodedata_ptr)
{
	nodedata_ptr->SetChildStatus(0, Status::BT_READY);
}

/*--------------------------------------------------------------------------*
Name:           RunChild

Description:    Run child (put it on stack, and return the stack number).

Arguments:      on_same_stack:		flag: if child is pushed to the same stack.
				stay_stack:			flag: if child stays on stack after 
									      it returns SUCCESS or FAILURE.
				nodedata_ptr:		current node data.

Returns:        unsigned:		stack number the child node is pushed on.
*---------------------------------------------------------------------------*/
unsigned DecoratorNode::RunChild(bool on_same_stack, bool stay_stack, NodeData *nodedata_ptr)
{
	// for decorator node, push child onto the next empty stack
	// (so every frame it runs both decorator and child)

	AgentBTData &agentdata = nodedata_ptr->GetAgentData();

	return agentdata.AddChildToExecutionList(
		nodedata_ptr->GetNodeLogic().GetIndex(), 
		m_children[0], 
		on_same_stack, 
		stay_stack);
}