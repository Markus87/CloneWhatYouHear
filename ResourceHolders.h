#pragma once

#include <combaseapi.h>

#include <functional>

namespace cwyh
{
    struct OnDestruct
    {
        OnDestruct( const std::function< void() >& onDestruct );
        ~OnDestruct();
    private:
        std::function< void() > myOnDestruct;
    };


    struct CoInitializer : public OnDestruct
    {
        CoInitializer();
    };


template< typename P >
    struct PointerBase
    {
        P* operator()()
        {
            return myPointer;
        }
        P** Ref()
        {
            return &myPointer;
        }
    protected:
        P* myPointer = nullptr;
    };


template< typename P >
    struct AutoRelease : OnDestruct, PointerBase< P >
    {
        AutoRelease()
            : OnDestruct
            ( [=]
            {
                if( !this->myPointer )
                    return;
                this->myPointer->Release();
                this->myPointer = nullptr;
            })
        {
        }
    };


template< typename P >
    struct AutoFree : OnDestruct, PointerBase< P >
    {
        AutoFree()
            : OnDestruct( [=]{ CoTaskMemFree( this->myPointer ); } )
        {
        }
    };
}
