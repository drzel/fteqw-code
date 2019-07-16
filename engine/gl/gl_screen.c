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

// screen.c -- master for refresh, status bar, console, chat, notify, etc

#include "quakedef.h"
extern qboolean gammaworks;
#ifdef GLQUAKE
#include "glquake.h"
#include "shader.h"
#include "gl_draw.h"

#include <time.h>

qboolean GLSCR_UpdateScreen (void);


extern qboolean	scr_drawdialog;

extern cvar_t vid_triplebuffer;
extern cvar_t          scr_fov;

extern qboolean        scr_initialized;
extern float oldsbar;
extern qboolean        scr_drawloading;

extern cvar_t vid_conautoscale;
extern qboolean		scr_con_forcedraw;
extern qboolean		depthcleared;

/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.

WARNING: be very careful calling this from elsewhere, because the refresh
needs almost the entire 256k of stack space!
==================
*/
void SCR_DrawCursor(void);
qboolean GLSCR_UpdateScreen (void)
{
	int uimenu;
	qboolean nohud;
	qboolean noworld;
	extern cvar_t vid_srgb;

	r_refdef.pxrect.maxheight = vid.pixelheight;

	vid.numpages = 2 + vid_triplebuffer.value;

	if (!scr_initialized || !con_initialized)
	{
		return false;                         // not initialized yet
	}

	R2D_Font_Changed();

	if (vid_srgb.modified)
	{
		vid_srgb.modified = false;

		//vid_srgb can be changed between 0 and 1, but other values need texture reloads. do that without too much extra weirdness.
		if ((vid.flags & VID_SRGB_CAPABLE) && gl_config.arb_framebuffer_srgb)
		{	//srgb-capable
			if (vid_srgb.ival > 0 && (vid.flags & VID_SRGBAWARE))
			{	//full srgb wanted (and textures are loaded)
				qglEnable(GL_FRAMEBUFFER_SRGB);
				vid.flags |= VID_SRGB_FB_LINEAR;
			}
			else if (vid_srgb.ival < 0 || (vid.flags & VID_SRGBAWARE))
			{	//srgb wanted only for the framebuffer, for gamma tricks.
				qglEnable(GL_FRAMEBUFFER_SRGB);
				vid.flags |= VID_SRGB_FB_LINEAR;
			}
			else
			{	//srgb not wanted...
				qglDisable(GL_FRAMEBUFFER_SRGB);
				vid.flags &= ~VID_SRGB_FB_LINEAR;
			}
		}
	}

	if (scr_disabled_for_loading)
	{
		extern float scr_disabled_time;
		if (Sys_DoubleTime() - scr_disabled_time > 60 || !Key_Dest_Has(~kdm_game))
		{
			//FIXME: instead of reenabling the screen, we should just draw the relevent things skipping only the game.
			scr_disabled_for_loading = false;
		}
		else
		{
//			scr_drawloading = true;
			SCR_DrawLoading (true);
			SCR_SetUpToDrawConsole();
			if (Key_Dest_Has(kdm_console))
				SCR_DrawConsole(false);
//			scr_drawloading = false;
			if (R2D_Flush)
				R2D_Flush();
			VID_SwapBuffers();
			return true;
		}
	}


	Shader_DoReload();

	qglDisable(GL_SCISSOR_TEST);
#ifdef VM_UI
	uimenu = UI_MenuState();
#else
	uimenu = 0;
#endif

#ifdef TEXTEDITOR
	if (editormodal)
	{
		Editor_Draw();
		V_UpdatePalette (false);
		R2D_BrightenScreen();
		Media_RecordFrame();

		if (key_dest_mask & kdm_console)
			Con_DrawConsole(vid.height/2, false);
		else
			Con_DrawConsole(0, false);
		SCR_DrawCursor();
	}
	else
#endif
	if (Media_ShowFilm())
	{
		M_Draw(0);
		V_UpdatePalette (false);
		R2D_BrightenScreen();
		Media_RecordFrame();
	}
	else
	{
		//
		// do 3D refresh drawing, and then update the screen
		//
		SCR_SetUpToDrawConsole ();

		noworld = false;
		nohud = false;

		if (r_clear.ival)
		{
			GL_ForceDepthWritable();
			qglClearColor((r_clear.ival&1)?1:0, (r_clear.ival&2)?1:0, (r_clear.ival&4)?1:0, 1);
			qglClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			depthcleared = true;
		}

#ifdef VM_CG
		if (CG_Refresh())
			nohud = true;
		else
#endif
#ifdef CSQC_DAT
			if (CSQC_DrawView())
			nohud = true;
		else
#endif
		{
			if (uimenu != 1)
			{
				if (r_worldentity.model && cls.state == ca_active)
					V_RenderView (nohud);
				else
				{
					noworld = true;
				}
			}
		}

		GL_Set2D (false);

		scr_con_forcedraw = false;
		if (noworld)
		{
			//draw the levelshot or the conback fullscreen
			if (R2D_DrawLevelshot())
				;
			else if (scr_con_current != vid.height)
				R2D_ConsoleBackground(0, vid.height, true);
			else
				scr_con_forcedraw = true;

			nohud = true;
		}

		SCR_DrawTwoDimensional(uimenu, nohud);

		V_UpdatePalette (false);
		R2D_BrightenScreen();
		Media_RecordFrame();
	}

	RSpeedShow();
	if (R2D_Flush)
		R2D_Flush();

	{
		RSpeedMark();
		VID_SwapBuffers();
		RSpeedEnd(RSPEED_PRESENT);
	}

	//gl 4.5 / GL_ARB_robustness / GL_KHR_robustness
	if (qglGetGraphicsResetStatus)
	{
		GLenum err = qglGetGraphicsResetStatus();
		switch(err)
		{
		case GL_NO_ERROR:
			break;
		case GL_GUILTY_CONTEXT_RESET:	//we did it
		case GL_INNOCENT_CONTEXT_RESET:	//something else broke the hardware and broke our ram
		case GL_UNKNOWN_CONTEXT_RESET:	//whodunit
		default:
			Con_Printf("OpenGL reset detected\n");
			Sys_Sleep(3.0);
			Cmd_ExecuteString("vid_restart", RESTRICT_LOCAL);
			break;
		}
	}
	return true;
}


char *GLVID_GetRGBInfo(int *bytestride, int *truewidth, int *trueheight, enum uploadfmt *fmt)
{	//returns a BZ_Malloced array
	extern qboolean gammaworks;
	int i, c;
	qbyte *ret;
	extern qboolean r2d_canhwgamma;

	*bytestride = 0;
	*truewidth = vid.fbpwidth;
	*trueheight = vid.fbpheight;

	/*if (1)
	{
		float *p;

		p = BZ_Malloc(vid.pixelwidth*vid.pixelheight*sizeof(float));
		qglReadPixels (0, 0, vid.pixelwidth, vid.pixelheight, GL_DEPTH_COMPONENT, GL_FLOAT, p); 

		ret = BZ_Malloc(vid.pixelwidth*vid.pixelheight*3);

		c = vid.pixelwidth*vid.pixelheight;
		for (i = 1; i < c; i++)
		{
			ret[i*3+0]=p[i]*p[i]*p[i]*255;
			ret[i*3+1]=p[i]*p[i]*p[i]*255;
			ret[i*3+2]=p[i]*p[i]*p[i]*255;
		}
		BZ_Free(p);
	}
	else*/ if (gl_config.gles || (*truewidth&3))
	{
		qbyte *p;

		//gles:
		//Only two format/type parameter pairs are accepted.
		//GL_RGBA/GL_UNSIGNED_BYTE is always accepted, and the other acceptable pair can be discovered by querying GL_IMPLEMENTATION_COLOR_READ_FORMAT and GL_IMPLEMENTATION_COLOR_READ_TYPE.
		//thus its simpler to only use GL_RGBA/GL_UNSIGNED_BYTE
		//desktopgl:
		//total line byte length must be aligned to GL_PACK_ALIGNMENT. by reading rgba instead of rgb, we can ensure the line is a multiple of 4 bytes.

		ret = BZ_Malloc((*truewidth)*(*trueheight)*4);
		qglReadPixels (0, 0, (*truewidth), (*trueheight), GL_RGBA, GL_UNSIGNED_BYTE, ret);
		*bytestride = *truewidth*-3;

		*fmt = TF_RGB24;
		c = (*truewidth)*(*trueheight);
		p = ret;
		for (i = 1; i < c; i++)
		{
			p[i*3+0]=p[i*4+0];
			p[i*3+1]=p[i*4+1];
			p[i*3+2]=p[i*4+2];
		}
		ret = BZ_Realloc(ret, (*truewidth)*(*trueheight)*3);
	}
#if 1//def _DEBUG
	else if (!gl_config.gles && sh_config.texfmt[PTI_BGRA8])
	{
		*fmt = TF_BGRA32;
		ret = BZ_Malloc((*truewidth)*(*trueheight)*4);
		qglReadPixels (0, 0, (*truewidth), (*trueheight), GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, ret); 
		*bytestride = *truewidth*-4;
	}
#endif
	else
	{
		*fmt = TF_RGB24;
		ret = BZ_Malloc((*truewidth)*(*trueheight)*3);
		qglReadPixels (0, 0, (*truewidth), (*trueheight), GL_RGB, GL_UNSIGNED_BYTE, ret); 
		*bytestride = *truewidth*-3;
	}

	if (gammaworks && r2d_canhwgamma)
	{
		if (*fmt == TF_BGRA32 || *fmt == TF_RGBA32)
		{
			c = (*truewidth)*(*trueheight)*4;
			for (i=0 ; i<c ; i+=4)
			{
				extern qbyte		gammatable[256];
				ret[i+0] = gammatable[ret[i+0]];
				ret[i+1] = gammatable[ret[i+1]];
				ret[i+2] = gammatable[ret[i+2]];
			}
		}
		else
		{
			c = (*truewidth)*(*trueheight)*3;
			for (i=0 ; i<c ; i+=3)
			{
				extern qbyte		gammatable[256];
				ret[i+0] = gammatable[ret[i+0]];
				ret[i+1] = gammatable[ret[i+1]];
				ret[i+2] = gammatable[ret[i+2]];
			}
		}
	}
	
	return ret;
}
#endif
