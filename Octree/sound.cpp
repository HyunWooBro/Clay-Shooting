#include "sound.h"
#include "tll.h"
#include "Camera.h"
//#include "resource.h"


extern int device;
extern BOOL lowqual;
extern int SphereSector;
extern VECTOR SpherePosition;
extern int numSamples;
extern int numChannels;
extern LinkedList<SOUND_SAMPLE> SampleList;
extern LinkedList<SOUND_CHANNEL> ChannelList;
extern HWND  g_hWnd;
extern CCamera	g_Camera;
//extern CAMERA LastCam;
//extern int currentCamera;

int SOUND_SAMPLE::Compare(const SOUND_SAMPLE& Sample)
{
    if (linkPosition < Sample.linkPosition)
        return smaller;
      if (linkPosition > Sample.linkPosition)
        return bigger;
      else
        return same;
}

int SOUND_CHANNEL::Compare(const SOUND_CHANNEL& Channel)
{
    if (linkPosition < Channel.linkPosition)
        return smaller;
      if (linkPosition > Channel.linkPosition)
        return bigger;
      else
        return same;
}

void InitializeBASS()
{
    /* Check that BASS 1.4 was loaded */
    if (BASS_GetVersion() != MAKELONG(1, 4))
        MessageBox(0, "BASS version 1.4 was not loaded", "Incorrect BASS.DLL", 0);

    /* Initialize the output device (syncs not used) */
    if (!BASS_Init(device, lowqual?22050:44100, BASS_DEVICE_3D, g_hWnd))
        MessageBox(0, "Could not initialize output device", "Error", 0);

    /* Use meters as distance unit, real world rolloff, real doppler effect */
    BASS_Set3DFactors(1.0, 1.0, 1.0);

    /* Turn EAX off (volume=0.0), if error then EAX is not supported */
    BASS_SetEAXParameters(-1, 0.0, -1.0, -1.0);

    BASS_Start();    /* Start digital output */
}


void LoadSample(SOUND_SAMPLE* Sample)
{
    /* Load a sample from a file */
    Sample->hSample = BASS_SampleLoad(FALSE, Sample->name, 0, 0, Sample->max, Sample->flags);
    if (Sample->hSample)
    {
        BASS_SAMPLE sampleInfo;
        BASS_SampleGetInfo(Sample->hSample, &sampleInfo);

        sampleInfo.volume = Sample->volume;
        sampleInfo.mindist = Sample->mindist;
        sampleInfo.maxdist = Sample->maxdist;
        BASS_SampleSetInfo(Sample->hSample, &sampleInfo);

        Sample->linkPosition = ++numSamples;
        SampleList.Insert(Sample);
    }
    else
        MessageBox(NULL,"Could not load sound sample","Error",MB_OK|MB_ICONERROR);
}

void CreateChannel(int SampleNumber)
{
    SOUND_SAMPLE* Sample = SampleList.Get(SampleNumber);
    SOUND_CHANNEL* Channel = new SOUND_CHANNEL;
    Channel->hSample = Sample->hSample;
    Channel->linkPosition = ++numChannels;
    ChannelList.Insert(Channel);
}

void UpdateListener()
{
    BASS_3DVECTOR Player;
    BASS_3DVECTOR Bass_Front;
    BASS_3DVECTOR Bass_Top;
    BASS_3DVECTOR Bass_Velocity;

    Player.x = g_Camera.Position().x;       // Get the player's position
    Player.y = g_Camera.Position().y;
    Player.z = g_Camera.Position().z;
	CVector3 vVector = g_Camera.View()-g_Camera.Position();
	vVector = Normalize(vVector);
    VECTOR Front = VECTOR(vVector.x, vVector.y, vVector.z);   // Get the player's front vector
    VECTOR Top = VECTOR(g_Camera.UpVector().x, g_Camera.UpVector().y, g_Camera.UpVector().z);     // Get the player's up vector
    Bass_Front.x = -Front.x;                           // Reverse the front vector for OpenGL
    Bass_Front.y = -Front.y;
    Bass_Front.z = -Front.z;
    Bass_Top.x = Top.x;
    Bass_Top.y = Top.y;
    Bass_Top.z = Top.z;
    Bass_Velocity.x = g_Camera.Position().x;//camera[currentCamera].Position.x - LastCam.Position.x;
    Bass_Velocity.y = g_Camera.Position().y;//camera[currentCamera].Position.y - LastCam.Position.y;
    Bass_Velocity.z = g_Camera.Position().z;//camera[currentCamera].Position.z - LastCam.Position.z;

    BASS_Set3DPosition(&Player, &Bass_Velocity, &Bass_Front, &Bass_Top);  // Make changes to the player position/orientation

    /* Apply the 3D changes */
    BASS_Apply3D();
}

void UpdateChannel(int ChannelNumber, VECTOR* Position, VECTOR* Orientation, VECTOR* Velocity)
{
    SOUND_CHANNEL* Channel = ChannelList.Get(ChannelNumber);

    if (Position)
    {
        Channel->position.x = Position->x;
        Channel->position.y = Position->y;
        Channel->position.z = Position->z;
    }
    if (Orientation)
    {
        Channel->orientation.x = Orientation->x;
        Channel->orientation.y = Orientation->y;
        Channel->orientation.z = Orientation->z;
    }
    if (Velocity)
    {
        Channel->velocity.x = Velocity->x;
        Channel->velocity.y = Velocity->y;
        Channel->velocity.z = Velocity->z;
    }

    /* If the channel's playing then update it's position */
    if (BASS_ChannelIsActive(Channel->hChannel))
        BASS_ChannelSet3DPosition(Channel->hChannel, &Channel->position, NULL, &Channel->velocity);

    /* Apply the 3D changes */
    BASS_Apply3D();
}

void PlayChannel(int ChannelNumber, bool is3dsound)
{
    SOUND_CHANNEL* Channel = ChannelList.Get(ChannelNumber);
	if(is3dsound == false)
		Channel->hChannel = BASS_SamplePlay(Channel->hSample);
	else
		Channel->hChannel =	BASS_SamplePlay3D(Channel->hSample, &Channel->position, NULL, &Channel->velocity);

    char temp[256];
    int result = BASS_ErrorGetCode();
    if (result)
    {
        sprintf(temp, "BASS error code No. %d", result);
        MessageBox(NULL, temp, "Error", MB_OK|MB_ICONERROR);
    }
}

VOID CALLBACK SoundTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    SOUND_CHANNEL* Channel = ChannelList.Get((int)idEvent);
    PlayChannel((int)idEvent, true);
    if ((Channel->looped && !Channel->intermittent) || !Channel->looped)
        KillTimer(hWnd, idEvent);
    if (Channel->looped && Channel->intermittent && Channel->random)
    {
        int randomtime = (rand()%(Channel->upperrand - Channel->lowerrand)) + Channel->lowerrand;
        SetTimer(hWnd, idEvent, randomtime,    (TIMERPROC)SoundTimerProc);
    }
}

void CreateSounds()
{
    SOUND_SAMPLE* Sample;
    SOUND_CHANNEL* Channel;
    VECTOR Position;

// engine sound
//    Sample = new SOUND_SAMPLE;
//    sprintf(Sample->name, "%s", "engine.wav");
//    Sample->max = 10;
//    Sample->volume = 100;
//    Sample->mindist = 10.0;
//    Sample->maxdist = 1000.0;
//    Sample->flags = BASS_SAMPLE_OVER_DIST | BASS_SAMPLE_LOOP | BASS_SAMPLE_3D | BASS_SAMPLE_VAM;
//    LoadSample(Sample);           // Load the sample and put it in the sample list
//
//    CreateChannel(sample_engine); // Create a new channel with the sample and put it in the channel list
//    Channel = ChannelList.Get(channel_engine);
//    Channel->looped = true;
//    Channel->intermittent = false;
//    Channel->random = false;
//    Channel->interval = 2000;
//    Channel->idEvent = (unsigned int)channel_engine;
//    SetTimer(g_hWnd, Channel->idEvent, Channel->interval, (TIMERPROC)SoundTimerProc);
//
//// water drips
//    Sample = new SOUND_SAMPLE;
//    sprintf(Sample->name, "%s", "waterdrop.wav");
//    Sample->max = 10;
//    Sample->volume = 20;
//    Sample->mindist = 12.0;
//    Sample->maxdist = 100.0;
//    Sample->flags = BASS_SAMPLE_OVER_DIST | BASS_SAMPLE_MUTEMAX | BASS_SAMPLE_3D | BASS_SAMPLE_VAM;
//    LoadSample(Sample);
//
//    // 1st drip
//    CreateChannel(sample_drip);
//    Channel = ChannelList.Get(channel_drip1);
//    Channel->looped = true;
//    Channel->intermittent = true;
//    Channel->random = false;
//    Channel->interval = 1000;
//    Channel->idEvent = (unsigned int)channel_drip1;
//    SetTimer(g_hWnd, Channel->idEvent, Channel->interval, (TIMERPROC)SoundTimerProc);
//    Position.x = 150.0;
//    Position.y = -10.0;
//    Position.z = -30.0;
//    UpdateChannel(channel_drip1, &Position, NULL, NULL);
//
//    // 2nd drip
//    CreateChannel(sample_drip);
//    Channel = ChannelList.Get(channel_drip2);
//    Channel->looped = true;
//    Channel->intermittent = true;
//    Channel->random = true;
//    Channel->lowerrand = 800;
//    Channel->upperrand = 8000;
//    Channel->interval = 1000;
//    Channel->idEvent = (unsigned int)channel_drip2;
//    SetTimer(g_hWnd, Channel->idEvent, Channel->interval, (TIMERPROC)SoundTimerProc);
//    Position.x = 150.0;
//    Position.y = -10.0;
//    Position.z = -30.0;
//    UpdateChannel(channel_drip2, &Position, NULL, NULL);
//
//// bird sound
//    Sample = new SOUND_SAMPLE;
//    sprintf(Sample->name, "%s", "bird.wav");
//    Sample->max = 10;
//    Sample->volume = 50;
//    Sample->mindist = 10.0;
//    Sample->maxdist = 100.0;
//    Sample->flags = BASS_SAMPLE_OVER_DIST | BASS_SAMPLE_MUTEMAX | BASS_SAMPLE_3D | BASS_SAMPLE_VAM;
//    LoadSample(Sample);
//
//    CreateChannel(sample_bird);
//    Channel = ChannelList.Get(channel_bird);
//    Channel->looped = true;
//    Channel->intermittent = true;
//    Channel->random = true;
//    Channel->lowerrand = 2000;
//    Channel->upperrand = 25000;
//    Channel->interval = 5000;
//    Channel->idEvent = (unsigned int)channel_bird;
//    SetTimer(g_hWnd, Channel->idEvent, Channel->interval, (TIMERPROC)SoundTimerProc);
//    Position.x = 110.0;
//    Position.y = 0.0;
//    Position.z = 120.0;
//    UpdateChannel(channel_bird, &Position, NULL, NULL);

// car horn sound      (played on a keystroke so we don't set a timer)
    Sample = new SOUND_SAMPLE;
    sprintf(Sample->name, "%s", "gunshot.wav");
    Sample->max = 10;
    Sample->volume = 100;
    Sample->mindist = 10.0;
    Sample->maxdist = 1000.0;
    Sample->flags = BASS_SAMPLE_OVER_DIST | BASS_SAMPLE_VAM;
    LoadSample(Sample);

    CreateChannel(sample_gunshot);

	Sample = new SOUND_SAMPLE;
    sprintf(Sample->name, "%s", "loading.wav");
    Sample->max = 10;
    Sample->volume = 100;
    Sample->mindist = 10.0;
    Sample->maxdist = 1000.0;
    Sample->flags = BASS_SAMPLE_OVER_DIST  | BASS_SAMPLE_VAM;
    LoadSample(Sample);

    CreateChannel(sample_loading);

	Sample = new SOUND_SAMPLE;
    sprintf(Sample->name, "%s", "click.wav");
    Sample->max = 10;
    Sample->volume = 100;
    Sample->mindist = 10.0;
    Sample->maxdist = 1000.0;
    Sample->flags = BASS_SAMPLE_OVER_DIST  | BASS_SAMPLE_VAM;
    LoadSample(Sample);

    CreateChannel(sample_click);

	Sample = new SOUND_SAMPLE;
    sprintf(Sample->name, "%s", "hover.wav");
    Sample->max = 10;
    Sample->volume = 100;
    Sample->mindist = 10.0;
    Sample->maxdist = 1000.0;
    Sample->flags = BASS_SAMPLE_OVER_DIST  | BASS_SAMPLE_VAM;
    LoadSample(Sample);

    CreateChannel(sample_hover);

	Sample = new SOUND_SAMPLE;
    sprintf(Sample->name, "%s", "explode.wav");
    Sample->max = 10;
    Sample->volume = 100;
    Sample->mindist = 10.0;
    Sample->maxdist = 1000.0;
    Sample->flags = BASS_SAMPLE_OVER_DIST  | BASS_SAMPLE_VAM;
    LoadSample(Sample);

    CreateChannel(sample_explode);

	Sample = new SOUND_SAMPLE;
    sprintf(Sample->name, "%s", "bird.wav");
    Sample->max = 10;
    Sample->volume = 100;
    Sample->mindist = 10.0;
    Sample->maxdist = 1000.0;
    Sample->flags = BASS_SAMPLE_OVER_DIST  | BASS_SAMPLE_VAM;
    LoadSample(Sample);

    CreateChannel(sample_bird);

	Sample = new SOUND_SAMPLE;
    sprintf(Sample->name, "%s", "applause.wav");
    Sample->max = 10;
    Sample->volume = 100;
    Sample->mindist = 10.0;
    Sample->maxdist = 1000.0;
    Sample->flags = BASS_SAMPLE_OVER_DIST  | BASS_SAMPLE_VAM;
    LoadSample(Sample);

    CreateChannel(sample_applause);
}

// Portion Copyright (C) Ian Luck
// Simple device selector dialog stuff begins here
//BOOL CALLBACK devicedialogproc(HWND h,UINT m,WPARAM w,LPARAM l)
//{
//    switch (m) {
//        case WM_COMMAND:
//            switch (LOWORD(w)) {
//                case ID_DEVLIST:
//                    if (HIWORD(w)==LBN_SELCHANGE)
//                        device=SendMessage((HWND)l,LB_GETCURSEL,0,0);
//                    else if (HIWORD(w)==LBN_DBLCLK)
//                        EndDialog(h,0);
//                    break;
//                case ID_LOWQUAL:
//                    lowqual=SendDlgItemMessage(h,ID_LOWQUAL,BM_GETCHECK,0,0);
//                    break;
//                case IDOK:
//                    EndDialog(h,0);
//                    return 1;
//            }
//            break;
//
//        case WM_INITDIALOG:
//            {
//                char text[100],*d;
//                int c;
//                for (c=0;BASS_GetDeviceDescription(c,&d);c++) {
//                    strcpy(text,d);
//                    /* Check if the device supports 3D - don't bother with sync/update threads */
//                    if (!BASS_Init(c,44100,BASS_DEVICE_3D|BASS_DEVICE_NOSYNC|BASS_DEVICE_NOTHREAD,h))
//                        continue; // no 3D support
//                    if (BASS_GetEAXParameters(NULL,NULL,NULL,NULL))
//                        strcat(text," [EAX]"); // it has EAX
//                    BASS_Free();
//                    SendDlgItemMessage(h,ID_DEVLIST,LB_ADDSTRING,0,(LONG)text);
//                }
//                SendDlgItemMessage(h,ID_DEVLIST,LB_SETCURSEL,0,0);
//            }
//            device=lowqual=0;
//            return 1;
//    }
//    return 0;
//}
// Device selector stuff ends here
