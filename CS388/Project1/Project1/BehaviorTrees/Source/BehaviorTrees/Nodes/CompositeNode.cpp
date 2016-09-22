/******************************************************************************/
/*!
\file		CompositeNode.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Composite node logic base class.

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
void CompositeNode::OnInitial(NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status CompositeNode::OnEnter(NodeData *nodedata_ptr)
{
	ResetChildReturnStatus(nodedata_ptr);

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void CompositeNode::OnExit(NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:		dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status CompositeNode::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	// run all children, return SUCCESS when all of them finish running

	UNREFERENCED_PARAMETER(dt);

	// check if last children finishes running
	if (nodedata_ptr->GetAllChildStatus().back() != Status::BT_READY)
		return Status::BT_SUCCESS;
	else
	{
		Status child_status = 
			nodedata_ptr->GetChildStatus(nodedata_ptr->GetCurrentChildOrder());

		if ((child_status == Status::BT_SUCCESS) || 
			(child_status == Status::BT_FAILURE))
			nodedata_ptr->IncrementCurrentChildOrder();

		RunChild(nodedata_ptr->GetCurrentChildNodeIndex(), 
			true, 
			false, 
			nodedata_ptr);

		return Status::BT_RUNNING;
	}
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:		return status.
*---------------------------------------------------------------------------*/
Status CompositeNode::OnSuspend(NodeData *nodedata_ptr)
{
	return Status::BT_SUSPEND;
}
