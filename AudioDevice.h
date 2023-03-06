#pragma once

#include "Forward.h"

#include "ResourceHolders.h"

#include <ks.h>
#include <ksmedia.h>

namespace cwyh
{
    class AudioDevice
        : public AutoRelease< IMMDevice >
    {
    public:
        void Initialize( const DWORD streamFlags, const REFERENCE_TIME& refTimePerSec );
        IAudioClient* Client();
        WAVEFORMATEX* WaveFormat();
        DWORD FrameSizeInBytes();

    private:
        AutoRelease < IAudioClient > myAudioClient;
        AutoFree    < WAVEFORMATEX > myWaveFormat;
        UINT32          myBufferFrameCount  = 0;
        REFERENCE_TIME  myRefTimePerSec     = 0;

        void PrintDeviceInfo();
    };


    void ChooseOutputDevice( IMMDeviceEnumerator* deviceEnumerator, IMMDevice** deviceChosen, IMMDevice* excludeDevice );
}
