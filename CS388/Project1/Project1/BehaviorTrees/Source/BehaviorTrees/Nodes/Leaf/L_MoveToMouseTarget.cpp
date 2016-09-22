/******************************************************************************/
/*!
\file		L_MoveToMouseTarget.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Action: Move to mouse click point.

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
void L_MoveToMouseTarget::OnInitial(NodeData *nodedata_ptr)
{
	LeafNode::OnInitial(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_MoveToMouseTarget::OnEnter(NodeData *nodedata_ptr)
{
	LeafNode::OnEnter(nodedata_ptr);

	AgentBTData &agentdata = nodedata_ptr->GetAgentData();
	GameObject *self = agentdata.GetGameObject();
	TinyCustomData *customdata = agentdata.GetCustomData<TinyCustomData>();

	if (customdata->m_mouseClick)
	{
		customdata->m_mouseClick = false;

		self->SetTargetPOS(customdata->m_mousePos);
		self->SetSpeedStatus(TinySpeedStatus::TS_JOG);
		SetTinySpeed(self);

		return Status::BT_RUNNING;
	}

	return Status::BT_FAILURE;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void L_MoveToMouseTarget::OnExit(NodeData *nodedata_ptr)
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
Status L_MoveToMouseTarget::OnUpdate(float dt, NodeData *nodedata_ptr)
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
Status L_MoveToMouseTarget::OnSuspend(NodeData *nodedata_ptr)
{
	return LeafNode::OnSuspend(nodedata_ptr);
}