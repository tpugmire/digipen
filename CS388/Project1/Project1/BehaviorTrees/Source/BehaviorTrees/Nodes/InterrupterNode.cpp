/******************************************************************************/
/*!
\file		InterrupterNode.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Interrupter node logic base class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* protected methods */

/*--------------------------------------------------------------------------*
Name:           OnInitial

Description:    Only run when initializng the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void InterrupterNode::OnInitial(NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status InterrupterNode::OnEnter(NodeData *nodedata_ptr)
{
	ResetChildReturnStatus(nodedata_ptr);
	RunChild(false, false, nodedata_ptr);

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void InterrupterNode::OnExit(NodeData *nodedata_ptr)
{
	// remove child from stacks

	AgentBTData &agent = nodedata_ptr->GetAgentData();

	agent.RemoveChildrenFromExecutionList(nodedata_ptr->GetNodeLogic().GetIndex());
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status InterrupterNode::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(dt);

	return nodedata_ptr->GetChildStatus(0);
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status InterrupterNode::OnSuspend(NodeData *nodedata_ptr)
{
	Status child_status = nodedata_ptr->GetChildStatus(0);

	if ((child_status == BT_SUCCESS) || (child_status == BT_FAILURE))
		return child_status;

	return Status::BT_SUSPEND;
}
