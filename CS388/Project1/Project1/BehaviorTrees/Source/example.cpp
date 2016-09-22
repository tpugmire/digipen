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

//Add new states below
enum StateName {
	STATE_Initialize,	//The first enum is the starting state
	STATE_PickPlayerToChase,
	STATE_Idle,
	STATE_MoveToRandomTarget
};

//Add new substates below
enum SubstateName {		//Order doesn't matter
	//empty
};

bool Example::States( State_Machine_Event event, MSG_Object * msg, int state, int substate )
{
BeginStateMachine

	//Global message response section
	OnMsg( MSG_Reset )
		ResetStateMachine();

	OnMsg( MSG_MouseClick )
		m_owner->GetMovement().SetTarget( msg->GetVector3Data() );



	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Initialize )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			ChangeStateDelayed( 1.0f, STATE_Idle );
			//m_owner->GetTiny().SetDiffuse(1.0f, 0.0f, 0.0f);	//Color Tiny (Example)
	

	///////////////////////////////////////////////////////////////
	DeclareState( STATE_PickPlayerToChase )

		OnEnter
			m_owner->GetMovement().SetWalkSpeed();
			m_curTarget = GetFarthestAgent();
			if( m_curTarget == 0 ) {
				ChangeState( STATE_MoveToRandomTarget );
			}
			SendMsgToStateMachineNow( MSG_SetTargetPosition );

		OnMsg( MSG_SetTargetPosition )
			GameObject* go = g_database.Find( m_curTarget );
			if( go ) {
				D3DXVECTOR3 target = go->GetBody().GetPos();
				m_owner->GetMovement().SetTarget( target );
			}
			SendMsgToState( MSG_SetTargetPosition );

		OnMsg( MSG_Arrived )
			ChangeState( STATE_Idle );


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_Idle )

		OnEnter
			m_owner->GetMovement().SetIdleSpeed();
			if( rand()%2 == 0 ) {
				ChangeStateDelayed( RandDelay( 1.0f, 2.0f ), STATE_MoveToRandomTarget );
			}
			else {
				ChangeStateDelayed( RandDelay( 1.0f, 2.0f ), STATE_PickPlayerToChase );
			}


	///////////////////////////////////////////////////////////////
	DeclareState( STATE_MoveToRandomTarget )

		OnEnter
			m_owner->GetMovement().SetJogSpeed(); 
			D3DXVECTOR3 target( 0, 0, 0 );
			target.x = (float) ( rand() % 256 ) / 256.f;
			target.z = (float) ( rand() % 256 ) / 256.f;
			m_owner->GetMovement().SetTarget( target );

		OnMsg( MSG_Arrived )
			ChangeState( STATE_Idle );


EndStateMachine
}


objectID Example::GetFarthestAgent( void )
{
	float farthestDistance = 0.0f;
	GameObject* farthestGameObject = 0;
	dbCompositionList list;
	g_database.ComposeList( list, OBJECT_NPC );
	
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		if( (*i)->GetID() != m_owner->GetID() )
		{
			D3DXVECTOR3 npcPos = (*i)->GetBody().GetPos();
			D3DXVECTOR3 myPos = m_owner->GetBody().GetPos();
			D3DXVECTOR3 diff = npcPos - myPos;
			float distance = D3DXVec3Length( &diff );

			if( farthestGameObject )
			{
				if( distance > farthestDistance )
				{
					farthestDistance = distance;
					farthestGameObject = *i;
				}
			}
			else
			{
				farthestDistance = distance;
				farthestGameObject = *i;
			}
		}
	}

	return( farthestGameObject->GetID() );
}
