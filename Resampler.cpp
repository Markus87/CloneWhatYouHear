#include "Resampler.h"

#include "ErrorCheck.h"

#include <audioclient.h>
#include <mfapi.h>
#include <mftransform.h>
#include <wmcodecdsp.h>
#include <mmdeviceapi.h>

#include <iostream>
#include <thread>

using namespace cwyh;

Resampler::Resampler( WAVEFORMATEX* inputWaveFormat, WAVEFORMATEX* outputWaveFormat )
{
    EXIT_ON_NOK( CoCreateInstance( CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)myTransformUnknown.Ref() ) );

    AutoRelease< IWMResamplerProps > resamplerProps;
    EXIT_ON_NOK( myTransformUnknown()->QueryInterface( IID_PPV_ARGS( resamplerProps.Ref() ) ) );
    resamplerProps()->SetHalfFilterLength( 50 );

    EXIT_ON_NOK( myTransformUnknown()->QueryInterface( IID_PPV_ARGS( myTransform.Ref() ) ) );

    EXIT_ON_NOK( MFCreateMediaType( myInputType.Ref() ) );
    EXIT_ON_NOK( MFInitMediaTypeFromWaveFormatEx( myInputType(), inputWaveFormat, sizeof( WAVEFORMATEX ) + inputWaveFormat->cbSize ) );
    EXIT_ON_NOK( myTransform()->SetInputType( 0, myInputType(), 0 ) );

    EXIT_ON_NOK( MFCreateMediaType( myOutputType.Ref() ) );
    EXIT_ON_NOK( MFInitMediaTypeFromWaveFormatEx( myOutputType(), outputWaveFormat, sizeof( WAVEFORMATEX ) + outputWaveFormat->cbSize ) );
    EXIT_ON_NOK( myTransform()->SetOutputType( 0, myOutputType(), 0 ) );

    const auto _1MB = 1024 * 1024;
    EXIT_ON_NOK( MFCreateMemoryBuffer( _1MB, myInputBuffer.Ref() ) );
    EXIT_ON_NOK( MFCreateMemoryBuffer( _1MB, myOutputBuffer.Ref() ) );
}
void Resampler::Process( BYTE* inputData, DWORD inputByteCount, IMFMediaBuffer** contiguousOutputBuffer )
{
    {
        AutoRelease< IMFSample > inputSample;
        EXIT_ON_NOK( MFCreateSample( inputSample.Ref() ) );

        BYTE* inputBufferPtr;
        EXIT_ON_NOK( myInputBuffer()->Lock( &inputBufferPtr, NULL, NULL ) );
        std::memcpy( inputBufferPtr, inputData, inputByteCount );
        EXIT_ON_NOK( myInputBuffer()->Unlock() );

        EXIT_ON_NOK( myInputBuffer()->SetCurrentLength( inputByteCount ) );
        EXIT_ON_NOK( inputSample()->AddBuffer( myInputBuffer() ) );

        EXIT_ON_NOK( myTransform()->ProcessInput( 0, inputSample(), 0 ) );
    }
    {
        AutoRelease< IMFSample > outputSample;
        EXIT_ON_NOK( MFCreateSample( outputSample.Ref() ) );

        MFT_OUTPUT_STREAM_INFO streamInfo;
        MFT_OUTPUT_DATA_BUFFER outputDataBuffer;
        DWORD dwStatus;
        std::memset( &streamInfo        , 0, sizeof streamInfo          );
        std::memset( &outputDataBuffer  , 0, sizeof outputDataBuffer    );

        EXIT_ON_NOK( outputSample()->AddBuffer( myOutputBuffer() ) );
        outputDataBuffer.pSample    = outputSample();
        outputDataBuffer.dwStreamID = 0;
        outputDataBuffer.dwStatus   = 0;
        outputDataBuffer.pEvents    = NULL;

        EXIT_ON_NOK( myTransform()->ProcessOutput( 0, 1, &outputDataBuffer, &dwStatus ) );
        EXIT_ON_NOK( outputSample()->ConvertToContiguousBuffer( contiguousOutputBuffer ) );
    }
}
