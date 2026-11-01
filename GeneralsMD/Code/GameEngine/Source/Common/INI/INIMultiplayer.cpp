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

// FILE: INIMultiplayer.cpp ///////////////////////////////////////////////////////////////////////////
// Author: Matthew D. Campbell, January 2002
// Desc:   Parsing MultiplayerSettings and MultiplayerColor INI entries
///////////////////////////////////////////////////////////////////////////////////////////////////

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////
#include "PreRTS.h"	// This must go first in EVERY cpp file int the GameEngine

#include "Common/INI.h"
#include "Common/MultiplayerSettings.h"

void INI::parseMultiplayerSettingsDefinition( INI* ini )
{
	if( TheMultiplayerSettings )
	{
		// 
		// if the type of loading we're doing creates override data, we need to
		// be loading into a new override item
		//
		if( ini->getLoadType() == INI_LOAD_CREATE_OVERRIDES )
		{
			DEBUG_ASSERTCRASH(false, ("Creating an override of MultiplayerSettings!"));
		}
	}  // end if
	else
	{
		// we don't have any multiplayer settings instance at all yet, create one
		TheMultiplayerSettings = NEW MultiplayerSettings;
	}  // end else

	// parse the ini definition
	ini->initFromINI( TheMultiplayerSettings, TheMultiplayerSettings->getFieldParse() );
}

void INI::parseMultiplayerColorDefinition( INI* ini )
{
	const char *c;
	AsciiString name;
	MultiplayerColorDefinition *multiplayerColorDefinition;

	// read the name
	c = ini->getNextToken();
	name.set( c );	

	// find existing item if present, but this type does not allow overrides, 
	//so if it exists just overwrite it.
	multiplayerColorDefinition = TheMultiplayerSettings->findMultiplayerColorDefinitionByName( name );
	if( multiplayerColorDefinition == NULL )
		multiplayerColorDefinition = TheMultiplayerSettings->newMultiplayerColorDefinition( name );

	ini->initFromINI( multiplayerColorDefinition, multiplayerColorDefinition->getFieldParse() );

	multiplayerColorDefinition->setColor(multiplayerColorDefinition->getRGBValue());
	multiplayerColorDefinition->setNightColor(multiplayerColorDefinition->getRGBNightValue());
}

namespace
{
  struct MultiplayerStartingMoneySettings
  {
    Money money;
    Bool  isDefault;
  };
  
  const FieldParse startingMoneyFieldParseTable[] = 
  {
    { "Value",			  Money::parseMoneyAmount,	NULL,	offsetof( MultiplayerStartingMoneySettings, money ) },
    { "Default",	   	INI::parseBool,         	NULL,	offsetof( MultiplayerStartingMoneySettings, isDefault ) },
    { NULL,	NULL,	NULL,	0 }  // keep this last
  };
}


void INI::parseMultiplayerStartingMoneyChoiceDefinition( INI* ini )
{
  DEBUG_ASSERTCRASH( ini->getLoadType() != INI_LOAD_CREATE_OVERRIDES, ("Overrides not supported for MultiplayerStartingMoneyChoice") );
  
  // Temporary data store
  MultiplayerStartingMoneySettings settings;
  settings.isDefault = false;
  
  ini->initFromINI( &settings, startingMoneyFieldParseTable );
  
  TheMultiplayerSettings->addStartingMoneyChoice( settings.money, settings.isDefault );
}

// The OnlineChatColors parser and colour table moved here from the GameSpy chat code, which was
// removed with online play.
#define OFFSET(x) (sizeof(Int) * (x))
static const FieldParse GameSpyColorFieldParse[] = 
{

	{ "Default",						INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_DEFAULT) },
	{ "CurrentRoom",				INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CURRENTROOM) },
	{ "ChatRoom",						INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_ROOM) },
	{ "Game",								INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_GAME) },
	{ "GameFull",						INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_GAME_FULL) },
	{ "GameCRCMismatch",		INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_GAME_CRCMISMATCH) },
	{ "PlayerNormal",				INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_PLAYER_NORMAL) },
	{ "PlayerOwner",				INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_PLAYER_OWNER) },
	{ "PlayerBuddy",				INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_PLAYER_BUDDY) },
	{ "PlayerSelf",					INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_PLAYER_SELF) },
	{ "PlayerIgnored",			INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_PLAYER_IGNORED) },
	{ "ChatNormal",					INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_NORMAL) },
	{ "ChatEmote",					INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_EMOTE) },
	{ "ChatOwner",					INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_OWNER) },
	{ "ChatOwnerEmote",			INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_OWNER_EMOTE) },
	{ "ChatPriv",						INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_PRIVATE) },
	{ "ChatPrivEmote",			INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_PRIVATE_EMOTE) },
	{ "ChatPrivOwner",			INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_PRIVATE_OWNER) },
	{ "ChatPrivOwnerEmote",	INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_PRIVATE_OWNER_EMOTE) },
	{ "ChatBuddy",					INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_BUDDY) },
	{ "ChatSelf",						INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_CHAT_SELF) },
	{ "AcceptTrue",					INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_ACCEPT_TRUE) },
	{ "AcceptFalse",				INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_ACCEPT_FALSE) },
	{ "MapSelected",				INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_MAP_SELECTED) },
	{ "MapUnselected",			INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_MAP_UNSELECTED) },
	{ "MOTD",								INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_MOTD) },
	{ "MOTDHeading",				INI::parseColorInt,	NULL,	OFFSET(GSCOLOR_MOTD_HEADING) },

	{ NULL,					NULL,						NULL,						0 }  // keep this last

};

void INI::parseOnlineChatColorDefinition( INI* ini )
{
	// parse the ini definition
	ini->initFromINI( GameSpyColor, GameSpyColorFieldParse );
}


Color GameSpyColor[GSCOLOR_MAX] =
{
	GameMakeColor(255,255,255,255),	// GSCOLOR_DEFAULT
	GameMakeColor(255,255,  0,255),	// GSCOLOR_CURRENTROOM
	GameMakeColor(255,255,255,255),	// GSCOLOR_ROOM
	GameMakeColor(128,128,0,255),		// GSCOLOR_GAME
	GameMakeColor(128,128,128,255),	// GSCOLOR_GAME_FULL
	GameMakeColor(128,128,128,255),	// GSCOLOR_GAME_CRCMISMATCH
	GameMakeColor(255,255,255,255),	// GSCOLOR_PLAYER_NORMAL
	GameMakeColor(255,  0,255,255),	// GSCOLOR_PLAYER_OWNER
	GameMakeColor(255,  0,128,255),	// GSCOLOR_PLAYER_BUDDY
	GameMakeColor(255,  0,  0,255),	// GSCOLOR_PLAYER_SELF
	GameMakeColor(128,128,128,255),	// GSCOLOR_PLAYER_IGNORED
	GameMakeColor(255,255,255,255),		// GSCOLOR_CHAT_NORMAL
	GameMakeColor(255,128,0,255),		// GSCOLOR_CHAT_EMOTE,
	GameMakeColor(255,255,0,255),		// GSCOLOR_CHAT_OWNER,
	GameMakeColor(128,255,0,255),		// GSCOLOR_CHAT_OWNER_EMOTE,
	GameMakeColor(0,0,255,255),			// GSCOLOR_CHAT_PRIVATE,
	GameMakeColor(0,255,255,255),		// GSCOLOR_CHAT_PRIVATE_EMOTE,
	GameMakeColor(255,0,255,255),		// GSCOLOR_CHAT_PRIVATE_OWNER,
	GameMakeColor(255,128,255,255),	// GSCOLOR_CHAT_PRIVATE_OWNER_EMOTE,
	GameMakeColor(255,  0,255,255),	// GSCOLOR_CHAT_BUDDY,
	GameMakeColor(255,  0,128,255),	// GSCOLOR_CHAT_SELF,
	GameMakeColor(  0,255,  0,255),	// GSCOLOR_ACCEPT_TRUE,
	GameMakeColor(255,  0,  0,255),	// GSCOLOR_ACCEPT_FALSE,
	GameMakeColor(255,255,  0,255),	// GSCOLOR_MAP_SELECTED,
	GameMakeColor(255,255,255,255),	// GSCOLOR_MAP_UNSELECTED,
	GameMakeColor(255,255,255,255),	// GSCOLOR_MOTD,
	GameMakeColor(255,255,  0,255),	// GSCOLOR_MOTD_HEADING,
};
