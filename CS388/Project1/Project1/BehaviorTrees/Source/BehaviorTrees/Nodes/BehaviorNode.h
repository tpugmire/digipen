/******************************************************************************/
/*!
\file		BehaviorNode.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree node logic base class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// behavior tree node (composite, decorator, leaf all inherited from this)
	class BehaviorNode
	{
	public:

		/* getters/setters */

		std::string const &GetName(void) const				{ return m_name; }
		std::string const &GetSummary(void) const			{ return m_summary; }
		std::vector<int> &GetChildren(void)					{ return m_children; }
		std::vector<int> const &GetChildren(void) const		{ return m_children; }
		int GetIndex(void)									{ return m_index; }
		int GetParentIndex(void)							{ return m_parentindex; }
		int GetTreeIndex(void)								{ return m_treeindex; }
		unsigned GetDepth(void)								{ return m_depth; }

		TreeLogic &GetTreeLogic(void);
		TreeLogic const &GetTreeLogic(void) const;
		int GetChildNodeIndex(int child_order);
		int GetChildNodeIndex(int child_order) const;
		BehaviorNode &GetChild(int child_order);
		BehaviorNode const &GetChild(int child_order) const;
		BehaviorNode &GetParent(void);
		BehaviorNode const &GetParent(void) const;

		void SetName(const std::string &name)				{ m_name = name; }
		void SetSummary(const std::string &summary)			{ m_summary = summary; }
		void SetIndex(int index)							{ m_index = index; }
		void SetParentIndex(int parentindex)				{ m_parentindex = parentindex; }
		void SetDepth(unsigned depth)						{ m_depth = depth; }

		/* methods */

		// Initial node
		void Initialize(NodeData *nodedata_ptr);

		// Run every tick
		Status Tick(float dt, NodeData *nodedata_ptr);

#ifdef _DEBUG
		// Depth-first visit method
		void Visit(unsigned depth = 0);
		// Print node status
		void PrintStatus(NodeData &nodedata);
#endif

	protected:

		/* variable */

		std::string m_name;				// node name
		std::string m_summary;			// node summary
		std::vector<int> m_children;	// indexes of all children of the node
		int m_index;					// index of the node on array
		int m_parentindex;				// parent index on array
										// (-1 means the node is root)
		int m_treeindex;				// tree index
		unsigned m_depth;				// depth of the node 

		/* methods */

		// Only run when initialize the node
		virtual void OnInitial(NodeData *nodedata_ptr) = 0;
		// Only run when entering the node
		virtual Status OnEnter(NodeData *nodedata_ptr) = 0;
		// Only run when exiting the node
		virtual void OnExit(NodeData *nodedata_ptr) = 0;
		// Run every frame
		virtual Status OnUpdate(float dt, NodeData *nodedata_ptr) = 0;
		// Only run when node is in suspended
		virtual Status OnSuspend(NodeData *nodedata_ptr) = 0;

		// Verify the node
		virtual bool Verify(void) = 0;
	};
}