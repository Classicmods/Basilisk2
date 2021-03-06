/*
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <errno.h>

#include "sysdeps.h"
#include "macos_util.h"
#include "timer.h"

#include "retro_miscellaneous.h"
#include "features/features_cpu.h"

#define DEBUG 0
#include "debug.h"

/*
 *  Return microseconds since boot (64 bit)
 */

void Microseconds(uint32 &hi, uint32 &lo)
{
	D(bug("Microseconds\n"));
   
   retro_time_t currenttime = cpu_features_get_time_usec();
   hi = currenttime >> 32;
   lo = currenttime & 0xFFFFFFFF;
}


/*
 *  Return local date/time in Mac format (seconds since 1.1.1904)
 */

uint32 TimerDateTime(void)
{
	return TimeToMacTime(time(NULL));
   //return TimeToMacTime(cpu_features_get_time_usec() / 1000000);
}


/*
 *  Get current time
 */

void timer_current_time(tm_time_t &t)
{
	//time(&t);
   clock_gettime(CLOCK_REALTIME, &t);
}


/*
 *  Add times
 */

void timer_add_time(tm_time_t &res, tm_time_t a, tm_time_t b)
{
   /*
	res.tv_usec = a.tv_usec + b.tv_usec;
	if (res.tv_usec >= 1000000) {
		res.tv_sec++;
		res.tv_usec -= 1000000;
	}
   */
   res.tv_sec = a.tv_sec + b.tv_sec;
}


/*
 *  Subtract times
 */

void timer_sub_time(tm_time_t &res, tm_time_t a, tm_time_t b)
{
   /*
	res.tv_usec = a.tv_usec - b.tv_usec;
	if (res.tv_usec < 0) {
		res.tv_sec--;
		res.tv_usec += 1000000;
	}
   */
   res.tv_sec = a.tv_sec - b.tv_sec;
}


/*
 *  Compare times (<0: a < b, =0: a = b, >0: a > b)
 */

int timer_cmp_time(tm_time_t a, tm_time_t b)
{
   /*
	if (a.tv_sec == b.tv_sec)
		return a.tv_usec - b.tv_usec;
	else
		return a.tv_sec - b.tv_sec;
   */
   return a.tv_sec - b.tv_sec;
}


/*
 *  Convert Mac time value (>0: microseconds, <0: microseconds) to tm_time_t
 */

void timer_mac2host_time(tm_time_t &res, int32 mactime)
{
	if (mactime > 0) {
		// Time in milliseconds
		res.tv_sec = mactime / 1000;
		//res.tv_usec = (mactime % 1000) * 1000;
	} else {
		// Time in negative microseconds
		res.tv_sec = -mactime / 1000000;
		//res.tv_usec = -mactime % 1000000;
	}
}


/*
 *  Convert positive tm_time_t to Mac time value (>0: microseconds, <0: microseconds)
 *  A negative input value for hosttime results in a zero return value
 *  As long as the microseconds value fits in 32 bit, it must not be converted to milliseconds!
 */

int32 timer_host2mac_time(tm_time_t hosttime)
{
	if (hosttime.tv_sec < 0)
		return 0;
	else {
		//uint64 t = (uint64)hosttime.tv_sec * 1000000 + hosttime.tv_usec;
      uint64 t = (uint64)hosttime.tv_sec * 1000000;
      
		if (t > 0x7fffffff)
			return t / 1000;	// Time in milliseconds
		else
			return -t;			// Time in negative microseconds
	}
}


/*
 *  Get current value of microsecond timer
 */

uint64 GetTicks_usec(void)
{
   /*
	struct timeval t;
	gettimeofday(&t, NULL);
	return (uint64)t.tv_sec * 1000000 + t.tv_usec;
   */
   //return cpu_features_get_time_usec();
   tm_time_t t;
   clock_gettime(CLOCK_REALTIME, &t);
   return (uint64)t.tv_sec * 1000000;
}

void Delay_usec(uint32 usec)
{
   retro_sleep(usec / 1000);
}


/*
 *  Suspend emulator thread, virtual CPU in idle mode
 */



void idle_wait(void)
{
	//Sleep 10 ms
	Delay_usec(10000);
}


/*
 *  Resume execution of emulator thread, events just arrived
 */

void idle_resume(void)
{
   //nothing special is required to resume when using single thread
}
