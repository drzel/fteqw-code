/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// cmd.h -- Command buffer and command execution

//===========================================================================

/*

Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but remote
servers can also send across commands and entire text files can be execed.

The + command line options are also added to the command buffer.

The game starts with a Cbuf_AddText ("exec quake.rc\n"); Cbuf_Execute ();

*/


void Cbuf_Init (void);
// allocates an initial text buffer that will grow as needed

void Cbuf_AddText (const char *text, int level);
// as new commands are generated from the console or keybindings,
// the text is added to the end of the command buffer.

void Cbuf_InsertText (const char *text, int level);
// when a command wants to issue other commands immediately, the text is
// inserted at the beginning of the buffer, before any remaining unexecuted
// commands.

char *Cbuf_GetNext(int level);

void Cbuf_Execute (void);
// Pulls off \n terminated lines of text from the command buffer and sends
// them through Cmd_ExecuteString.  Stops when the buffer is empty.
// Normally called once per frame, but may be explicitly invoked.
// Do not call inside a command function!

//===========================================================================

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

*/

typedef void (*xcommand_t) (void);

int Cmd_Level(char *name);

void	Cmd_Init (void);

void	Cmd_RemoveCommand (char *cmd_name);
void	Cmd_AddRemCommand (char *cmd_name, xcommand_t function);	//removable command
void	Cmd_AddCommand (char *cmd_name, xcommand_t function);
// called by the init functions of other parts of the program to
// register commands and functions to call for them.
// The cmd_name is referenced later, so it should not be in temp memory
// if function is NULL, the command will be forwarded to the server
// as a clc_stringcmd instead of executed locally

qboolean Cmd_Exists (char *cmd_name);
// used by the cvar code to check for cvar / command name overlap

char *Cmd_CompleteCommand (char *partial, qboolean fullonly, int matchnum);
// attempts to match a partial command for automatic command line completion
// returns NULL if nothing fits

int		Cmd_Argc (void);
char	*Cmd_Argv (int arg);
char	*Cmd_Args (void);
extern int	Cmd_ExecLevel;
// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are allways safe.

int Cmd_CheckParm (char *parm);
// Returns the position (1 to argc-1) in the command's argument list
// where the given parameter apears, or 0 if not present

char *Cmd_AliasExist(char *name, int restrictionlevel);
void Alias_WipeStuffedAliaes(void);

void Cmd_TokenizeString (char *text);
// Takes a null terminated string.  Does not need to be /n terminated.
// breaks the string up into arg tokens.

void	Cmd_ExecuteString (char *text, int restrictionlevel);

#define RESTRICT_MAX		64	//1-64	it's all about bit size. This is max settable. servers are +1
#define RESTRICT_DEFAULT	50	//rcon get's 63, local always gets 64
#define RESTRICT_MIN		1	//rcon get's 63, local always gets 64

#define RESTRICT_LOCAL	RESTRICT_MAX
#define RESTRICT_SERVER	RESTRICT_MAX+1
#define RESTRICT_RCON	rcon_level.value
#define RESTRICT_PROGS	RESTRICT_MAX-2

#define Cmd_FromServer() (Cmd_ExecLevel>=RESTRICT_SERVER)

// Parses a single line of text into arguments and tries to execute it
// as if it was typed at the console

void	Cmd_ForwardToServer (void);
// adds the current command line as a clc_stringcmd to the client message.
// things like godmode, noclip, etc, are commands directed to the server,
// so when they are typed in at the console, they will need to be forwarded.

qboolean Cmd_FilterMessage (char *message, qboolean sameteam);
void Cmd_MessageTrigger (char *message, int type);

void Cmd_StuffCmds_f (void);

void Cmd_ShiftArgs (int ammount);

char		*Cmd_ExpandString (char *input, int maxaccesslevel);

extern cvar_t rcon_level;

