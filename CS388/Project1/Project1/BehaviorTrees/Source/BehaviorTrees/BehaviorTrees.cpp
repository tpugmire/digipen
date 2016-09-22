/******************************************************************************/
/*!
\file		BehaviorTrees.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           BehaviorTrees

Description:    Default constructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
BehaviorTrees::BehaviorTrees()
{
}

/*--------------------------------------------------------------------------*
Name:           BehaviorTrees

Description:    Get agent data from name.

Arguments:      agentname:			agent name.

Returns:        AgentBTData&:		agent behavior tree data.
*---------------------------------------------------------------------------*/
AgentBTData &BehaviorTrees::GetAgentData(const std::string &agentname)
{
	int *index_ptr = HashmapFind(m_agentindextable, agentname);

#if defined(_DEBUG)
	MY_BETTER_ASSERT(index_ptr != nullptr, "No agent data");
#endif

	return *m_agentdata[*index_ptr];
}

AgentBTData const &BehaviorTrees::GetAgentData(const std::string &agentname) const
{
	const int *index_ptr = HashmapFind(m_agentindextable, agentname);

#if defined(_DEBUG)
	MY_BETTER_ASSERT(index_ptr != nullptr, "No agent data");
#endif

	return *m_agentdata[*index_ptr];
}

/*--------------------------------------------------------------------------*
Name:           GetTreeLogic

Description:    Get tree logic instance from name.

Arguments:      treename:		behavior tree name.

Returns:        TreeLogic&:		behavior tree logic instance.
*---------------------------------------------------------------------------*/
TreeLogic &BehaviorTrees::GetTreeLogic(const std::string &treename)
{
	int treeindex = 0;
	int *treeindex_ptr = HashmapFind(m_treeindextable, treename);

#if defined(_DEBUG)
	MY_BETTER_ASSERT(treeindex_ptr != nullptr, "No tree logic");
#endif

	treeindex = *treeindex_ptr;

	return GetTreeLogic(treeindex);
}

TreeLogic const &BehaviorTrees::GetTreeLogic(const std::string &treename) const
{
	int treeindex = 0;
	const int *treeindex_ptr = HashmapFind(m_treeindextable, treename);

#if defined(_DEBUG)
	MY_BETTER_ASSERT(treeindex_ptr != nullptr, "No tree logic");
#endif

	treeindex = *treeindex_ptr;

	return GetTreeLogic(treeindex);
}

/*--------------------------------------------------------------------------*
Name:           GetTreeLogic

Description:    Get tree logic instance from index.

Arguments:      treeindex:		behavior tree index.

Returns:        TreeLogic&:		behavior tree logic instance.
*---------------------------------------------------------------------------*/
TreeLogic &BehaviorTrees::GetTreeLogic(int treeindex)
{
#ifdef _DEBUG
	MY_ASSERT_WITH_VALUES(
		treeindex >= 0 && treeindex < static_cast<int>(m_trees.size()),
		"Behavior tree index out of bounds\ntree index: %d\ntree size: %d",
		treeindex, m_trees.size());
#endif

	return m_trees[treeindex];
}

TreeLogic const &BehaviorTrees::GetTreeLogic(int treeindex) const
{
#ifdef _DEBUG
	MY_ASSERT_WITH_VALUES(
		treeindex >= 0 && treeindex < static_cast<int>(m_trees.size()),
		"Behavior tree index out of bounds\ntree index: %d\ntree size: %d",
		treeindex, m_trees.size());
#endif

	return m_trees[treeindex];
}

/*--------------------------------------------------------------------------*
Name:           GetTreeRoot

Description:    Get tree root from tree name.

Arguments:      treename:		behavior tree name.

Returns:        BehaviorNode&:	root behavior node of the behavior tree.
*---------------------------------------------------------------------------*/
BehaviorNode &BehaviorTrees::GetTreeRoot(const std::string &treename)
{
	TreeLogic &logic = GetTreeLogic(treename);

	return logic[0];
}

BehaviorNode const &BehaviorTrees::GetTreeRoot(const std::string &treename) const
{
	const TreeLogic &logic = GetTreeLogic(treename);

	return logic[0];
}

/*--------------------------------------------------------------------------*
Name:           Register

Description:    Register the agent.

Arguments:      agentname:		agent name.
				modelname:		model (behavior tree) name.
				npc:			game object associated to the agent.

Returns:        true:			registration successful.
				false:			fail to register.
*---------------------------------------------------------------------------*/
bool BehaviorTrees::Register(const std::string &agentname, const std::string &modelname)
{
	if (!Register(agentname))
		return false;

	InitializeAgent(agentname, modelname);

	return true;
}

/*--------------------------------------------------------------------------*
Name:           Register

Description:    Register the agent.

Arguments:      agentname:		agent name.

Returns:        true:			registration successful.
				false:			fail to register.
*---------------------------------------------------------------------------*/
bool BehaviorTrees::Register(const std::string &agentname)
{
	// add agent data to array, then update index table

	// agent found, don't register
#if defined(_DEBUG)
	MY_BETTER_ASSERT(
		HashmapFind(m_agentindextable, agentname) == nullptr,
		"Unable to register: Agent already exists");
#else
	if (HashmapFind(m_agentindextable, agentname) != nullptr)
		return false;
#endif

	unsigned agent_index = m_agentdata.size();

	m_agentdata.push_back(std::move(std::make_unique<AgentBTData>(agentname)));
	m_agentindextable.insert(std::make_pair(agentname, agent_index));

	return true;
}

/*--------------------------------------------------------------------------*
Name:           Unregister

Description:    Unregister the agent.

Arguments:      agentname:		agent name.

Returns:        true:			unregistration successful.
				false:			fail to unregister.
*---------------------------------------------------------------------------*/
bool BehaviorTrees::Unregister(const std::string &agentname)
{
	// remove agent data, then update index table

	int agent_index = 0;

	// if agent isn't found, stop, otherwise record index to data array
	int *agent_index_ptr = HashmapFind(m_agentindextable, agentname);

#if defined(_DEBUG)
	MY_BETTER_ASSERT(
		HashmapFind(m_agentindextable, agentname) != nullptr,
		"Unable to unregister: agent doesn't exists");
#else
	if (agent_index_ptr == nullptr)
		return false;
#endif

	agent_index = *agent_index_ptr;

	// remove it from database
	m_agentdata[agent_index]->MarkForDeletion();

	// get index of the last element
	// if there's only one item left in data array, we can simply clear it
	// if the item to be removed is the last one in data array, we can just 
	// remove it, otherwise, we copy the last element to its position, 
	// remove the agent from index table then update the index of the last 
	// element

	int last_index = m_agentdata.size() - 1;
	if (last_index <= 0)
	{
		m_agentdata.clear();
		m_agentindextable.clear();
	}

	if (agent_index != last_index)
	{
		std::string last_agent = m_agentdata[last_index]->GetName();

		m_agentdata[agent_index] = std::move(m_agentdata[last_index]);
		m_agentindextable.erase(last_agent);
		m_agentindextable.insert(std::make_pair(last_agent, agent_index));
	}

	m_agentdata.pop_back();
	m_agentindextable.erase(agentname);

	return true;
}

/*--------------------------------------------------------------------------*
Name:           InitializeAgent

Description:    Initialize model (behavior tree) for agent.

Arguments:      agentname:		agent name.
				modelname:		model (behavior tree) name.

Returns:        None.
*---------------------------------------------------------------------------*/
void BehaviorTrees::InitializeAgent(const std::string &agentname, const std::string &modelname)
{
	AgentBTData &agentdata = GetAgentData(agentname);

	agentdata.InitialNodeData(modelname);
	agentdata.Enable();
}

/*--------------------------------------------------------------------------*
Name:           Initialize

Description:    Initialize behavior trees class.

Arguments:      None.

Returns:        true:		finish initialization.
*---------------------------------------------------------------------------*/
bool BehaviorTrees::Initialize(void)
{
	// load behavior trees from file
	LoadTrees();

	return AIReasoner::Initialize();
}

/*--------------------------------------------------------------------------*
Name:           Update

Description:    Execute each agent's behavior tree.

Arguments:      dt:		delta time.

Returns:        None.
*---------------------------------------------------------------------------*/
void BehaviorTrees::Update(float dt)
{
	for (unsigned i = 0; i < m_agentdata.size(); ++i)
	{
		m_agentdata[i]->Execute(dt);
	}
}

/* private methods */

/*--------------------------------------------------------------------------*
Name:           LoadTrees

Description:    Load trees from file.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void BehaviorTrees::LoadTrees(void)
{
	// use treestack to build tree
	std::stack<unsigned> treestack;
	TreeLogic *logic_ptr = nullptr;

#define TREENAME(Name)					treestack = std::stack<unsigned>(); \
										CreateTreeLogic(#Name); \
										logic_ptr = &m_trees.back();
#define TREENODE(Node, Depth)			logic_ptr->AddLogic<class Node>(Depth, "", &treestack);
#include "../../BTResources/Trees.def"
#undef TREENAME
#undef TREENODE

}

/*--------------------------------------------------------------------------*
Name:           CreateTreeLogic

Description:    Create tree logic.

Arguments:      treename:		behavior tree name.

Returns:        None.
*---------------------------------------------------------------------------*/
void BehaviorTrees::CreateTreeLogic(const std::string &treename)
{
	int index = m_trees.size();

	m_trees.push_back(std::move(TreeLogic(treename)));

	m_treeindextable.insert({ treename, index });
}
