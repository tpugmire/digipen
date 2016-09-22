/******************************************************************************/
/*!
\file		LeafNode.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Leaf node logic base class.

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
void LeafNode::OnInitial(NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status LeafNode::OnEnter(NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(nodedata_ptr);

	DRAWNODENAME;

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void LeafNode::OnExit(NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(nodedata_ptr);

	DRAWNODENAME;
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status LeafNode::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(dt);
	UNREFERENCED_PARAMETER(nodedata_ptr);

	DRAWNODENAME;

	return Status::BT_SUCCESS;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status LeafNode::OnSuspend(NodeData *nodedata_ptr)
{
	// tiny idle

	UNREFERENCED_PARAMETER(nodedata_ptr);

	DRAWNODENAME;

	return Status::BT_SUSPEND;
}

/*--------------------------------------------------------------------------*
Name:           Verify

Description:    Verify the node.

Arguments:      None.

Returns:        true:	valid node.
false:	invalid node.
*---------------------------------------------------------------------------*/
bool LeafNode::Verify(void)
{
	// by definition leaf node can't have children
	// and it can't be passive node

	if (m_children.empty())
			return true;

	return false;
}
