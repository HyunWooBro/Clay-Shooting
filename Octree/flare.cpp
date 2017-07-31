/* Copyright (C) Yossarian King, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Yossarian King, 2000"
 */
/************************************************************

    Game Programming Gems
    Lens Flare Demo

    Y. King / May 2000
    Electronic Arts Canada, Inc.

 ************************************************************/

// Modified by Alan Baylis 20/10/2002

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "flare.h"
#include "texture.h"
#include "vector.h"
#include "Camera.h"
#include "ZFLog.h"

extern CCamera	g_Camera;
extern float SunPosition[3];
extern TEXTURE* texture;
//extern CCAMERA* camera;
//extern int currentCamera;
extern HWND g_hWnd;
extern RECT g_rRect;
extern PAINTSTRUCT ps;
extern ZFLog* g_pLog;

/************************************************************
    Defines.
 ************************************************************/

// Get height from width, assuming certain aspect ratio.
#define HEIGHTFROMWIDTH(w)  ((w)*SCREENheight/SCREENwidth)

#define isqrt(x)        (int)((double)(x))

/************************************************************
    Texture management.
 ************************************************************/

typedef struct TEXTURE_DEF
{
    char* filename;
    TEXTURE* FlareTexture;
}
    TEXTURE_DEF;

// Texture enumeration -- matches the data that gets loaded.
enum
{
    TEX_CRCL,
    TEX_HXGN,
    TEX_RING,
    TEX_FLAR,
    NPIECETEXTURES,
    TEX_SUN = NPIECETEXTURES,
    TEX_BACK,
    NTEXTURES
};


// Source files for textures.
TEXTURE_DEF gTextures[ NTEXTURES ] =
            {
                { "crcl.tga" },
                { "hxgn.tga" },
                { "ring.tga" },
                { "flar.tga" },
                { "sun.tga"  },
                { "back.tga" }
            };
TEXTURE_DEF *TM_getNthTexture( int n );
TEXTURE_DEF *TM_getNamedTexture( char *name );
void        TM_loadTextures( void );
void        TM_setTexture( TEXTURE_DEF *tex );

/************************************************************
    Internal function prototypes.
 ************************************************************/

void    drawQuad( int x, int y, int width, int height, TEXTURE_DEF *tex, unsigned int colour );

/************************************************************
    Global variables.
 ************************************************************/

int     SCREENwidth = g_rRect.right;
int     SCREENheight = g_rRect.bottom;

/************************************************************

    Public:
        FLARE_randomize

    Description:
        Generate a random lens flare.

 ************************************************************/

void    FLARE_randomize(FLARE_DEF *flare,
                        int nTextures,
                        int nPieces,
                        float fMaxSize,
                        unsigned int minColour,
                        unsigned int maxColour)
{
    int             i;
    float           fFracDist;
    float           fEnvelopeSize;
    FLARE_ELEMENT_DEF *element;

    srand((unsigned)time(NULL));

    // Initialize flare with requested number of elements, with
    // random placement, colour, and size of elements.
    flare->nPieces = nPieces;
    flare->fScale = fMaxSize;
    flare->fMaxSize = fMaxSize;
    fFracDist = 1.0f/(float)(flare->nPieces - 1);
    
    for (i = 0; i < flare->nPieces; ++i)
    {
        element = &flare->element[i];
        element->fDistance = (fFracDist*i) + FLARE_FRANGE(0,fFracDist);

        // Envelope size is maximum at ends of line, minimum in the middle (i.e. two
        // cones, touching at the tips).
        fEnvelopeSize = (float)fabs(1.0f - 2*element->fDistance);

        element->fSize = FLARE_FRANGE(0.6f, 1.0f) * fEnvelopeSize;
        element->argb = FLARE_RANGE(minColour & 0xff000000, maxColour & 0xff000000) |
                        FLARE_RANGE(minColour & 0x00ff0000, maxColour & 0x00ff0000) |
                        FLARE_RANGE(minColour & 0x0000ff00, maxColour & 0x0000ff00) |
                        FLARE_RANGE(minColour & 0x000000ff, maxColour & 0x000000ff);
        element->texture = TM_getNthTexture( FLARE_RANGE(0, nTextures - 1) );
    }
}

/************************************************************

    Public:
        FLARE_render

    Description:
        Draw lens flare with specified (lx,ly) light position
        in screen coordinates, for given (cx,cy) position of
        center of screen.

 ************************************************************/

void    FLARE_render(FLARE_DEF *flare, int lx, int ly, int cx, int cy)
{
    int     dx, dy;          // Screen coordinates of "destination"
    int     px, py;          // Screen coordinates of flare element
    int     maxflaredist, flaredist, flaremaxsize, flarescale;
    int     width, height, alpha;    // Piece parameters;
    int     i;
    FLARE_ELEMENT_DEF    *element;

    // Compute how far off-center the flare source is.
    maxflaredist = isqrt(cx*cx + cy*cy);
    flaredist = isqrt((lx - cx)*(lx - cx)+
                      (ly - cy)*(ly - cy));
    flaredist = (maxflaredist - flaredist);
    flaremaxsize = (int)(SCREENwidth * flare->fMaxSize);
    flarescale = (int)(SCREENwidth * flare->fScale);

    // Destination is opposite side of centre from source
    dx = cx + (cx - lx);
    dy = cy + (cy - ly);

    // Render each element.
    for (i = 0; i < flare->nPieces; ++i)
    {
        element = &flare->element[i];

        // Position is interpolated along line between start and destination.
        px = (int)((1.0f - element->fDistance)*lx + element->fDistance*dx);
        py = (int)((1.0f - element->fDistance)*ly + element->fDistance*dy);

        // Piece size are 0 to 1; flare size is proportion of
        // screen width; scale by flaredist/maxflaredist.
        width = (int)((flaredist*flarescale*element->fSize)/maxflaredist);

        // Width gets clamped, to allows the off-axis flares
        // to keep a good size without letting the elements get
        // too big when centered.
        if (width > flaremaxsize)
        {
            width = flaremaxsize;
        }

        // Flare elements are square (round) so height is just
        // width scaled by aspect ratio.
        height = HEIGHTFROMWIDTH(width);
        alpha = (flaredist*(element->argb >> 24))/maxflaredist;

        if ( width > 1 )
        {
            unsigned int    argb = (alpha << 24 ) | (element->argb & 0x00ffffff);

            drawQuad( px - width/2, py - height/2, width, height, element->texture, argb );
        }
    }
}

/*
    drawQuad -- used to draw individual elements of the lens flare
    in 2D. This is the main thing that needs to be changed to render
    with a different engine (e.g. D3D instead of OpenGL).
*/

void    drawQuad( int x, int y, int width, int height, TEXTURE_DEF *tex, unsigned int colour )
{
    glBindTexture(GL_TEXTURE_2D, tex->FlareTexture->TexID);

    glBegin(GL_QUADS);

    glColor4ub( (GLbyte)(colour >> 16 & 0xff),
                (GLbyte)(colour >>  8 & 0xff),
                (GLbyte)(colour >>  0 & 0xff),
                (GLbyte)(colour >> 24 & 0xff) );

    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( x, y, 1.0);
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( x + width, y, 1.0);
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( x + width, y + height, 1.0);
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( x, y + height, 1.0);

    glEnd();
}


// -------------- texture manager ---------------

TEXTURE_DEF *TM_getNthTexture( int n )
{
    if ( ( n < 0 ) || ( n >= NTEXTURES ) )
        return NULL;

    return &gTextures[n];
}

TEXTURE_DEF *TM_getNamedTexture( char *name )
{
    int     i;

    for ( i = 0; i < NTEXTURES; ++i )
    {
        if ( strncmp( name, gTextures[i].FlareTexture->TexName, strlen( name ) ) == 0 )
            return &gTextures[i];
    }

    return NULL;
}

void    TM_loadTextures( void )
{
    int             i;

    TEXTURE_DEF *tex = &gTextures[0];

    for ( i = 0; i < NTEXTURES; ++i, ++tex )
    {
        tex->FlareTexture->LoadTGA();            
    }
}

#define FLARE_MINCOLOUR        MAKEID(60, 255, 200, 0)
#define FLARE_MAXCOLOUR        MAKEID(220, 235, 235, 64)
#define FLARE_MINELEMENTSPERFLARE         8
#define FLARE_MAXSIZE                   0.2f

FLARE_DEF       renderFlare;    // Flare to render.

void    loadFlareFile( FLARE_DEF *flare, char *filename )
{
    int     n = 0;
    FILE    *f;
    char    buf[256];

    memset( flare, 0, sizeof( FLARE_DEF ) );

    f = fopen( filename, "r" );
    if ( f )
    {
        fgets( buf, sizeof( buf ), f );
        sscanf( buf, "%f %f", &flare->fScale, &flare->fMaxSize );

        while ( !feof(f) )
        {
            char            name[8] = { '\0', };
            double          dDist = 0.0, dSize = 0.0;
            unsigned int    a = 0, r = 0, g = 0, b = 0;

            fgets( buf, sizeof( buf ), f );
            if ( sscanf( buf, "%s %lf %lf ( %d %d %d %d )",
                    name, &dDist, &dSize, &a, &r, &g, &b ) )
            {
                flare->element[ n ].texture = TM_getNamedTexture( name );
                flare->element[ n ].fDistance = (float)dDist;
                flare->element[ n ].fSize = (float)dSize;
                flare->element[ n ].argb = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );

                ++n;
            }
        }

        flare->nPieces = n;

        fclose( f );
    }
}

void    newFlare( int bFromFile )
{
    if ( bFromFile )
        loadFlareFile( &renderFlare, "flare.txt" );
    else
        FLARE_randomize(&renderFlare, NPIECETEXTURES, FLARE_RANGE(FLARE_MINELEMENTSPERFLARE, FLARE_MAXELEMENTSPERFLARE), FLARE_MAXSIZE, FLARE_MINCOLOUR, FLARE_MAXCOLOUR);
}

void  DrawLensFlare()
{       
	VECTOR suns_position(25.0 + g_Camera.Position().x, 20.0 + g_Camera.Position().y , -35.0 + g_Camera.Position().z);

    SunPosition[1] = -1.0;
    SunPosition[2] = -1.0;

	//g_pLog->Log(0, "sun@ %f", SunPosition[1]);
    glRenderMode(GL_FEEDBACK);
	//g_pLog->Log(0, "sun@@ %f", SunPosition[1]);
    glBegin(GL_POINTS);
        glVertex3fv(&suns_position.x);
		//g_pLog->Log(0, "sun@@@ %f", SunPosition[1]);
    glEnd();
	//g_pLog->Log(0, "sun %f", SunPosition[1]);
    glRenderMode(GL_RENDER);
	//g_pLog->Log(0, "sun0 %f", SunPosition[1]);

    SunPosition[2] = g_rRect.bottom - SunPosition[2];
	//g_pLog->Log(0, "sun2 %f", SunPosition[1]);

//  Set for orthographic projection.
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
       gluOrtho2D(0, SCREENwidth, SCREENheight, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	//g_pLog->Log(0, "sun3 %f", SunPosition[1]);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
	//g_pLog->Log(0, "sun5 %f", SunPosition[1]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//g_pLog->Log(0, "sun9 %f", SunPosition[1]);

    if ((SunPosition[1] != -1.0))
    {
		
         //Calculate alpha value for sun glare
        int position[2];
		
        position[0] = abs((int)SunPosition[1] - 400);
		
        position[1] = abs((int)SunPosition[2] - 300);
		
        float glare_distance =sqrt((double)(position[0] * position[0] + position[1] * position[1]));
		
        float alpha = 1.0 - glare_distance / 500.0;
		

         //Render sun glare - I know it's a 32 bit texture but this is just a demo
        glBindTexture(GL_TEXTURE_2D, texture[6].TexID);

		glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
		//g_pLog->Log(0, "17");

        glColor4f(1.0, 1.0, 1.0, alpha - 0.3);
		//g_pLog->Log(0, "18");

		
        glTexCoord2f( 0.0f, 0.0f );
        glVertex3f(0, 0, 1.0);
        glTexCoord2f( 1.0f, 0.0f );
        glVertex3f(800, 0, 1.0);
        glTexCoord2f( 1.0f, 1.0f );
        glVertex3f(800, 600, 1.0);
        glTexCoord2f( 0.0f, 1.0f );
        glVertex3f(0, 600, 1.0);
		//g_pLog->Log(0, "19");
		
		/*
        glTexCoord2f( 0.0f, 0.0f );
        glVertex3f(SunPosition[1] - 400, SunPosition[2] - 300, 2.0);
        glTexCoord2f( 1.0f, 0.0f );
        glVertex3f(SunPosition[1] + 400, SunPosition[2] - 300, 2.0);
        glTexCoord2f( 1.0f, 1.0f );
        glVertex3f(SunPosition[1] + 400, SunPosition[2] + 300, 2.0);
        glTexCoord2f( 0.0f, 1.0f );
        glVertex3f(SunPosition[1] - 400, SunPosition[2] + 300, 2.0);
		*/

        glEnd();
        //g_pLog->Log(0, "20");
        FLARE_render(&renderFlare, (int)SunPosition[1], (int)SunPosition[2], SCREENwidth/2, SCREENheight/2);
		//g_pLog->Log(0, "21");
    }
	
	//g_pLog->Log(0, "22");
    glMatrixMode(GL_PROJECTION);
	//g_pLog->Log(0, "23");
    glPopMatrix();
	//g_pLog->Log(0, "24");
    glMatrixMode(GL_MODELVIEW);
	//g_pLog->Log(0, "25");
    glPopMatrix();
	//g_pLog->Log(0, "26");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
	//g_pLog->Log(0, "27");
}

void InitializeLensFlare()
{
    SCREENwidth = g_rRect.right;
    SCREENheight = g_rRect.bottom;

    for (int loop = 0; loop < NTEXTURES; loop++)
    {
        gTextures[loop].FlareTexture = new TEXTURE;
        sprintf(gTextures[loop].FlareTexture->TexName, "%s", gTextures[loop].filename);
    }
    TM_loadTextures();
    newFlare( 0 );

    glFeedbackBuffer(3, GL_2D, SunPosition);
}

void ResetLensFlare()
{
    FLARE_randomize(&renderFlare, NPIECETEXTURES, FLARE_RANGE(FLARE_MINELEMENTSPERFLARE, FLARE_MAXELEMENTSPERFLARE), FLARE_MAXSIZE, FLARE_MINCOLOUR, FLARE_MAXCOLOUR);
}

void CleanupLensFlare()
{
    for (int loop = 0; loop < NTEXTURES; loop++)
    {
        delete gTextures[loop].FlareTexture;
    }
}
