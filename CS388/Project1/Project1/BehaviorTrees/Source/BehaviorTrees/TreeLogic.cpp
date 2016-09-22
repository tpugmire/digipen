/******************************************************************************/
/*!
\file		TreeLogic.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree logic class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/*--------------------------------------------------------------------------*
Name:           TreeLogic

Description:    Constructor.

Arguments:      name:	name of the behavior tree.

Returns:        None.
*---------------------------------------------------------------------------*/
TreeLogic::TreeLogic(const std::string &name)
	: m_name(name)
{
}

/*--------------------------------------------------------------------------*
Name:           TreeLogic

Description:    Move constructor.

Arguments:      logic:	behavior tree being moved.

Returns:        None.
*---------------------------------------------------------------------------*/
TreeLogic::TreeLogic(TreeLogic &&logic)
	: m_name(logic.m_name),
	m_summary(logic.m_summary),
	m_logics(std::move(logic.m_logics))
{
}

/*--------------------------------------------------------------------------*
Name:           operator=

Description:    Move operator.

Arguments:      logic:			behavior tree logic.

Returns:        TreeLogic&:		reference of the behavior tree logic.
*---------------------------------------------------------------------------*/
TreeLogic &TreeLogic::operator=(TreeLogic &&logic)
{
	m_name = logic.m_name;
	m_summary = logic.m_summary;
	m_logics = std::move(logic.m_logics);

	return *this;
}

/*--------------------------------------------------------------------------*
Name:           GenerateDefaultDataArray

Description:    Generate default data array.

Arguments:      nodedata_ptr:		pointer of the NodeData array to fill data.

Returns:        None.
*---------------------------------------------------------------------------*/
void TreeLogic::GenerateDefaultDataArray(std::vector<NodeData> *nodedata_ptr)
{
	for (unsigned i = 0; i < m_logics.size(); ++i)
	{
		nodedata_ptr->push_back(NodeData(GetNode(i)));
	}
}