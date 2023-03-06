#include "AudioDevice.h"

#include "ErrorCheck.h"

#include <audioclient.h>
#include <mmdeviceapi.h>

#include <Functiondiscoverykeys_devpkey.h>

#include <iostream>
#include <set>

using namespace cwyh;


namespace
{
    std::wstring GetDeviceID( IMMDevice* device )
    {
        AutoFree< WCHAR > id;
        EXIT_ON_NOK( device->GetId( id.Ref() ) );
        return std::wstring( id() );
    }
    std::wstring GetDeviceName( IMMDevice* device )
    {
        AutoRelease< IPropertyStore > deviceProperties;
        EXIT_ON_NOK( device->OpenPropertyStore( STGM_READ, deviceProperties.Ref() ) );

        PROPVARIANT deviceName;
        EXIT_ON_NOK( deviceProperties()->GetValue( PKEY_Device_FriendlyName, &deviceName ) );
        if( deviceName.vt == VT_LPWSTR )
            return deviceName.pwszVal;
        else
            return L"Unknown device";
    }
}


void AudioDevice::Initialize( const DWORD streamFlags, const REFERENCE_TIME& refTimePerSec )
{
    myRefTimePerSec = refTimePerSec;
    EXIT_ON_NOK( myPointer->Activate( IID_IAudioClient, CLSCTX_ALL, NULL, (void**)myAudioClient.Ref() ) );
    EXIT_ON_NOK( myAudioClient()->GetMixFormat( myWaveFormat.Ref() ) );
    PrintDeviceInfo();
    EXIT_ON_NOK( myAudioClient()->Initialize( AUDCLNT_SHAREMODE_SHARED, streamFlags, myRefTimePerSec, 0, WaveFormat(), NULL ) );
    EXIT_ON_NOK( myAudioClient()->GetBufferSize( &myBufferFrameCount ) );
}
IAudioClient* AudioDevice::Client()
{
    return myAudioClient();
}
WAVEFORMATEX* AudioDevice::WaveFormat()
{
    return myWaveFormat();
}
DWORD AudioDevice::FrameSizeInBytes()
{
    return ( myWaveFormat()->wBitsPerSample / 8 ) * myWaveFormat()->nChannels;
}
void AudioDevice::PrintDeviceInfo()
{
    std::wcout
        << GetDeviceName( myPointer )                               << std::endl
        << "\tAverage byte/Sec = " << WaveFormat()->nAvgBytesPerSec << std::endl
        << "\tChannels         = " << WaveFormat()->nChannels       << std::endl
        << "\tSamples/Sec      = " << WaveFormat()->nSamplesPerSec  << std::endl
        << "\tBits/Sample      = " << WaveFormat()->wBitsPerSample  << std::endl
        << std::endl
        ;
}


void cwyh::ChooseOutputDevice( IMMDeviceEnumerator* deviceEnumerator, IMMDevice** deviceChosen, IMMDevice* excludeDevice )
{
    AutoRelease< IMMDeviceCollection > deviceCollection;
    EXIT_ON_NOK( deviceEnumerator->EnumAudioEndpoints( eRender, DEVICE_STATE_ACTIVE, deviceCollection.Ref() ) );

    UINT audioDeviceCount = 0;
    EXIT_ON_NOK( deviceCollection()->GetCount( &audioDeviceCount ) );

    std::cout << "Available devices to clone to:" << std::endl;
    std::set< UINT > validChoices;
    for( UINT deviceNumber = 0; deviceNumber < audioDeviceCount; ++deviceNumber )
    {
        AutoRelease< IMMDevice > device;
        EXIT_ON_NOK( deviceCollection()->Item( deviceNumber, device.Ref() ) );

        if( GetDeviceID( device() ) == GetDeviceID( excludeDevice ) )
            continue;

        std::wcout << "\t" << std::dec << deviceNumber << ": " << GetDeviceName( device() ) << std::endl;
        validChoices.insert( deviceNumber );
    }

    std::cout << "Please input your choice and press enter: ";
    UINT chosenDevice = 0;
    while( !( std::cin >> chosenDevice ) || validChoices.find( chosenDevice ) == validChoices.cend() )
    {
        std::cin.clear();
        std::cin.ignore( 100, '\n' );
        std::cout << "Invalid choice, try again: ";
    }

    EXIT_ON_NOK( deviceCollection()->Item( chosenDevice, deviceChosen ) );
}
