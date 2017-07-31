// Sound Classes    by Alan Baylis 2002

#ifndef SoundH
#define SoundH

#include <windows.h>
#include "main.h"
#include "tll.h"
#include "vector.h"
#include "bass.h"

// The order of these should match the order in which the samples are loaded
enum {/*sample_engine = 1, sample_drip, sample_bird,*/ 
	sample_gunshot=1, sample_loading, sample_click, sample_hover, sample_explode, sample_bird, sample_applause};
// The order of these should match the order in which the channels are created
enum {/*channel_engine = 1, channel_drip1, channel_drip2, channel_bird,*/ 
	channel_gunshot=1, channel_loading, channel_click, channel_hover, channel_explode, channel_bird, channel_applause};

class SOUND_SAMPLE
{
    public:
        SOUND_SAMPLE(){};
        ~SOUND_SAMPLE(){};

        int Compare(const SOUND_SAMPLE& Sample);
        int GetMyPosition() const {return linkPosition;}
        void SetMyPosition(int newPosition) {linkPosition = newPosition;}
        int linkPosition;

        HSAMPLE hSample;             // the sample's handle
        char name[32];               // filename
        int max;                     // number of simultaneous playbacks
        DWORD flags;                 // option flags
        DWORD volume;
        float mindist;
        float maxdist;
};

class SOUND_CHANNEL
{
    public:
        SOUND_CHANNEL(){};
        ~SOUND_CHANNEL(){};

        int Compare(const SOUND_CHANNEL& Channel);
        int GetMyPosition() const {return linkPosition;}
        void SetMyPosition(int newPosition) {linkPosition = newPosition;}
        int linkPosition;

        HCHANNEL hChannel;             // the channel's handle
        HSAMPLE hSample;             // handle of the sample associated with this channel
        BASS_3DVECTOR position;      // position
        BASS_3DVECTOR orientation;   // orientation
        BASS_3DVECTOR velocity;         // velocity
        int direction;                 // direction of the channel
        bool looped;                 // flag for looped sample
        bool intermittent;           // flag for intermittent sound
        bool random;                 // random intermittent sound
        unsigned int lowerrand;      // minimum repeat time
        unsigned int upperrand;      // maximum repeat time
        unsigned int interval;       // time between plays
        unsigned int idEvent;        // unique timer id (set this to the channel number)
};

void InitializeBASS();
void LoadSample(SOUND_SAMPLE* Sample);
void CreateChannel(int SampleNumber);
void UpdateListener();
void UpdateChannel(int ChannelNumber, VECTOR* Position, VECTOR* Orientation, VECTOR* Velocity);
void PlayChannel(int ChannelNumber, bool is3dsound  = true);
VOID CALLBACK SoundTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void CreateSounds();
//BOOL CALLBACK devicedialogproc(HWND h,UINT m,WPARAM w,LPARAM l);

#endif
