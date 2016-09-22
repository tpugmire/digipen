/* Copyright Steve Rabin, 2013. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2013"
 */

#include <Stdafx.h>

GameObject::GameObject( objectID id, unsigned int type, char* name )
: m_markedForDeletion(false),
  m_stateMachineManager(0),
  m_body(0),
  m_movement(0),
  m_tiny(0),
  m_targetPOS(0,0,0)
{
	m_id = id;
	m_type = type;
	
	if( strlen(name) < GAME_OBJECT_MAX_NAME_SIZE ) {
		strcpy( m_name, name );
	}
	else {
		strcpy( m_name, "invalid_name" );
		ASSERTMSG(0, "GameObject::GameObject - name is too long" );
	}
}

GameObject::~GameObject( void )
{
	if(m_stateMachineManager)
	{
		delete m_stateMachineManager;
	}
	if(m_body)
	{
		delete m_body;
	}
	if(m_movement)
	{
		delete m_movement;
	}
	if(m_tiny)
	{
		delete m_tiny;
	}
}

void GameObject::CreateStateMachineManager( void )
{
  ASSERTMSG(!HasStateMachine(), "GameObject::CreateStateMachine: Tried to add a state machine to an agent that already has a state machine.");
	m_stateMachineManager = new StateMachineManager( *this );
}

void GameObject::CreateMovement( void )
{
	m_movement = new Movement( *this ); 
}

void GameObject::CreateBody( int health, D3DXVECTOR3& pos )
{
	m_body = new Body( health, pos, *this );
}

void GameObject::CreateTiny( CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent, float red, float green, float blue )
{
	m_tiny = new CTiny( *this );
	if( m_tiny == NULL )
	{
		return;
	}

	if( SUCCEEDED( m_tiny->Setup( pMA, pv_pChars, pSM, dTimeCurrent ) ) )
	{
		m_tiny->SetSounds( true );
		m_tiny->SetDiffuse(red, green, blue);
	}
	else
	{
		delete m_tiny;
	}
}

void GameObject::Initialize( void )
{

}

/*---------------------------------------------------------------------------*
  Name:         Update

  Description:  Calls the update function of the currect state machine.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void GameObject::Update( void )
{
	if(m_stateMachineManager)
	{
		m_stateMachineManager->Update();
	}
}

void GameObject::Animate( double dTimeDelta )
{
  if (DXUTIsTimePaused()) return;

	if( m_movement )
	{
		m_movement->Animate( dTimeDelta );
	}
}

void GameObject::AdvanceTime( double dTimeDelta, D3DXVECTOR3 *pvEye )
{
	if( m_tiny )
	{
		m_tiny->AdvanceTime( dTimeDelta, pvEye );
	}
}

void GameObject::Draw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj )
{
	if( m_tiny )
	{
		m_tiny->Draw();
	}
}

void GameObject::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_tiny )
	{
		m_tiny->RestoreDeviceObjects( pd3dDevice );
	}
}

void GameObject::InvalidateDeviceObjects( void )
{

}

int GetDepth(std::string &line)
{
	int depth = 0;

	for (unsigned int i = 0; i < line.size(); ++i)
	{
		if (line[i] == '\t')
		{
			depth++;
		}
		else
		{
			break;
		}
	}

	return depth;
}
