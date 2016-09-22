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

Clock::Clock(void)
{
	m_currentTime = 0.0f;
	m_timeLastTick = 0.001f;
	m_startTime = timeGetTime();
}

/*---------------------------------------------------------------------------*
  Name:         MarkTimeThisTick

  Description:  Marks the current time for this tick (frame). This can be
				referenced during the frame to simulate a consistent moment 
				in time.
  
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void Clock::MarkTimeThisTick(void)
{
	unsigned int timeInMS = timeGetTime();
	float newTime = (timeInMS - m_startTime) / 1000.0f;

	m_timeLastTick = newTime - m_currentTime;
	m_currentTime = newTime;

	if( m_timeLastTick <= 0.0f ) {
		m_timeLastTick = 0.001f;
	}

}


