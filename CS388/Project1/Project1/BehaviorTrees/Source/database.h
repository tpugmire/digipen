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

class GameObject;

#define INVALID_OBJECT_ID 0

typedef std::vector<GameObject*> dbCompositionList;

class Database
{
public:

	Database( void );
	~Database( void );

	void Update( void );
	void Animate( double dTimeDelta );
	void AdvanceTimeAndDraw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye );
	void Initialize( void );
	void RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	void InvalidateDeviceObjects( void );
	void SendMsgFromSystem( objectID id, MSG_Name name, MSG_Data& data = MSG_Data() );
	void SendMsgFromSystem( GameObject* object, MSG_Name name, MSG_Data& data = MSG_Data() );
	void SendMsgFromSystem( MSG_Name name, MSG_Data& data = MSG_Data() );

	void Store( GameObject & object );
	void Remove( objectID id );
	GameObject* Find( objectID id );
	objectID GetIDByName( char* name );

	objectID GetNewObjectID( void );
	
	void ComposeList( dbCompositionList & list, unsigned int type = 0 );


private:

	typedef std::list<GameObject*> dbContainer;

	//Make this a more efficient data structure (like a hash table)
	dbContainer m_database;

	objectID m_nextFreeID;


};
