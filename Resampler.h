#pragma once

#include "Forward.h"

#include "ResourceHolders.h"

namespace cwyh
{
    class Resampler
    {
    public:
        Resampler( WAVEFORMATEX* inputWaveFormat, WAVEFORMATEX* outputWaveFormat );
        void Process( BYTE* inputData, DWORD inputByteCount, IMFMediaBuffer** contiguousOutputBuffer );

    private:
        AutoRelease< IUnknown       > myTransformUnknown;
        AutoRelease< IMFTransform   > myTransform;
        AutoRelease< IMFMediaType   > myInputType;
        AutoRelease< IMFMediaType   > myOutputType;
        AutoRelease< IMFMediaBuffer > myInputBuffer;
        AutoRelease< IMFMediaBuffer > myOutputBuffer;
    };
}
