cmake_minimum_required(VERSION 2.8)

project(ccudaq)
set(ccudaq_VERSION_MAJOR 1)
set(ccudaq_VERSION_MINOR 0)

# ROOT
if(NOT DEFINED ENV{ROOTSYS})
    message(FATAL_ERROR "Please set environment variable ROOTSYS in your system")
else()
    set(ROOTSYS $ENV{ROOTSYS})
    message(STATUS "ROOTSYS = ${ROOTSYS}")
endif()
if(WIN32)
    find_program(ROOTCINT_EXECUTABLE rootcint PATHS ${ROOTSYS}/bin)
elseif(UNIX)
    find_program(ROOTCINT_EXECUTABLE rootcint PATHS ${ROOTSYS}/bin)
    find_program(ROOT_CONFIG_EXECUTABLE root-config PATHS ${ROOTSYS}/bin)
else(WIN32)
    message(FATAL_ERROR "Platform not supportted: only windows and unix are supported")
endif(WIN32)

#root include path
if(WIN32)
    set(ROOT_INCLUDE_DIR ${ROOTSYS}/include)
else(WIN32)
    execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
        OUTPUT_VARIABLE ROOT_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(WIN32)
include_directories(${ROOT_INCLUDE_DIR})
message(STATUS "ROOT_INCLUDE_DIR = ${ROOT_INCLUDE_DIR}")

#root library
if(WIN32)
    set(ROOT_LIBRARY_DIR ${ROOTSYS}/lib)
    set(ROOT_LIBRARIES ${ROOTSYS}/lib/libCore.lib
                 ${ROOTSYS}/lib/libCint.lib ${ROOTSYS}/lib/libHist.lib
                 ${ROOTSYS}/lib/libGraf.lib ${ROOTSYS}/lib/libGraf3d.lib
                 ${ROOTSYS}/lib/libGpad.lib ${ROOTSYS}/lib/libTree.lib
                 ${ROOTSYS}/lib/libRint.lib ${ROOTSYS}/lib/libPostscript.lib
                 ${ROOTSYS}/lib/libMatrix.lib ${ROOTSYS}/lib/libPhysics.lib
                 ${ROOTSYS}/lib/libNet.lib ${ROOTSYS}/lib/libRIO.lib
                 ${ROOTSYS}/lib/libMathCore.lib
                 ${ROOTSYS}/lib/libSpectrum.lib
                 ${ROOTSYS}/lib/libGui.lib ${ROOTSYS}/lib/libGraf.lib
                 ${ROOTSYS}/lib/libGpad.lib)
else(WIN32)
    execute_process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --libdir
        OUTPUT_VARIABLE ROOT_LIBRARY_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --glibs
        OUTPUT_VARIABLE ROOT_LIBRARIES
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(ROOT_LIBRARIES ${ROOT_LIBRARIES} -lSpectrum)
endif(WIN32)
link_directories(${ROOT_LIBRARY_DIR})
message(STATUS "ROOT_LIBRARY_DIR = ${ROOT_LIBRARY_DIR}")
message(STATUS "ROOT_LIBRARIES = ${ROOT_LIBRARIES}")

#pthread
set(PTHREAD_DIR ${ccudaq_SOURCE_DIR}/pthread_win32)
set(PTHREAD_INCLUDE_DIR ${PTHREAD_DIR}/include)
set(PTHREAD_LIBRARY_DIR ${PTHREAD_DIR}/lib)
set(PTHREAD_LIBRARIES pthreadVC2.lib)
set(PTHREAD_DLLS ${PTHREAD_DIR}/dll/pthreadVC2.dll)
include_directories(${PTHREAD_INCLUDE_DIR})
link_directories(${PTHREAD_LIBRARY_DIR})

#pdcurses
set(PDC_DIR ${ccudaq_SOURCE_DIR}/pdcurses)
set(PDC_INCLUDE_DIR ${PDC_DIR}/include)
set(PDC_LIBRARY_DIR ${PDC_DIR}/lib)
set(PDC_LIBRARIES pdcurses.lib)
set(PDC_DLLS ${PDC_DIR}/dll/pdcurses.dll)
include_directories(${PDC_INCLUDE_DIR})
link_directories(${PDC_LIBRARY_DIR})
add_definitions(-DPDC_WIDE -DPDC_FORCE_UTF8 -DPDC_DLL_BUILD)

#libusb
set(XXUSB_DIR ${ccudaq_SOURCE_DIR}/libusb_win32)
set(XXUSB_INCLUDE_DIR ${XXUSB_DIR}/include)
set(XXUSB_LIBRARY_DIR ${XXUSB_DIR}/lib)
set(XXUSB_LIBRARIES libusb.lib)
set(XXUSB_DLLS ${XXUSB_DIR}/dll/libusb0.dll)
include_directories(${XXUSB_INCLUDE_DIR})
link_directories(${XXUSB_LIBRARY_DIR})

#windows api
set(WIN_LIBRARIES user32.lib advapi32.lib kernel32.lib)

#ni-visa
set(VISA_INCLUDE_DIR "C:/Program Files/IVI Foundation/VISA/WinNT/include" CACHE PATH "choose the dir where visa.h is included")
set(VISA_LIBRARIES "C:/Program Files/IVI Foundation/VISA/WinNT/lib/msc/visa32.lib" CACHE FILEPATH
"choose the file visa32.lib")
include_directories(${VISA_INCLUDE_DIR})

#CAENHVWrapper
set(CAENHV_DIR ${ccudaq_SOURCE_DIR}/caenhvwrapper)
set(CAENHV_INCLUDE_DIR ${CAENHV_DIR}/include)
set(CAENHV_LIBRARIES ${CAENHV_DIR}/lib/CAENHVWrapper.lib)
set(CAENHV_DLLS ${CAENHV_DIR}/dll/CAENHVWrapper.dll)
include_directories(${CAENHV_INCLUDE_DIR})

#####################################################
set(SOURCES main.cpp nsclmodule.cpp CCCUSB.cpp CCCUSBReadoutList.cpp config.cpp display.cpp log.cpp manager.cpp VISASystemManager.cpp VISAInstrument.cpp AFG3252.cpp SYX527.cpp)
add_executable(ccudaq ${SOURCES})
target_link_libraries(ccudaq ${PTHREAD_LIBRARIES} ${PDC_LIBRARIES} ${XXUSB_LIBRARIES} ${WIN_LIBRARIES} ${VISA_LIBRARIES} ${CAENHV_LIBRARIES} ${ROOT_LIBRARIES})

file(COPY ${PTHREAD_DLLS} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PDC_DLLS} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${XXUSB_DLLS} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${CAENHV_DLLS} DESTINATION ${PROJECT_BINARY_DIR})
file(COPY cc.conf adc.conf pmt.conf DESTINATION ${PROJECT_BINARY_DIR})
#####################################################
