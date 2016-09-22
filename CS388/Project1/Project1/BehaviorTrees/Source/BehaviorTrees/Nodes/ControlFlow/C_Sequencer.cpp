/******************************************************************************/
/*!
\file		C_Sequencer.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Sequencer node logic.

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
void C_Sequencer::OnInitial(NodeData *nodedata_ptr)
{
	CompositeNode::OnInitial(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_Sequencer::OnEnter(NodeData *nodedata_ptr)
{
	return CompositeNode::OnEnter(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void C_Sequencer::OnExit(NodeData *nodedata_ptr)
{
	CompositeNode::OnExit(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_Sequencer::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(dt);

	// run all children, if any of them returns FAILURE, return FAILURE
	// return SUCCESS when all children return SUCCESS

	// check if last children finishes running
	if (nodedata_ptr->GetAllChildStatus().back() == Status::BT_SUCCESS)
		return Status::BT_SUCCESS;
	else
	{
		Status child_status = 
			nodedata_ptr->GetChildStatus(nodedata_ptr->GetCurrentChildOrder());

		if (child_status == Status::BT_SUCCESS)
			nodedata_ptr->IncrementCurrentChildOrder();
		else if (child_status == Status::BT_FAILURE)
			return Status::BT_FAILURE;

		RunChild(nodedata_ptr->GetCurrentChildNodeIndex(), true, false, nodedata_ptr);

		return Status::BT_RUNNING;
	}
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_Sequencer::OnSuspend(NodeData *nodedata_ptr)
{
	return CompositeNode::OnSuspend(nodedata_ptr);
}