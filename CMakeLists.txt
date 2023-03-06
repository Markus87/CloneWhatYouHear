cmake_minimum_required(VERSION 3.5)

project(CloneWhatYouHear LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded )
add_executable(CloneWhatYouHear
    main.cpp
    AudioDevice.cpp
    AudioDevice.h
    CloneAudio.cpp
    CloneAudio.h
    ErrorCheck.cpp
    ErrorCheck.h
    Forward.h
    Resampler.cpp
    Resampler.h
    ResourceHolders.cpp
    ResourceHolders.h
    uuids.cpp
    )
target_link_libraries(CloneWhatYouHear PRIVATE mfplat mfuuid wmcodecdspuuid)

install(TARGETS CloneWhatYouHear
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
