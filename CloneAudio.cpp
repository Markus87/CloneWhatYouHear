#include "CloneAudio.h"

#include "ErrorCheck.h"
#include "Resampler.h"

#include <audioclient.h>
#include <mfapi.h>
#include <mftransform.h>
#include <wmcodecdsp.h>
#include <mmdeviceapi.h>

#include <iostream>
#include <thread>

using namespace cwyh;

namespace
{
    REFERENCE_TIME RefTimesPerMSec()
    {
        return 10 * 1000;
    }
    REFERENCE_TIME RefTimesPerSec()
    {
        return RefTimesPerMSec() * 1000;
    }
}

CloneAudio::CloneAudio()
{
    AutoRelease< IMMDeviceEnumerator > deviceEnumerator;
    EXIT_ON_NOK( CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)deviceEnumerator.Ref() ) );

    EXIT_ON_NOK( deviceEnumerator()->GetDefaultAudioEndpoint( eRender, eConsole, mySourceAudioEndpoint.Ref() ) );
    std::cout << "Source device: ";
    mySourceAudioEndpoint.Initialize( AUDCLNT_STREAMFLAGS_LOOPBACK, RefTimesPerSec() );

    ChooseOutputDevice( deviceEnumerator(), myTargetAudioEndpoint.Ref(), mySourceAudioEndpoint() );
    std::cout << std::endl << "Target device: ";
    myTargetAudioEndpoint.Initialize( 0, RefTimesPerSec() );

    EXIT_ON_NOK( mySourceAudioEndpoint.Client()->GetService( IID_IAudioCaptureClient  , (void**)myAudioCaptureClient.Ref()    ) );
    EXIT_ON_NOK( myTargetAudioEndpoint.Client()->GetService( IID_IAudioRenderClient   , (void**)myAudioRenderClient.Ref()     ) );
}
void CloneAudio::RunInThread()
{
    std::jthread inThread
    ( [=]
    {
        CoInitializer coInit;
        Run();
    });

    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    std::cout << std::endl << "Press 'e' + enter to exit" << std::endl;
    while( std::cin.get() != 'e' );
    myMustStop = true;
}
UINT32 CloneAudio::GetNextPacketSize()
{
    UINT32 packetLength = 0;
    EXIT_ON_NOK( myAudioCaptureClient()->GetNextPacketSize( &packetLength ) );
    return packetLength;
}
void CloneAudio::Run()
{
    std::cout << "Starting to clone audio ..." << std::endl;

    Resampler resampler( mySourceAudioEndpoint.WaveFormat(), myTargetAudioEndpoint.WaveFormat() );

    EXIT_ON_NOK( mySourceAudioEndpoint.Client()->Start() );
    EXIT_ON_NOK( myTargetAudioEndpoint.Client()->Start() );

    while( !myMustStop )
    {
        while( auto packetLength = GetNextPacketSize() )
        {
            UINT32 availableInputFrames     = 0;
            UINT32 availableOutputFrames    = 0;
            DWORD sourceFlags               = 0;

            BYTE* sourceData                = nullptr;
            EXIT_ON_NOK( myAudioCaptureClient()->GetBuffer( &sourceData, &availableInputFrames, &sourceFlags, NULL, NULL ) );

            if( sourceFlags & AUDCLNT_BUFFERFLAGS_SILENT )
                sourceData = NULL;

            if( sourceData )
            {
                AutoRelease< IMFMediaBuffer > outputBuffer;
                resampler.Process( sourceData, availableInputFrames * mySourceAudioEndpoint.FrameSizeInBytes(), outputBuffer.Ref() );

                BYTE* outputData = nullptr;
                EXIT_ON_NOK( outputBuffer()->Lock( &outputData, NULL, NULL ) );

                DWORD outputBufferActualLength = 0;
                EXIT_ON_NOK( outputBuffer()->GetCurrentLength( &outputBufferActualLength ) );

                availableOutputFrames = outputBufferActualLength / myTargetAudioEndpoint.FrameSizeInBytes();
                BYTE* targetData = 0;
                EXIT_ON_NOK( myAudioRenderClient()->GetBuffer( availableOutputFrames, &targetData ) );
                std::memcpy( targetData, outputData, outputBufferActualLength );

                EXIT_ON_NOK( outputBuffer()->Unlock() );
            }

            EXIT_ON_NOK( myAudioCaptureClient()->ReleaseBuffer( availableInputFrames ) );
            EXIT_ON_NOK( myAudioRenderClient()->ReleaseBuffer( availableOutputFrames, sourceFlags & AUDCLNT_BUFFERFLAGS_SILENT ) );
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
    }

    EXIT_ON_NOK( myTargetAudioEndpoint.Client()->Stop() );
    EXIT_ON_NOK( mySourceAudioEndpoint.Client()->Stop() );
}
