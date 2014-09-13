cmake_minimum_required(VERSION 2.8)

project(ccudaq)
set(ccudaq_VERSION_MAJOR 0)
set(ccudaq_VERSION_MINOR 1)

#pthread
set(PTHREAD_DIR ${PROJECT_SOURCE_DIR}/pthread_win32)
set(PTHREAD_INCLUDE_DIR ${PTHREAD_DIR}/include)
set(PTHREAD_LIBRARY_DIR ${PTHREAD_DIR}/lib)
set(PTHREAD_LIBRARIES pthreadVC2.lib)
include_directories(${PTHREAD_INCLUDE_DIR})
link_directories(${PTHREAD_LIBRARY_DIR})

#pdcurses
set(PDC_DIR ${PROJECT_SOURCE_DIR}/pdcurses)
set(PDC_INCLUDE_DIR ${PDC_DIR}/include)
set(PDC_LIBRARY_DIR ${PDC_DIR}/lib)
set(PDC_LIBRARIES pdcurses.lib)
include_directories(${PDC_INCLUDE_DIR})
link_directories(${PDC_LIBRARY_DIR})
add_definitions(-DPDC_WIDE -DPDC_FORCE_UTF8 -DPDC_DLL_BUILD)

#xxusb
set(XXUSB_DIR ${PROJECT_SOURCE_DIR}/libxxusb)
set(XXUSB_INCLUDE_DIR ${XXUSB_DIR}/include)
set(XXUSB_LIBRARY_DIR ${XXUSB_DIR}/lib)
set(XXUSB_LIBRARIES libxxusb.lib)
include_directories(${XXUSB_INCLUDE_DIR})
link_directories(${XXUSB_LIBRARY_DIR})

#windows api
set(WIN_LIBRARIES user32.lib advapi32.lib kernel32.lib)

#ni-visa
#set(NIVISA_DIR ./libxxusb)
#set(NIVISA_INCLUDE_DIR ${XXUSB_DIR}/include)
#set(NIVISA_LIBRARY_DIR ${XXUSB_DIR}/lib)
#set(NIVISA_LIBRARIES libxxusb.lib)
#include_directories(${NIVISA_INCLUDE_DIR})
#link_directories(${NIVISA_LIBRARY_DIR})


#####################################################
set(SOURCES main.cpp adc.cpp ccu.cpp display.cpp log.cpp manager.cpp modul.cpp)
add_executable(ccudaq ${SOURCES})
target_link_libraries(ccudaq ${PTHREAD_LIBRARIES} ${PDC_LIBRARIES} ${XXUSB_LIBRARIES} ${WIN_LIBRARIES})

file(COPY ${PTHREAD_LIBRARY_DIR}/${PTHREAD_LIBRARIES} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PDC_LIBRARY_DIR}/${PDC_LIBRARIES} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${XXUSB_LIBRARY_DIR}/${XXUSB_LIBRARIES} DESTINATION ${PROJECT_BINARY_DIR})
#####################################################
