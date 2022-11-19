cmake_minimum_required(VERSION 3.23)
project(example)

set(CMAKE_CXX_STANDARD 14)

include_directories(OpenVrDriverExample/OpenVrDriverExample/include)

add_library(RemoteVVR SHARED
        OpenVrDriverExample/OpenVrDriverExample/include/ControllerDriver.h
        OpenVrDriverExample/OpenVrDriverExample/include/DeviceProvider.h
        OpenVrDriverExample/OpenVrDriverExample/include/openvr.h
        OpenVrDriverExample/OpenVrDriverExample/include/openvr_capi.h
        OpenVrDriverExample/OpenVrDriverExample/include/openvr_driver.h
        OpenVrDriverExample/OpenVrDriverExample/src/ControllerDriver.cpp
        OpenVrDriverExample/OpenVrDriverExample/src/DeviceFactory.cpp
        OpenVrDriverExample/OpenVrDriverExample/src/DeviceProvider.cpp
        OpenVrDriverExample/OpenVrDriverExample/src/HMDDriver.cpp
        OpenVrDriverExample/OpenVrDriverExample/src/HMDDriver.h
        )
# Set the output directory for the library
set_target_properties(RemoteVVR PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )