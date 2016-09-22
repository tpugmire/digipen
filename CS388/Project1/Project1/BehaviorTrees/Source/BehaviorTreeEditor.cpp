// Jacques Newman
// Copyright: All content ?2016 DigiPen (USA) Corporation, all rights reserved.
// 10/21/2015
#include <Stdafx.h>

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <winuser.h>
#include "BehaviorTreeNodes.h"

namespace BehaviorTreeEditor
{
  struct Node
  {
	int     ID;
	std::vector<int> childrenid;
	char    Name[32];
	ImVec2  Pos, Size;
	bool    Active;
	int     InputsCount, OutputsCount;
	IBTNode * node;
	Node(int id, const char* name, const ImVec2& pos, bool active, int inputs_count, int outputs_count, IBTNode * node_)
	{
	  node = node_;
	  ID = id; strncpy_s(Name, name, 31);
	  Name[31] = 0; Pos = pos; Active = active;
	  InputsCount = inputs_count;
	  OutputsCount = outputs_count;
	}

	ImVec2 GetInputSlotPos(int slot_no) const   { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
	ImVec2 GetOutputSlotPos(int slot_no) const  { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
  };
  struct NodeLink
  {
	int     InputIdx, InputSlot, OutputIdx, OutputSlot;

	NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
  };
  const float NODE_SLOT_RADIUS = 4.0f;
  const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
  static ImColor SelectorTint(0, 51, 51);
  static ImColor LeafTint(86, 119, 20);
  static ImColor DecoratorTint(85, 0, 0);

  /*
  enum NodeStatus
  {
  NS_Failed,
  NS_Completed,
  NS_Running,
  NS_OnEnter


  };

  enum NodeType
  {
  NT_Selector = 0,
  NT_Decorator = 1,
  NT_Leaf = 2
  };
  */
  const unsigned nodeTypeCount = 4;
  const char * nodeTypes[nodeTypeCount] = { "NT_Selector", "NT_Decorator", "NT_Leaf", "" };
  const unsigned nodeStatuscount = 4;
  const char * nodeStatus[nodeStatuscount] = { "NS_Failed", "NS_Completed", "NS_Running", "NS_OnEnter" };
  static float baseNodeSizeX = 20;
  static float baseNodeSizeY = 20;
  static float zoomScale = 100; // e.g 100%

  void BuildTree(IBTNode * root, vector<Node> & nodes, vector<NodeLink> & links, int & id, ImVec2 pos = ImVec2(0, 0))
  {
	// check to see if it has children
	int childCount = 0;
	IBTLogicNode * node = nullptr;
	switch (root->m_type)
	{
	case NT_Selector: // intentional
	case NT_Decorator:
	  node = static_cast<IBTLogicNode *>(root);
	  childCount = node->m_children.size();
	  break;
	case NT_Leaf: break;
	default: break;
	}
	int inputCount = 0;
	if (root->p_parent)
	{
	  inputCount = 1;
	}
	// add root
	nodes.push_back(Node(id, root->m_name.c_str(), pos, false, inputCount, childCount, root));
	nodes[id].Size = ImVec2(baseNodeSizeX * zoomScale / 100.0f, baseNodeSizeY * zoomScale / 100.0f);
	if (id == 0)
	{
	  pos = nodes[id].Size;
	}
	// add nodes
	// add children if possible
	if (node)
	{
	  int childSlot = 0;
	  int myid = id;
	  for (auto & child : node->m_children)
	  {
		if (child == nullptr) continue;
		++id;
		// determine position based on size and child number
		ImVec2 childPos = pos;
		// move to the right
		childPos.x += nodes[myid].Size.x*5.0f;
		childPos.y += nodes[myid].Size.y * 2 * (childSlot +1 - (static_cast<float>(childCount+1) / 2.0f));
		// make things more extream the farther from the center they are

		links.push_back(NodeLink(myid, childSlot, id, 0));
		nodes[myid].childrenid.push_back(id);
		BuildTree(child, nodes, links, id, childPos);
		++childSlot;
	  }
	}
  }
  void UpdateNodes(IBTNode * root, vector<Node> & nodes, int id = 0)
  {
	// check to see if it has children

	int childCount = 0;
	IBTLogicNode * node = nullptr;
	switch (root->m_type)
	{
	case NT_Selector: // intentional
	case NT_Decorator:
	  node = static_cast<IBTLogicNode *>(root);
	  childCount = node->m_children.size();
	  break;
	case NT_Leaf: break;
	default: break;
	}
	nodes[id].Active = true;
	// add nodes
	// add children if possible
	if (node)
	{
	  int childSlot = 0;
	  
	  for (auto childID : nodes[id].childrenid)
	  {
		if (node->m_currentChildIndex > 0)
		{
			if (node->m_currentChildIndex > static_cast<int>(node->m_children.size()))
		  {
			if (nodes[childID].node == node->m_children[node->m_currentChildIndex])
			{
			  UpdateNodes(node->m_children[node->m_currentChildIndex], nodes, childID);
			}
		  }
		}
	  }
	}
  }

  static struct PossibleNodes
  {
	std::unordered_map<std::string, LogicNodeInfo> logicNodes_copy;
	std::unordered_map<std::string, LeafNodeCreateFn> leafNodes_copy;
	std::vector<std::string> LuaFilenames;
	std::vector<const char *> SelctorNodes;
	std::vector<const char *> DecoratorNodes;
	std::vector<const char *> LeafNodes;
  }possible_nodes;
  void GetPossibleNodes()
  {
	possible_nodes.logicNodes_copy.clear();
	possible_nodes.leafNodes_copy.clear();
	possible_nodes.LuaFilenames.clear();
	possible_nodes.SelctorNodes.clear();
	possible_nodes.DecoratorNodes.clear();
	possible_nodes.LeafNodes.clear();
	// make copies in order to guarentee data will not change until this function is called
	possible_nodes.logicNodes_copy = LogicNodesMap();
	possible_nodes.leafNodes_copy = LeafNodesMap();

	// append logic nodes to respective vectors
	for (auto & node : possible_nodes.logicNodes_copy)
	{
	  switch (node.second.first)
	  {
	  case NT_Selector:
		possible_nodes.SelctorNodes.push_back(node.first.c_str());
		break;
	  case NT_Decorator:
		possible_nodes.DecoratorNodes.push_back(node.first.c_str());
		break;
	  case NT_Leaf: break;
	  default: break;
	  }
	}

	// append leaf nodes to respective vectors
	for (auto & node : possible_nodes.leafNodes_copy)
	{
	  possible_nodes.LeafNodes.push_back(node.first.c_str());
	}
  }

  void ShowBehaviorTreeNodes(GameObject * tree, bool shouldreload = false)
  {
	IBTNode * root = tree->GetBehaviorTree();
	// display contents of root
	{
	  static vector<Node> nodes;
	  static vector<NodeLink> links;
	  static ImVec2 scrolling = ImVec2(0.0f, 0.0f);

	  static int node_selected = -1;
	  static bool reload = true;
	  static std::string rootName = "";
	  if (ImGui::Button("Rebuild Tree") || reload || shouldreload || root->m_name != rootName)
	  {
		nodes.clear();
		links.clear();
		int rootID = 0;
		BuildTree(root, nodes, links,rootID);
		reload = false;
		rootName = root->m_name;
	  }
	  // set all to inactive
	  for (auto &node : nodes)
	  {
		node.Active = false;
	  }
	  // update all
	  UpdateNodes(root, nodes);

	  // Draw a list
	  bool open_context_menu = false;
	  int node_hovered_in_list = -1;
	  int node_hovered_in_scene = -1;
	  ImGui::BeginChild("node_list", ImVec2(100, 0));
	  ImGui::Text("Nodes");
	  ImGui::Separator();
	  reload |= ImGui::DragFloat("X", &baseNodeSizeX);
	  reload |= ImGui::DragFloat("Y", &baseNodeSizeY); 

	  for (int node_idx = 0; node_idx < static_cast<int>(nodes.size()); node_idx++)
	  {
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		if (ImGui::Selectable(node->Name, node->ID == node_selected))
		  node_selected = node->ID;
		if (ImGui::IsItemHovered())
		{
		  node_hovered_in_list = node->ID;
		  open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	  }
	  ImGui::EndChild();

	  ImGui::SameLine();
	  ImGui::BeginGroup();


	  ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	  ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(40, 40, 40, 200));
	  ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	  ImDrawList* draw_list = ImGui::GetWindowDrawList();
	  draw_list->ChannelsSplit(2);
	  ImVec2 offset = ImGui::GetCursorScreenPos() - scrolling;

	  ImGui::PushItemWidth(120.0f);
	  
	  // Display links
	  draw_list->ChannelsSetCurrent(0); // Background
	  for (int link_idx = 0; link_idx < static_cast<int>(links.size()); link_idx++)
	  {
		// Color when not active:
		ImColor LineColor(160, 160, 160);
		ImColor ActiveColor(220, 220, 80);

		NodeLink* link = &links[link_idx];
		Node* node_inp = &nodes[link->InputIdx];
		Node* node_out = &nodes[link->OutputIdx];

		// Hermite spline

		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
		ImVec2 t1 = ImVec2(+80.0f, 0.0f);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
		ImVec2 t2 = ImVec2(+80.0f, 0.0f);
		const int STEPS = 12;
		for (int step = 0; step <= STEPS; step++)
		{
		  float t = (float)step / (float)STEPS;
		  float h1 = +2 * t*t*t - 3 * t*t + 1.0f;
		  float h2 = -2 * t*t*t + 3 * t*t;
		  float h3 = t*t*t - 2 * t*t + t;
		  float h4 = t*t*t - t*t;
		  draw_list->PathLineTo(ImVec2(h1*p1.x + h2*p2.x + h3*t1.x + h4*t2.x, h1*p1.y + h2*p2.y + h3*t1.y + h4*t2.y));
		}

		if (   node_inp->node->GetCurrentStatus() == NS_Running 
			&& node_out->node->GetCurrentStatus() == NS_Running)
		{
		  LineColor = ActiveColor;
		}
		draw_list->PathStroke(LineColor, false, 3.0f);
	  }

	  // Display nodes
	  for (int node_idx = 0; node_idx < static_cast<int>(nodes.size()); node_idx++)
	  {
		Node* node = &nodes[node_idx];
		/*if (node->node == nullptr)
		{
		  continue;
		}*/
		ImGui::PushID(node->ID);

		ImVec2 node_rect_min = offset + node->Pos;

		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground

		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
		ImGui::PushID(node->ID);
		if (ImGui::TreeNode(node->Name))
		{
		  /**************************************************************/
		  if (node->node->m_type == NT_Leaf)
		  {
			ImGui::Text("Type: Leaf");
		  }
		  else
		  {
			ImGui::Text(string("Type: " + node->node->m_type_name).c_str());
		  }
		  ImGui::Text("Status: "); ImGui::SameLine();
		  ImGui::Text(nodeStatus[node->node->GetCurrentStatus()]);
		  node->node->OnEdit();
		  /**************************************************************/
		  /*Edit node contents*******************************************/
		  /**************************************************************/
		  /**************************************************************/
		  ImGui::TreePop();
		}
		ImGui::PopID();
		ImGui::PopStyleColor(3);
		ImGui::EndGroup();

		if (ImGui::IsItemHovered())
		{
		  if (node->node->m_description != "")
		  {
			ImGui::BeginTooltip();
			ImGui::Text(node->node->m_description.c_str());
			ImGui::EndTooltip();
		  }
		}
		node->Size = ImGui::GetItemRectSize() + (NODE_WINDOW_PADDING + NODE_WINDOW_PADDING);
		ImVec2 node_rect_max = node_rect_min + node->Size;

		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size);
		if (ImGui::IsItemHovered())
		{
		  node_hovered_in_scene = node->ID;
		  open_context_menu |= ImGui::IsMouseClicked(1);
		}
		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
		  node_selected = node->ID;
		if (node_moving_active && ImGui::IsMouseDragging(0))
		{
		  node->Pos.x += ImGui::GetIO().MouseDelta.x;
		  node->Pos.y += ImGui::GetIO().MouseDelta.y;
		}
		const ImColor HoveredTint(15, 15, 15);
		ImColor RegColor(60, 60, 60);
		if (node->node->m_type == NT_Leaf)
		{
		  RegColor = LeafTint;
		}
		else if (node->node->m_type == NT_Selector)
		{
		  RegColor = SelectorTint;
		}
		else if (node->node->m_type == NT_Decorator)
		{
		  RegColor = DecoratorTint;
		}
		// offset for leaves and selectors
		draw_list->AddRectFilled(node_rect_min, node_rect_max, (
		  node_hovered_in_list == node->ID
		  || node_hovered_in_scene == node->ID
		  || (node_hovered_in_list == -1 && node_selected == node->ID))
		  ? RegColor + HoveredTint : RegColor, 4.0f);

		/******************************/
		// change color when active
		ImColor node_color(100, 100, 100);
		//if (node->Active)
		//  node_color = ImColor(255, 255, 0);
		if (node->node->GetCurrentStatus() == NS_Running)
		{
		  node_color = ImColor(255, 255, 0);
		}
		/******************************/

		draw_list->AddRect(node_rect_min, node_rect_max, node_color, 4.0f);
		for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
		{
		  ImVec2 pos = node->GetInputSlotPos(slot_idx);
		  draw_list->AddCircleFilled(offset + pos, NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));
		}
		for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
		{
		  ImVec2 pos = node->GetOutputSlotPos(slot_idx);
		  draw_list->AddCircleFilled(offset + pos, NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));
		}

		ImGui::PopID();
	  }
	  draw_list->ChannelsMerge();
	  
	  // Open context menu
	  if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	  {
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		open_context_menu = true;
	  }
	  if (open_context_menu)
	  {
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
		  node_selected = node_hovered_in_list;
		if (node_hovered_in_scene != -1)
		  node_selected = node_hovered_in_scene;
	  }
	  bool CreateNewNode = false;
	  bool RenameSelected = false;
	  // Currently Selected node
	  Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
	  // Draw context menu
	  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	  if (ImGui::BeginPopup("context_menu"))
	  {
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
		  ImGui::Text("Node '%s'", node->Name);
		  ImGui::Separator();
		  NodeType type = node->node->m_type;
		  bool canAddChild = false;
		  if (type != NT_Leaf)
		  {
			if (type == NT_Decorator)
			{
			  IBTLogicNode * p = static_cast<IBTLogicNode *>(node->node);
			  if (p->m_children.size() < 1)
			  {
				canAddChild = true;
			  }
			}
			if (type == NT_Selector)
			{
			  canAddChild = true;
			}
		  }
		  if (canAddChild && ImGui::MenuItem("Add Child"))
		  {
			GetPossibleNodes();
			CreateNewNode = true;
		  }
		  if (type != NT_Leaf && ImGui::MenuItem("Rename.."))
		  {
			RenameSelected = true;
		  }
		  if (root != node->node && ImGui::MenuItem("Delete"))
		  {
			// remove from parent
			IBTNode * p = node->node->p_parent;
			if (p)
			{
			  IBTLogicNode * parent = static_cast<IBTLogicNode * >(p);
			  auto childIt = std::find(parent->m_children.begin(), parent->m_children.end(), node->node);
			  if (childIt != parent->m_children.end())
			  {
				// check out child index
				//if (parent->m_currentChildIndex == std::distance(parent->m_children.begin(), childIt))
				{
				  // reset it 
				  parent->m_currentChildIndex = 0;
				}
				// remove it
				parent->m_children.erase(childIt);
			  }
			  // delete
			  delete node->node;
			  node->node = nullptr;
			  node_selected = -1;
			  reload = true;
			}

		  }
		  //if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		ImGui::EndPopup();
	  }
	  if (CreateNewNode)
	  {
		ImGui::OpenPopup("Create A Node");
	  }
	  if (ImGui::BeginPopup("Create A Node"))
	  {
		// Pick Node Type
		static int currentItem = 0;
		static int selected = 0;

		ImGui::Text("Node Type"); ImGui::SameLine();
		if (ImGui::Combo("##Node Type", &currentItem, nodeTypes, nodeTypeCount))
		{
		  selected = 0;
		}
		// show valid nodes
		static char buf[16] = { 0 };
		if (currentItem < 4 && currentItem >= 0)
		{
		  NodeType type = static_cast<NodeType>(currentItem);
		  if (type == NT_Selector)
		  {
			ImGui::Text("Selectors"); ImGui::SameLine();
			if (ImGui::Combo("##Selectors", &selected,
			  const_cast<const char **>(possible_nodes.SelctorNodes.data()),
			  static_cast<int>(possible_nodes.SelctorNodes.size())))
			{
			  std::strcpy(buf, possible_nodes.SelctorNodes[selected]);
			}
		  }
		  else if (type == NT_Decorator)
		  {
			ImGui::Text("Decorators"); ImGui::SameLine();
			if (ImGui::Combo("##DecoratorsChooser", &selected,
			  const_cast<const char **>(possible_nodes.DecoratorNodes.data()),
			  static_cast<int>(possible_nodes.DecoratorNodes.size())))
			{
			  std::strcpy(buf, possible_nodes.DecoratorNodes[selected]);
			}
		  }
		  else if (type == NT_Leaf)
		  {
			ImGui::Text("Leaves"); ImGui::SameLine();
			if (ImGui::Combo("##Leaves", &selected,
			  const_cast<const char **>(possible_nodes.LeafNodes.data()),
			  static_cast<int>(possible_nodes.LeafNodes.size())))
			{
			  std::strcpy(buf, possible_nodes.LeafNodes[selected]);
			}
		  }
		  ImGui::Text("NodeName"); ImGui::SameLine();
		  ImGui::InputText("##NodeName", buf, 16, ImGuiInputTextFlags_CharsNoBlank);
		  if (ImGui::Button("Add"))
		  {
			if (std::strcmp(buf, "") != 0)
			{
			  // add the tree
			  IBTLogicNode * parent = nullptr;
			  switch (root->m_type)
			  {
			  case NT_Selector: // intentional
			  case NT_Decorator:
				parent = static_cast<IBTLogicNode *>(node->node);
				break;
			  case NT_Leaf: break;
			  default: break;
			  }
			  if (parent)
			  {
				IBTNode * new_node = nullptr;
				if (type == NT_Leaf)
				{
				  new_node = createLeafNode(std::string(possible_nodes.LeafNodes[selected]), std::string(buf));
				}
				else if (type == NT_Decorator)
				{
				  new_node = createLogicNode(std::string(possible_nodes.DecoratorNodes[selected]), std::string(buf));
				}
				else if (type == NT_Selector)
				{
				  new_node = createLogicNode(std::string(possible_nodes.SelctorNodes[selected]), std::string(buf));
				}
				if (new_node)
				{
				  parent->m_children.push_back(new_node);
				  new_node->p_parent = parent;
				  reload = true;
				}
			  }
			  ImGui::CloseCurrentPopup();
			}
		  }
		  ImGui::SameLine();
		}
		if (ImGui::Button("Cancel"))
		{
		  ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	  }
	  ImGui::PopStyleVar();

	  if (RenameSelected)
	  {
		ImGui::OpenPopup("Rename Selected");
	  }
	  if (ImGui::BeginPopup("Rename Selected"))
	  {
		static char buffer[48] = { 0 };
		ImGui::InputText("##renameBehaviorASText", buffer, 48, ImGuiInputTextFlags_CharsNoBlank);
		if (std::strcmp(buffer, "") != 0)
		{
		  if (ImGui::Button(("Rename to: " + std::string(buffer)).c_str()))
		  {
			node->node->m_name = std::string(buffer);
			ImGui::CloseCurrentPopup();
			reload = true;
		  }
		}
		ImGui::EndPopup();
	  }

	  // Scrolling
	  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
	  {
		scrolling.x -= ImGui::GetIO().MouseDelta.x;
		scrolling.y -= ImGui::GetIO().MouseDelta.y;
	  }
	  ImGui::PopItemWidth();


	  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && abs(ImGui::GetIO().MouseWheel) > 0.1f)
	  {
		if (ImGui::IsKeyDown(VK_SHIFT))
		{
		  scrolling.x -= ImGui::GetIO().MouseWheel * 15.0f;
		}
		else if (ImGui::IsKeyDown(VK_CONTROL))
		{
		  scrolling.y -= ImGui::GetIO().MouseWheel * 15.0f;
		}
		else
		{
		  zoomScale += ImGui::GetIO().MouseWheel * 15.0f;
		  if (zoomScale <= 0.10f)
		  {
			zoomScale = 0.1f;
		  }
		  reload = true;
		}
	  }
	  ImGui::EndChild();
	  ImGui::PopStyleColor();
	  ImGui::PopStyleVar(2);
	  ImGui::EndGroup();
	}
  }
  static objectID selectedObj;
  static bool reloadTree = true;
  static void BehaviorMenu(GUIWindow * self)
  {
	if (ImGui::BeginMenuBar())
	{
	  GameObject * obj = g_database.Find(selectedObj);
	  if (obj)
	  {
		auto tree = obj->GetBehaviorTree();

		if (tree)
		{
		  bool SaveAs = false;
		  // display menu

		  if (ImGui::BeginMenu("File"))
		  {
			if (ImGui::MenuItem("Save"))
			{
			  obj->SaveBHV();
			  reloadTree = true;
			}
			if (ImGui::MenuItem("SaveAs"))
			{
			  SaveAs = true;
			}
			ImGui::EndMenu();
		  }

		  // save as popup
		  static char SaveAsName[48] = { 0 };
		  if (SaveAs)
		  {
			ImGui::OpenPopup("Save Behavior As...");
			std::strcpy(SaveAsName, tree->m_name.c_str());
		  }
		  if (ImGui::BeginPopup("Save Behavior As..."))
		  {
			ImGui::InputText("##SaveBehaviorASText", SaveAsName, 48, ImGuiInputTextFlags_CharsNoBlank);
			if (std::strcmp(SaveAsName, "") != 0)
			{
			  if (ImGui::Button(("Save as: " + std::string(SaveAsName) + ".bhv").c_str()))
			  {
				obj->SaveAsBHV(SaveAsName);
				reloadTree = true;
				ImGui::CloseCurrentPopup();
			  }
			}
			ImGui::EndPopup();
		  }
		}
	  }
	}
	ImGui::EndMenuBar();
  }
  static void BehaviorViewer(GUIWindow * self)
  {
	BehaviorMenu(self);
	{
	  ImGui::Button("Show Object List");
	  if (ImGui::BeginPopupContextItem("Item List",0))
	  { 
		dbCompositionList list;
		g_database.ComposeList(list, OBJECT_NPC);

		dbCompositionList::iterator i;
		for (i = list.begin(); i != list.end(); ++i)
		{
		  if (ImGui::Selectable((*i)->GetName(), (*i)->GetID() == selectedObj))
		  {
			selectedObj = (*i)->GetID();
			reloadTree = true;
		  }
		}
		ImGui::EndPopup();
	  }
	  if (g_database.Find(selectedObj))
	  {
		ShowBehaviorTreeNodes(g_database.Find(selectedObj), reloadTree);
		reloadTree = false;
	  }
	}

  }
  GUIWindow bhvTreeViewer("BehaviorViewer", BehaviorViewer,showBehaviorViewer,ImGuiWindowFlags_MenuBar);
}
// NODE GRAPH EXAMPLE START

namespace example
{
  struct Node
  {
	int     ID;
	char    Name[32];
	ImVec2  Pos, Size;
	float   Value;
	int     InputsCount, OutputsCount;

	Node(int id, const char* name, const ImVec2& pos, float value, int inputs_count, int outputs_count) { ID = id; strncpy_s(Name, name, 31); Name[31] = 0; Pos = pos; Value = value; InputsCount = inputs_count; OutputsCount = outputs_count; }

	ImVec2 GetInputSlotPos(int slot_no) const   { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
	ImVec2 GetOutputSlotPos(int slot_no) const  { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
  };
  struct NodeLink
  {
	int     InputIdx, InputSlot, OutputIdx, OutputSlot;

	NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
  };
  void NodeGraphExample(GUIWindow * self)
  {
	// Really dumb data structure provided for the example.
	// Note that we storing links are INDICES (not ID) to make example code shorter, obviously a bad idea for any general purpose code
	{
	  ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);

	  static ImVector<Node> nodes;
	  static ImVector<NodeLink> links;
	  static bool inited = false;
	  static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	  static int node_selected = -1;
	  if (!inited)
	  {
		nodes.push_back(Node(0, "MainTex", ImVec2(40, 50), 0.5f, 1, 1));
		nodes.push_back(Node(1, "BumpMap", ImVec2(40, 150), 0.42f, 1, 1));
		nodes.push_back(Node(2, "Combine", ImVec2(270, 80), 1.0f, 2, 2));
		links.push_back(NodeLink(0, 0, 2, 0));
		links.push_back(NodeLink(1, 0, 2, 1));
		inited = true;
	  }

	  // Draw a list
	  bool open_context_menu = false;
	  int node_hovered_in_list = -1;
	  int node_hovered_in_scene = -1;
	  ImGui::BeginChild("node_list", ImVec2(100, 0));
	  ImGui::Text("Nodes");
	  ImGui::Separator();
	  for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	  {
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		if (ImGui::Selectable(node->Name, node->ID == node_selected))
		  node_selected = node->ID;
		if (ImGui::IsItemHovered())
		{
		  node_hovered_in_list = node->ID;
		  open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	  }
	  ImGui::EndChild();

	  ImGui::SameLine();
	  ImGui::BeginGroup();

	  const float NODE_SLOT_RADIUS = 4.0f;
	  const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

	  ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	  ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(40, 40, 40, 200));
	  ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	  ImGui::PushItemWidth(120.0f);

	  ImDrawList* draw_list = ImGui::GetWindowDrawList();
	  draw_list->ChannelsSplit(2);
	  ImVec2 offset = ImGui::GetCursorScreenPos() - scrolling;

	  // Display links
	  draw_list->ChannelsSetCurrent(0); // Background
	  for (int link_idx = 0; link_idx < links.Size; link_idx++)
	  {
		NodeLink* link = &links[link_idx];
		Node* node_inp = &nodes[link->InputIdx];
		Node* node_out = &nodes[link->OutputIdx];

		// Hermite spline

		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
		ImVec2 t1 = ImVec2(+80.0f, 0.0f);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
		ImVec2 t2 = ImVec2(+80.0f, 0.0f);
		const int STEPS = 12;
		for (int step = 0; step <= STEPS; step++)
		{
		  float t = (float)step / (float)STEPS;
		  float h1 = +2 * t*t*t - 3 * t*t + 1.0f;
		  float h2 = -2 * t*t*t + 3 * t*t;
		  float h3 = t*t*t - 2 * t*t + t;
		  float h4 = t*t*t - t*t;
		  draw_list->PathLineTo(ImVec2(h1*p1.x + h2*p2.x + h3*t1.x + h4*t2.x, h1*p1.y + h2*p2.y + h3*t1.y + h4*t2.y));
		}
		draw_list->PathStroke(ImColor(200, 200, 100), false, 3.0f);
	  }

	  // Display nodes
	  for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	  {
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);

		ImVec2 node_rect_min = offset + node->Pos;

		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground

		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node->Name);
		ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
		float dummy_color[3] = { node->Pos.x / ImGui::GetWindowWidth(), node->Pos.y / ImGui::GetWindowHeight(), fmodf((float)node->ID * 0.5f, 1.0f) };
		ImGui::ColorEdit3("##color", &dummy_color[0]);
		ImGui::EndGroup();

		node->Size = ImGui::GetItemRectSize() + (NODE_WINDOW_PADDING + NODE_WINDOW_PADDING);
		ImVec2 node_rect_max = node_rect_min + node->Size;

		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size);
		if (ImGui::IsItemHovered())
		{
		  node_hovered_in_scene = node->ID;
		  open_context_menu |= ImGui::IsMouseClicked(1);
		}
		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
		  node_selected = node->ID;
		if (node_moving_active && ImGui::IsMouseDragging(0))
		{
		  node->Pos.x += ImGui::GetIO().MouseDelta.x;
		  node->Pos.y += ImGui::GetIO().MouseDelta.y;
		}

		draw_list->AddRectFilled(node_rect_min, node_rect_max, (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? ImColor(75, 75, 75) : ImColor(60, 60, 60), 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);
		for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
		{
		  ImVec2 pos = node->GetInputSlotPos(slot_idx);
		  draw_list->AddCircleFilled(offset + pos, NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));
		}
		for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
		{
		  ImVec2 pos = node->GetOutputSlotPos(slot_idx);
		  draw_list->AddCircleFilled(offset + pos, NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));
		}

		ImGui::PopID();
	  }
	  draw_list->ChannelsMerge();

	  // Open context menu
	  if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	  {
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		open_context_menu = true;
	  }
	  if (open_context_menu)
	  {
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
		  node_selected = node_hovered_in_list;
		if (node_hovered_in_scene != -1)
		  node_selected = node_hovered_in_scene;
	  }

	  // Draw context menu
	  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	  if (ImGui::BeginPopup("context_menu"))
	  {
		Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
		  ImGui::Text("Node '%s'", node->Name);
		  ImGui::Separator();
		  if (ImGui::MenuItem("Rename.."))
		  {

		  }
		  if (ImGui::MenuItem("Delete", NULL, false, false)) {}
		  //if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else
		{
		  if (ImGui::MenuItem("Add")) { nodes.push_back(Node(nodes.Size, "New node", scene_pos, 0.5f, 2, 2)); }
		  if (ImGui::MenuItem("Paste", NULL, false, false)) {}
		}
		ImGui::EndPopup();
	  }
	  ImGui::PopStyleVar();

	  // Scrolling
	  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
	  {
		scrolling.x -= ImGui::GetIO().MouseDelta.x;
		scrolling.y -= ImGui::GetIO().MouseDelta.y;
	  }

	  ImGui::PopItemWidth();
	  ImGui::EndChild();
	  ImGui::PopStyleColor();
	  ImGui::PopStyleVar(2);
	  ImGui::EndGroup();
	}
  }
  // NODE GRAPH EXAMPLE END
  //GUIWindow nodeGraphExWindow("NodeGraphExample", NodeGraphExample, true);
}
