#pragma once

#include <winerror.h>

#include <string>

namespace cwyh
{
    bool ExitOnNotOK( const HRESULT result, const std::string& call );
}

#define EXIT_ON_NOK( call ) cwyh::ExitOnNotOK( call, #call )
