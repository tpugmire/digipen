/******************************************************************************/
/*!
\file		ControlFlowNode.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Control flow node logic base class.

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
bool ControlFlowNode::Verify(void)
{
	// by definition composite node has to have at least 1 child
	// but there's no reason to only have 1

	if (m_children.size() < 2)
		return false;

	return true;
}

/*--------------------------------------------------------------------------*
Name:           ResetChildReturnStatus

Description:    Reset all children return status.

Arguments:      nodedata_ptr:		current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void ControlFlowNode::ResetChildReturnStatus(NodeData *nodedata_ptr)
{
	if (nodedata_ptr->GetAllChildStatus().empty())
	{
		for (unsigned i = 0; i < m_children.size(); ++i)
			nodedata_ptr->GetAllChildStatus().push_back(Status::BT_READY);
	}
	else
	{
		for (unsigned i = 0; i < nodedata_ptr->GetAllChildStatus().size(); ++i)
			nodedata_ptr->SetChildStatus(i, Status::BT_READY);
	}
}

/*--------------------------------------------------------------------------*
Name:           RunChild

Description:    Run child (put it on stack, and return the stack number).

Arguments:      nodedata:		current node data.
				child_index:	child index.
				on_same_stack:	flag: if child is pushed to the same stack.
				stay_stack:		flag: if child stays on stack 
									  after it returns SUCCESS or FAILURE.

Returns:        unsigned:		stack number the child node is pushed on.
*---------------------------------------------------------------------------*/
unsigned ControlFlowNode::RunChild(
	int child_index, bool on_same_stack, bool stay_stack, NodeData *nodedata_ptr)
{
	AgentBTData &agentdata = nodedata_ptr->GetAgentData();

	return agentdata.AddChildToExecutionList(
		nodedata_ptr->GetNodeLogic().GetIndex(), 
		child_index, 
		on_same_stack, 
		stay_stack);
}
