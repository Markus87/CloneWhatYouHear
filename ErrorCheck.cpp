#include "ErrorCheck.h"

#include <iostream>

using namespace cwyh;

bool cwyh::ExitOnNotOK( const HRESULT result, const std::string& call )
{
    if( result == S_OK )
        return false;
    std::cerr << call << " failed: 0x" << std::hex << result << std::endl;
    std::exit( 0 );
    return true;
}
