# CMakeLists.txt
#
# CMake file for the Paho C++ example applications.
#
#*******************************************************************************
# This is part of the Paho MQTT C++ client library.
#
# Copyright (c) 2016-2024
# 
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# and Eclipse Distribution License v1.0 which accompany this distribution.
# 
# The Eclipse Public License is available at
#   http://www.eclipse.org/legal/epl-v20.html
# and the Eclipse Distribution License is available at
#   http://www.eclipse.org/org/documents/edl-v10.php.
# 
# Contributors:
#   Guilherme Maciel Ferreira - initial version
#   Frank Pagliughi - Updates for new samples
#*******************************************************************************/

## --- Library dependencies ---
# include(toolchain.cmake)
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
link_directories("/hdd1/work/paho.mqtt.cpp/build/src")
include_directories("/hdd1/work/paho.mqtt.cpp/include")
# The example applications
set(EXECUTABLES
#    publisher
   subscriber
performance
)

# These will only be built if SSL selected
if(PAHO_WITH_SSL)
    set(SSL_EXECUTABLES ssl_publish)
endif()

## Build the example apps
foreach(EXECUTABLE ${EXECUTABLES} ${SSL_EXECUTABLES})
    add_executable(${EXECUTABLE} ${EXECUTABLE}.cpp)
    target_link_libraries(${EXECUTABLE} paho-mqttpp3 paho-mqtt3as)

    set_target_properties(${EXECUTABLE} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    if(PAHO_BUILD_SHARED)
        target_compile_definitions(${EXECUTABLE} PRIVATE PAHO_MQTTPP_IMPORTS)
    endif()
endforeach()

## Extra configuration for the SSL/TLS examples, if selected
foreach(EXECUTABLE ${SSL_EXECUTABLES})
    target_compile_definitions(${EXECUTABLE} PUBLIC OPENSSL)
endforeach()

## install binaries
include(GNUInstallDirs)

install(TARGETS ${EXECUTABLES} ${SSL_EXECUTABLES}
    EXPORT PahoMqttCppSamples
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


