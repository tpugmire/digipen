/******************************************************************************/
/*!
\file		AgentBTData.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree data for each agent.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           AgentBTData

Description:    Constructor.

Arguments:      agentname:	agent name.

Returns:        None.
*---------------------------------------------------------------------------*/
AgentBTData::AgentBTData(const std::string &agentname)
	: m_name(agentname)
{
}

/*--------------------------------------------------------------------------*
Name:           AgentBTData

Description:    Move constructor (class with unique_ptr members).

Arguments:      agentdata:		behavior tree data to be moved.

Returns:        None.
*---------------------------------------------------------------------------*/
AgentBTData::AgentBTData(AgentBTData &&agentdata)
	: m_name(agentdata.m_name), 
	m_nodedata(std::move(agentdata.m_nodedata)),
	m_executionlist(agentdata.m_executionlist), 
	m_customdata(std::move(agentdata.m_customdata))
{
}

/*--------------------------------------------------------------------------*
Name:           GetNodeData

Description:    Get behavior node data from behavior tree node index.

Arguments:      node_index:	behavior tree node index.

Returns:        NodeData&:	reference of the behavior node data.
*---------------------------------------------------------------------------*/
NodeData &AgentBTData::GetNodeData(int node_index)
{
#ifdef _DEBUG
	MY_ASSERT_WITH_VALUES(
		node_index >= 0 && node_index < static_cast<int>(m_nodedata.size()),
		"Node data index out of bounds\nnode index: %d\nnodedata size: %d",
		node_index, m_nodedata.size());
#endif

	return m_nodedata[node_index];
}

NodeData const &AgentBTData::GetNodeData(int node_index) const
{
#ifdef _DEBUG
	MY_ASSERT_WITH_VALUES(
		node_index >= 0 && node_index < static_cast<int>(m_nodedata.size()),
		"Node data index out of bounds\nnode index: %d\nnodedata size: %d",
		node_index, m_nodedata.size());
#endif

	return m_nodedata[node_index];
}

/*--------------------------------------------------------------------------*
Name:           GetAgentAbstractData

Description:    Get user defined data (AgentAbstractData).

Arguments:      None.

Returns:        AgentAbstractData*:		Pointer of user defined data.
*---------------------------------------------------------------------------*/
AgentAbstractData *AgentBTData::GetAgentAbstractData(void)
{
	if (!m_customdata)
	{
		InitialAgentAbstractData();
	}

	return static_cast<AgentAbstractData *>(m_customdata.get());
}

/*--------------------------------------------------------------------------*
Name:           GetGameObject

Description:    Get game object pointer (TinyCustomData).

Arguments:      None.

Returns:        GameObject*:	game object associated to agent.
*---------------------------------------------------------------------------*/
GameObject *AgentBTData::GetGameObject(void)
{
	TinyCustomData *customdata = static_cast<TinyCustomData *>(m_customdata.get());

	if (customdata)
		return customdata->m_npc;
	else
		return nullptr;
}

/*--------------------------------------------------------------------------*
Name:           InitialAgentAbstractData

Description:    Initialize user defined data (AgentAbstractData).

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::InitialAgentAbstractData(void)
{
	// initialize custom data for TinyCustomData class
	if (!m_customdata)
		InitialCustomData<AgentAbstractData>();
}

/*--------------------------------------------------------------------------*
Name:           TinyCustomData

Description:    Initialize user defined data (TinyCustomData).

Arguments:      npc:			game object associated to agent.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::InitialTinyCustomData(GameObject *npc)
{
	// initialize custom data for TinyCustomData class
	InitialCustomData<TinyCustomData>();
	TinyCustomData *customdata = GetCustomData<TinyCustomData>();

	customdata->m_npc = npc;
	npc->SetHasBehaviorTreeFlag(true);
}

/*--------------------------------------------------------------------------*
Name:           operator=

Description:    Move operator (class with unique_ptr members).

Arguments:      agentdata:		behavior node data.

Returns:        AgentBTData&:	reference of the behavior node data.
*---------------------------------------------------------------------------*/
AgentBTData &AgentBTData::operator=(AgentBTData &&agentdata)
{
	m_name = agentdata.m_name;
	m_nodedata = std::move(agentdata.m_nodedata);
	m_executionlist = agentdata.m_executionlist;
	m_customdata = std::move(agentdata.m_customdata);

	return *this;
}

/*--------------------------------------------------------------------------*
Name:           InitialNodeData

Description:    Initialize behavior node data.

Arguments:      treename:		behavior node data.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::InitialNodeData(const std::string &treename)
{
	// copy default node data
	// then assign agentdata pointer and run Initialize()

	TreeLogic &logic = g_trees.GetTreeLogic(treename);

	// generate m_nodedata
	m_nodedata.clear();
	logic.GenerateDefaultDataArray(&m_nodedata);

	for (unsigned i = 0; i < m_nodedata.size(); ++i)
	{
		m_nodedata[i].SetAgentData(this);
		m_nodedata[i].GetNodeLogic().Initialize(&m_nodedata[i]);
	}

	BehaviorTrees &trees = g_trees;
}

/*--------------------------------------------------------------------------*
Name:           Enable

Description:    Enable (Reset execution list).

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::Enable(void)
{
	// clear execution list
	// then push root onto the first node stack of execution list

	ClearExecutionList();
	GrowNodeStack();
	PushNodeToExecutionList(0, 0, true);
}

/*--------------------------------------------------------------------------*
Name:           Disable

Description:    Disable (Clear execution list).

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::Disable(void)
{
	// clear execution list

	ClearExecutionList();
}

/*--------------------------------------------------------------------------*
Name:           MarkForDeletion

Description:    Mark the agent for deletion.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::MarkForDeletion(void)
{
	GameObject *npc = GetGameObject();

	if (npc)
		npc->MarkForDeletion();
}

/*--------------------------------------------------------------------------*
Name:           AddChildToExecutionList

Description:    Add child to execution list,
				and return the stack index of the child.

Arguments:      node_index:			current node index.
				child_index:		child node index.
				on_same_stack:		flag: if the child is on the same stack.
				stay_stack:			flag: if the child is staying on stack.

Returns:        unsigned:			stack index of the child.
*---------------------------------------------------------------------------*/
unsigned AgentBTData::AddChildToExecutionList(int node_index, 
	int child_index, bool on_same_stack, bool stay_stack)
{
#ifdef _DEBUG
	MY_ASSERT_WITH_VALUES(
		node_index >= 0 && node_index < static_cast<int>(m_nodedata.size()),
		"Node data index out of bounds\nnode index: %d\nnodedata size: %d",
		node_index, m_nodedata.size());
#endif
	
	unsigned stack_index = m_nodedata[node_index].GetStackIndex();

	// if the child is on the same stack, push to it
	// if not, then we need to find the new empty stack index, then push child to it

	if (on_same_stack)
	{
		PushNodeToExecutionList(child_index, stack_index, stay_stack);

		return stack_index;
	}
	else
	{
		unsigned child_stackindex = FindNextEmptyNodeStack(stack_index);

		PushNodeToExecutionList(child_index, child_stackindex, stay_stack);

		return child_stackindex;
	}
}

/*--------------------------------------------------------------------------*
Name:           RemoveChildrenFromExecutionList

Description:    Remove all children of the node from execution list.

Arguments:      node_index:		parent node.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::RemoveChildrenFromExecutionList(int node_index)
{
	BehaviorNode &btnode = m_nodedata[node_index].GetNodeLogic();

	// remove children from stacks
	for (unsigned i = 0; i < btnode.GetChildren().size(); ++i)
		ClearNodeStack(GetNodeData(btnode.GetChildNodeIndex(i)).GetStackIndex());
}

/*---------------------------------------------------------------------------*
Name:			SendMsg

Description:	Send a message from agent to a game object.

Arguments:		id:		the ID of the object.
				name:	the name of the message.
				data:	data to be delivered with the message.

Returns:i		None.
*---------------------------------------------------------------------------*/
void AgentBTData::SendMsg(objectID id, MSG_Name name, MSG_Data& data)
{
	GameObject* object = g_database.Find(id);

	if (object)
	{
		objectID self_id = GetGameObject()->GetID();

		MSG_Object msg(0.0f, name, self_id, id, SCOPE_TO_STATE_MACHINE, 0, STATE_MACHINE_QUEUE_ALL, data, false, false);

		if (object->GetStateMachineManager())
		{
			object->GetStateMachineManager()->SendMsg(msg);
		}
		// added for Behavior tree (Chi-Hao) START
		else
		{
			g_trees.GetAgentData(object->GetName()).ReceiveMsg(msg);
		}
		// added for Behavior tree (Chi-Hao) END
	}
}

/*---------------------------------------------------------------------------*
Name:			SendMsg

Description:	Send a message from agent to a game object.

Arguments:		object:		pointer to the game object.
				name:		the name of the message.
				data:		data to be delivered with the message.

Returns:		None.
*---------------------------------------------------------------------------*/
void AgentBTData::SendMsg(GameObject* object, MSG_Name name, MSG_Data& data)
{
	if (object)
	{
		objectID self_id = GetGameObject()->GetID();

		MSG_Object msg(0.0f, name, self_id, object->GetID(), SCOPE_TO_STATE_MACHINE, 0, STATE_MACHINE_QUEUE_ALL, data, false, false);

		if (object->HasStateMachine())
		{
			object->GetStateMachineManager()->SendMsg(msg);
		}
		// added for Behavior tree (Chi-Hao) START
		else
		{
			g_trees.GetAgentData(object->GetName()).ReceiveMsg(msg);
		}
		// added for Behavior tree (Chi-Hao) END
	}
}

/*---------------------------------------------------------------------------*
Name:			SendMsgToAll

Description:	Send a message from agent to all game objects.

Arguments:		name:		the name of the message
				data:		data to be delivered with the message

Returns:		None.
*---------------------------------------------------------------------------*/
void AgentBTData::SendMsgToAll(MSG_Name name, MSG_Data& data)
{
	objectID self_id = GetGameObject()->GetID();
	AgentBTDataList &agentlist = g_trees.GetAllAgentsBTData();

	for (AgentBTDataList::iterator it = agentlist.begin(); it != agentlist.end(); ++it)
	{
		GameObject *receiver_obj = (*it)->GetGameObject();
		objectID receiver_id = receiver_obj->GetID();

		// don't send to itself
		if (receiver_id == self_id)
			continue;

		MSG_Object msg(0.0f, name, self_id, receiver_id, SCOPE_TO_STATE_MACHINE, 0, STATE_MACHINE_QUEUE_ALL, data, false, false);

		if (receiver_obj->HasStateMachine())
		{
			receiver_obj->GetStateMachineManager()->SendMsg(msg);
		}
		// added for Behavior tree (Chi-Hao) START
		else
		{
			(*it)->ReceiveMsg(msg);
		}
		// added for Behavior tree (Chi-Hao) END
	}
}

/*--------------------------------------------------------------------------*
Name:           ReceiveMsg

Description:    Receive message and put it into queue.

Arguments:      msg:		message object.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::ReceiveMsg(const MSG_Object &msg)
{
	// if no custom data is initialized, initial it first
	// then push the message to the queue

	AgentAbstractData *customdata = GetAgentAbstractData();

	customdata->m_msgqueue.push(msg);
}

/*--------------------------------------------------------------------------*
Name:           Execute

Description:    Execute each node on execution list.
				(also handle messages)

Arguments:      dt:		delta time.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::Execute(float dt)
{
	// reset running leaf nodes debug text
	AgentAbstractData *customdata = GetAgentAbstractData();
	customdata->ClearRunningNodesString();

	// pop and evaluate message from queue
	if (m_customdata)
		m_customdata->OnMessage();

	// run execution stack

	for (unsigned i = 0; i < m_executionlist.size(); ++i)
	{
		if (!m_executionlist[i].empty())
		{
			NodeData &data = GetNodeDataFromNodeStack(i);

			data.SetStatus(data.RunLogic(dt));

			if (data.GetStatus() == Status::BT_READY)
			{
				data.Reset();

				if (!data.GetStayStackFlag())
					m_executionlist[i].pop();
#if defined (_DEBUG) && defined (PRINT_BEHAVIORTREE)
				else
				{
					if (data.GetNodeLogic().GetDepth() == 0)
						printf("-------------------------------------------------------\n");
				}
#endif
			}
			else
			{
				if (data.HasParent())
				{
					// only update parent's childstatus if it returns 
					// success or failure, so for parallel composite, 
					// its childstatus will be previous finished status

					if ((data.GetStatus() == Status::BT_SUCCESS) || 
						(data.GetStatus() == Status::BT_FAILURE))
					{
						NodeData &parentdata = data.GetParentNodeData();

						// do additional check here
						// because for parallel composite, "current child" 
						// variable doesn't do anything (all children are 
						// running simultaneously), so first check if parent's 
						// current child equals to the data node, if not, then 
						// it needs to go through all children to find correct 
						// child index to store its status

						if (parentdata.GetCurrentChildNodeIndex() == 
							data.GetNodeLogic().GetIndex())
						{
							parentdata.SetChildStatus(
								parentdata.GetCurrentChildOrder(),
								data.GetStatus());
						}
						else
						{
							BehaviorNode &parentlogic = parentdata.GetNodeLogic();
							BehaviorNode &logic = data.GetNodeLogic();

							for (unsigned i = 0; 
								i < parentlogic.GetChildren().size(); 
								++i)
							{
								if (parentlogic.GetChildNodeIndex(i) == 
									logic.GetIndex())
								{
									parentdata.SetChildStatus(i, data.GetStatus());
									
									break;
								}
							}
						}
					}
				}

			}
		}
	}
}

/* private methods */

/*--------------------------------------------------------------------------*
Name:           GetNodeDataFromNodeStack

Description:    Get behavior node data from top of the stack.

Arguments:      stack_index:	index of the ExecutionList(array of stacks).

Returns:        NodeData&:		reference of the behavior node data.
*---------------------------------------------------------------------------*/
NodeData &AgentBTData::GetNodeDataFromNodeStack(unsigned stack_index)
{
	return m_nodedata[m_executionlist[stack_index].top()];
}

/*--------------------------------------------------------------------------*
Name:           GrowNodeStack

Description:    Grow more stack from execution list if index is more than current size.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::GrowNodeStack(void)
{
	for (unsigned i = 0; i < STACK_INCREASE_NUM; ++i)
		m_executionlist.push_back(NodeStack());
}

/*--------------------------------------------------------------------------*
Name:           ClearExecutionList

Description:    Clear execution list.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::ClearExecutionList(void)
{
	m_executionlist = ExecutionList();
}

/*--------------------------------------------------------------------------*
Name:           FindNextEmptyNodeStack

Description:    Find next empty node stack and return its index.
				(also initialze if needed)

Arguments:      start_stackindex:	node stack index to start searching.

Returns:        unsigned:			empty node stack index.
*---------------------------------------------------------------------------*/
unsigned AgentBTData::FindNextEmptyNodeStack(unsigned start_stackindex)
{
	unsigned list_size = m_executionlist.size();

	for (unsigned i = start_stackindex; i < list_size; ++i)
	{
		// find empty stack
		if (m_executionlist[i].empty())
			return i;
	}

	// can't find empty stack, increase array size
	GrowNodeStack();

	return list_size;
}

/*--------------------------------------------------------------------------*
Name:           ClearNodeStack

Description:    Clear certain node stack from execution list.
				(also all other stacks associate to it)

Arguments:      stack_index:	node stack index to clear.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::ClearNodeStack(unsigned stack_index)
{
	while (m_executionlist[stack_index].size())
	{
		// has to clean out children node stacks before clear out itself

		NodeData &data = GetNodeDataFromNodeStack(stack_index);
		BehaviorNode &btnode = data.GetNodeLogic();

		for (unsigned i = 0; i < btnode.GetChildren().size(); ++i)
		{
			unsigned child_stackindex = 
				GetNodeData(btnode.GetChildNodeIndex(i)).GetStackIndex();

			if (child_stackindex > stack_index)
			{
				ClearNodeStack(child_stackindex);
			}
		}

		m_executionlist[stack_index].pop();
	}
}

/*--------------------------------------------------------------------------*
Name:           PushNodeToExecutionList

Description:    Push node onto execution list.

Arguments:      node_index:		node to push.
				stack_index:	node stack to be pushed to.
				stay_stack:		if the node stays on the stack
								even after it reaches SUCCESSFUL or FAILURE state.

Returns:        None.
*---------------------------------------------------------------------------*/
void AgentBTData::PushNodeToExecutionList(
	int node_index, unsigned stack_index, bool stay_stack)
{
	NodeData &data = m_nodedata[node_index];

	data.SetStatus(BT::BT_READY);
	data.SetStackIndex(stack_index);
	data.SetStayStackFlag(stay_stack);

	m_executionlist[stack_index].push(data.GetNodeLogic().GetIndex());
}
