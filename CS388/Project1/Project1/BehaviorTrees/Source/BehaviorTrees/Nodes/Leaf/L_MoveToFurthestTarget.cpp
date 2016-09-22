/******************************************************************************/
/*!
\file		L_MoveToFurthestTarget.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Action: Move to the furthest AI agent.

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
void L_MoveToFurthestTarget::OnInitial(NodeData *nodedata_ptr)
{
	LeafNode::OnInitial(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_MoveToFurthestTarget::OnEnter(NodeData *nodedata_ptr)
{
	LeafNode::OnEnter(nodedata_ptr);

	GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();
	GameObject *furthest = GetFarthestAgent(self);

	if (furthest)
	{
		self->SetTargetPOS(furthest->GetBody().GetPos());
		self->SetSpeedStatus(TinySpeedStatus::TS_JOG);
		SetTinySpeed(self);

		return Status::BT_RUNNING;
	}
	else
		return Status::BT_FAILURE;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void L_MoveToFurthestTarget::OnExit(NodeData *nodedata_ptr)
{
	LeafNode::OnExit(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_MoveToFurthestTarget::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	LeafNode::OnUpdate(dt, nodedata_ptr);

	GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();

	if (IsNear(self->GetBody().GetPos(), self->GetTargetPOS()))
		return Status::BT_SUCCESS;

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_MoveToFurthestTarget::OnSuspend(NodeData *nodedata_ptr)
{
	return LeafNode::OnSuspend(nodedata_ptr);
}