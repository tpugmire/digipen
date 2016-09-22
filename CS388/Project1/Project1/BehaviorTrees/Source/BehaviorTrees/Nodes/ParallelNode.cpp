/******************************************************************************/
/*!
\file		ParallelNode.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Parallel node logic base class.

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
void ParallelNode::OnInitial(NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status ParallelNode::OnEnter(NodeData *nodedata_ptr)
{
	// push all children onto node stacks

	ResetChildReturnStatus(nodedata_ptr);

	for (unsigned i = 0; i < m_children.size(); ++i)
		RunChild(m_children[i], false, true, nodedata_ptr);

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void ParallelNode::OnExit(NodeData *nodedata_ptr)
{
	// remove all children from stacks

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
Status ParallelNode::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(dt);

	bool allfinished = true;

	// run all children until they return success or failure
	for (unsigned i = 0; i < m_children.size(); ++i)
	{
		Status childstatus = nodedata_ptr->GetChildStatus(i);

		if ((childstatus != Status::BT_SUCCESS) && 
			(childstatus != Status::BT_FAILURE))
		{
			allfinished = false;

			break;
		}
	}

	if (allfinished)
		return Status::BT_SUCCESS;
	else
		return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status ParallelNode::OnSuspend(NodeData *nodedata_ptr)
{
	// tiny idle

	AgentBTData &data = nodedata_ptr->GetAgentData();
	GameObject *self = data.GetGameObject();
	if (self)
		self->SetSpeedStatus(TinySpeedStatus::TS_IDLE);

	return Status::BT_SUSPEND;
}