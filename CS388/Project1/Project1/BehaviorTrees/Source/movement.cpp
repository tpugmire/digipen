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

Movement::Movement( GameObject& owner )
: m_owner( &owner ),
  m_speedWalk( 1.f / 5.7f ),
  m_speedJog( 1.f / 2.3f )
{
	m_target.x = m_target.y = m_target.z = 0.0f;
}

Movement::~Movement( void )
{

}

void Movement::Animate( double dTimeDelta )
{
	if( m_owner->GetBody().GetSpeed() == 0.0f )
	{
		m_owner->GetTiny().SmoothLoiter();
	}
	else
	{
		float speed = m_owner->GetBody().GetSpeed();
		D3DXVECTOR3 pos = m_owner->GetBody().GetPos();
		D3DXVECTOR3 toTarget = m_target - pos;

		if( D3DXVec3Length( &toTarget ) < 0.01f )
		{	//Notify target reached
			//m_owner->GetBody().SetPos( m_target );
			g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
		}
		else
		{	
			//Point character towards target this frame
			D3DXVECTOR3 dir;
			D3DXVec3Normalize( &dir, &toTarget );
			m_owner->GetBody().SetDir( dir );

			//Move character towards target this frame
			double speedScale = m_owner->GetTiny().GetSpeedScale();
			D3DXVec3Scale( &dir, &dir, float( m_owner->GetBody().GetSpeed() * speedScale * dTimeDelta ) );

			D3DXVECTOR3 newPos;
			D3DXVec3Add( &newPos, &dir, &m_owner->GetBody().GetPos() );
			m_owner->GetBody().SetPos( newPos );
		}
	}

	m_owner->GetTiny().SetOrientation();

}

void Movement::SetIdleSpeed( void )
{
	m_owner->GetBody().SetSpeed( 0.0f );
	m_owner->GetTiny().SetIdleKey( true );
}

void Movement::SetWalkSpeed( void )
{
	m_owner->GetBody().SetSpeed( m_speedWalk );
	m_owner->GetTiny().SetMoveKey();
}

void Movement::SetJogSpeed( void )
{
	m_owner->GetBody().SetSpeed( m_speedJog );
	m_owner->GetTiny().SetMoveKey();
}
