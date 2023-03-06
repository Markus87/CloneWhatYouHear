#include "ResourceHolders.h"

#include "ErrorCheck.h"

using namespace cwyh;


OnDestruct::OnDestruct( const std::function< void() >& onDestruct )
    : myOnDestruct( onDestruct )
{
}
OnDestruct::~OnDestruct()
{
    myOnDestruct();
}


CoInitializer::CoInitializer()
 : OnDestruct( []{ CoUninitialize(); } )
{
    EXIT_ON_NOK( CoInitialize( nullptr ) );
}
