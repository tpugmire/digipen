/* Copyright Steve Rabin, 2013.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2013"
 */

#pragma once

//Add new object types here (bitfield mask - objects can be combinations of types)
#define OBJECT_Ignore_Type  (0)
#define OBJECT_Gameflow     (1<<1)
#define OBJECT_Character    (1<<2)
#define OBJECT_NPC          (1<<3)
#define OBJECT_Player       (1<<4)
#define OBJECT_Enemy        (1<<5)
#define OBJECT_Weapon       (1<<6)
#define OBJECT_Item         (1<<7)
#define OBJECT_Projectile   (1<<8)

#define GAME_OBJECT_MAX_NAME_SIZE 64


//Forward declarations
class StateMachineManager;
class MSG_Object;
class Movement;
class Body;

class GameObject
{
public:

	GameObject(objectID id, unsigned int type, char* name);
	~GameObject(void);

	inline objectID GetID(void)					{ return(m_id); }
	inline unsigned int GetType(void)				{ return(m_type); }
	inline char* GetName(void)					{ return(m_name); }

	void Initialize(void);
	void Update(void);
	void Animate(double dTimeDelta);
	void AdvanceTime(double dTimeDelta, D3DXVECTOR3 *pvEye);
	void Draw(IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj);
	void RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	void InvalidateDeviceObjects(void);

	//State machine related
	void CreateStateMachineManager(void);
	inline bool HasStateMachine(void)     { return (m_stateMachineManager != 0); }
	inline StateMachineManager* GetStateMachineManager( void )	{ ASSERTMSG(m_stateMachineManager, "GameObject::GetStateMachineManager - m_stateMachineManager not set"); return( m_stateMachineManager ); }
	//inline StateMachineManager* GetStateMachineManager(void)	{ return(m_stateMachineManager); }

	//Scheduled deletion
	inline void MarkForDeletion(void)				{ m_markedForDeletion = true; }
	inline bool IsMarkedForDeletion(void)			{ return(m_markedForDeletion); }

	//Movement component
	void CreateMovement(void);
	inline Movement& GetMovement(void)			{ ASSERTMSG(m_movement, "GameObject::GetMovement - m_movement not set"); return(*m_movement); }

	//Body component
	void CreateBody(int health, D3DXVECTOR3& pos);
	inline Body& GetBody(void)					{ ASSERTMSG(m_body, "GameObject::GetBody - m_body not set"); return(*m_body); }

	//Tiny
	void CreateTiny(CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent, float red = 1.0f, float green = 1.0f, float blue = 1.0f);
	inline CTiny& GetTiny(void)					{ ASSERTMSG(m_tiny, "GameObject::GetModel - m_tiny not set"); return(*m_tiny); }

	D3DXVECTOR3 GetTargetPOS()            { return m_targetPOS; }
	void SetTargetPOS(D3DXVECTOR3 pos)    { m_targetPOS = pos; }

	// added for Behavior tree (Chi-Hao) START
	TinySpeedStatus GetSpeedStatus(void)			{ return m_speedstatus; }
	void SetSpeedStatus(TinySpeedStatus status)		{ m_speedstatus = status; }

	bool HasBehaviorTree(void)						{ return m_hasBehaviorTree; }
	void SetHasBehaviorTreeFlag(bool flag)			{ m_hasBehaviorTree = flag; }

	// added for Behavior tree (Chi-Hao) END

private:
	friend void PushText(objectID, std::string);
	friend void RenderText();
	std::stringstream m_behaviorTreeInfo;
	D3DXVECTOR3 m_targetPOS;

	objectID m_id;									//Unique id of object (safer than a pointer).
	unsigned int m_type;							//Type of object (can be combination).
	bool m_markedForDeletion;						//Flag to delete this object (when it is safe to do so).
	char m_name[GAME_OBJECT_MAX_NAME_SIZE];			//String name of object.

	//Components
	Movement* m_movement;
	Body* m_body;
	CTiny* m_tiny;
	StateMachineManager* m_stateMachineManager;

	// added for Behavior tree (Chi-Hao) START
	TinySpeedStatus m_speedstatus;	// agent's speed status
	bool m_hasBehaviorTree;			// flag: if agent has behavior tree
	// added for Behavior tree (Chi-Hao) END
};
