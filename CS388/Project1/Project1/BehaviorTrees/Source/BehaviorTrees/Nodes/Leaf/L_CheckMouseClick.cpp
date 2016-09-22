/******************************************************************************/
/*!
\file		L_CheckMouseClick.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Condition: Check if mouse is clicked.

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
void L_CheckMouseClick::OnInitial(NodeData *nodedata_ptr)
{
	LeafNode::OnInitial(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_CheckMouseClick::OnEnter(NodeData *nodedata_ptr)
{
	//LeafNode::OnEnter(nodedata_ptr);

	UNREFERENCED_PARAMETER(nodedata_ptr);

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void L_CheckMouseClick::OnExit(NodeData *nodedata_ptr)
{
	//LeafNode::OnExit(nodedata_ptr);

	UNREFERENCED_PARAMETER(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_CheckMouseClick::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	//LeafNode::OnUpdate(dt, nodedata_ptr);
	
	// return SUCCESS only when clicking mouse
	// otherwise, run forever

	TinyCustomData *customdata = nodedata_ptr->GetAgentData().GetCustomData<TinyCustomData>();

	if (customdata->m_mouseClick)
		return Status::BT_SUCCESS;

	return Status::BT_FAILURE;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_CheckMouseClick::OnSuspend(NodeData *nodedata_ptr)
{
	return Status::BT_SUSPEND;

	//return LeafNode::OnSuspend(nodedata_ptr);
}