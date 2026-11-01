/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////


/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                     $Archive:: /RedAlert2/NAT.CPP                                          $*
 *                                                                                             *
 *                      $Author:: Steve_t                                                     $*
 *                                                                                             *
 *                     $Modtime:: 3/15/01 12:00PM                                             $*
 *                                                                                             *
 *                    $Revision:: 1                                                           $*
 *                                                                                             *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 *                                                                                             *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 *                                                                                             *
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once

#ifndef __FIREWALLHELPER_H
#define __FIREWALLHELPER_H

// The firewall detection that produced these values was part of online play (GameSpy) and was
// removed. The type is kept because every slot in the game options string (LAN games, replays)
// carries it as its NAT field.
class FirewallHelperClass {

	public:

		/*
		** Enumeration of firewall behaviors we can detect.
		**
		** It is assumed that all port mangling firewalls change the mangled source port in response to
		** an application source port change.
		*/
		typedef enum tFirewallBehaviorType {

			FIREWALL_MIN = 0,

			/*
			** Just used as an initialiser.
			*/
			FIREWALL_TYPE_UNKNOWN = 0,

			/*
			** This is a simple, non-port translating firewall, or there is no firewall at all.
			*/
			FIREWALL_TYPE_SIMPLE = 1,

			/*
			** This is a firewall/NAT with port mangling but it's pretty dumb - it uses the same mangled
			** source port regardless of the destination address.
			*/
			FIREWALL_TYPE_DUMB_MANGLING = 2,

			/*
			** This is a smarter firewall/NAT with port mangling that uses different mangled source ports
			** for different destination IPs.
			*/
			FIREWALL_TYPE_SMART_MANGLING = 4,

			/*
			** This is a firewall that exhibits the bug as seen in the Netgear firewalls. A previously good
			** source port mapping will change in response to unsolicited traffic from a known IP.
			*/
			FIREWALL_TYPE_NETGEAR_BUG = 8,

			/*
			** This firewall has a simple absolute offset port allocation scheme.
			*/
			FIREWALL_TYPE_SIMPLE_PORT_ALLOCATION = 16,

			/*
			** This firewall has a relative offset port allocation scheme. For these firewalls, we have to
			** subtract the actual source port from the mangled source port to discover the allocation scheme.
			** The mangled port number is based in part on the original source port number.
			*/
			FIREWALL_TYPE_RELATIVE_PORT_ALLOCATION = 32,

			/*
			** This firewall mangles source ports differently depending on the destination port.
			*/
			FIREWALL_TYPE_DESTINATION_PORT_DELTA = 64,

			FIREWALL_MAX = 128

		} FirewallBehaviorType;
};


#endif	// __FIREWALLHELPER_H

