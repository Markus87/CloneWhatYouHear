#include <audioclient.h>
#include <mmdeviceapi.h>

const CLSID CLSID_MMDeviceEnumerator    = __uuidof( MMDeviceEnumerator  );
const IID   IID_IMMDeviceEnumerator     = __uuidof( IMMDeviceEnumerator );
const IID   IID_IAudioClient            = __uuidof( IAudioClient        );
const IID   IID_IAudioRenderClient      = __uuidof( IAudioRenderClient  );
const IID   IID_IAudioCaptureClient     = __uuidof( IAudioCaptureClient );
