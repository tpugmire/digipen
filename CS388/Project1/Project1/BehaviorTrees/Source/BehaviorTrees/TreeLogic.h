/******************************************************************************/
/*!
\file		TreeLogic.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree logic class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// behavior tree logic class (1 instance of each behavior tree)
	class TreeLogic
	{
	public:

		/* constructors/destructor */

		TreeLogic(const std::string &name);
		TreeLogic(TreeLogic &&logic);

		/* getters/setters */

		std::string &GetName(void)									{ return m_name; }
		std::string const &GetName(void) const						{ return m_name; }
		std::string &GetSummary(void)								{ return m_summary; }
		std::string const &GetSummary(void) const					{ return m_summary; }
		BehaviorNode &GetNode(unsigned nodeindex)					{ return *m_logics[nodeindex]; }
		BehaviorNode const &GetNode(unsigned nodeindex) const		{ return *m_logics[nodeindex]; }

		void SetSummary(const std::string &summary)					{ m_summary = summary; }

		/* operators */

		BehaviorNode &operator[](unsigned nodeindex)				{ return GetNode(nodeindex); }
		BehaviorNode const &operator[](unsigned nodeindex) const	{ return GetNode(nodeindex); }

		// Move operator
		TreeLogic &operator=(TreeLogic &&logic);

		/* methods */

		// Generate default data array
		void GenerateDefaultDataArray(std::vector<NodeData> *nodedata_ptr);

		// Add node logic (return -1 if logic can't be added)
		template <typename T>
		int AddLogic(unsigned depth, const std::string &name, std::stack<unsigned> *treestack_ptr);

	private:

		/* variables */

		std::string m_name;										// name of the tree
		std::string m_summary;									// tree summary
		std::vector<std::unique_ptr<BehaviorNode>> m_logics;	// node logic array
	};

	/* template functions */

	/*--------------------------------------------------------------------------*
	Name:           AddLogic

	Description:    Add node logic.

	Arguments:      depth:			node depth.
					name:			node name. pass in empty string to use class name as default node name.
					treestack_ptr:	tree stack used to build tree.

	Returns:        int:		return the node index, -1 if logic can't be added.
	*---------------------------------------------------------------------------*/
	template <typename T>
	int TreeLogic::AddLogic(unsigned depth, const std::string &name, std::stack<unsigned> *treestack_ptr)
	{
		// create logic and assign name from classname if name is empty string

		std::unique_ptr<T> logic_ptr = std::make_unique<T>();

		if (name.empty())
			logic_ptr->SetName(GetMyClassName(logic_ptr.get()));
		else
			logic_ptr->SetName(name);

		// then build tree from tree stack

		if (!treestack_ptr->empty())
		{
			// illegal, new logic has depth 2+ than previous logic
			if (depth > (treestack_ptr->top() + 1))
				return -1;

			int size = m_logics.size();

			logic_ptr->SetDepth(depth);
			logic_ptr->SetIndex(size);

			// use treestack to keep track parent index
			while (!treestack_ptr->empty())
			{
				unsigned topstack_depth = m_logics[treestack_ptr->top()]->GetDepth();

				// if depth is 1+ than top of stack
				// top of stack is parent_index, then push itself onto stack
				if (depth > topstack_depth)
				{
					logic_ptr->SetParentIndex(treestack_ptr->top());
					m_logics[treestack_ptr->top()]->GetChildren().push_back(size);
					treestack_ptr->push(size);

					break;
				}
				// otherwise, keep popping stack until it satisfies the condition
				else
					treestack_ptr->pop();
			}

			m_logics.push_back(std::move(logic_ptr));

			return size;
		}
		else
		{
			// if there's no parent, this has to be the root node

			if ((depth == 0) && (m_logics.empty()))
			{
				logic_ptr->SetDepth(0);
				logic_ptr->SetIndex(0);
				logic_ptr->SetParentIndex(-1);

				m_logics.push_back(std::move(logic_ptr));

				treestack_ptr->push(0);

				return 0;
			}

			return -1;
		}
	}
}