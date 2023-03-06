#pragma once

#include "Forward.h"

#include "AudioDevice.h"
#include "ResourceHolders.h"

namespace cwyh
{
    class CloneAudio
    {
    public:
        CloneAudio();
        void RunInThread();

    private:
        UINT32 GetNextPacketSize();
        void Run();

    private:
        AudioDevice mySourceAudioEndpoint;
        AudioDevice myTargetAudioEndpoint;
        AutoRelease< IAudioCaptureClient    > myAudioCaptureClient;
        AutoRelease< IAudioRenderClient     > myAudioRenderClient;
        std::atomic< bool > myMustStop;
    };
}
