#include "CloneAudio.h"
#include "ResourceHolders.h"

#include <iostream>

using namespace cwyh;

int main()
{
    std::cout
        << "CloneWhatYouHear! - v1.0 (Built: " << __DATE__ << " - " << __TIME__ << ")" << std::endl << std::endl;
        ;
    CoInitializer coInit;
    CloneAudio().RunInThread();
    return 0;
}
