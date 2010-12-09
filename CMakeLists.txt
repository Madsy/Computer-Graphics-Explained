#######################################################################
## NOTE: CMAKE_PREFIX_PATH MUST BE SET ON WINDOWS BUILDS!            ##
## If not set to the directory that holds the /bin /include and /lib ##
## for your compiler, then you have two build options:               ##
##                                                                   ##
## 1.) The env variables VCINSTALLDIR and WindowsSdkDir must be set  ##
##     for MSVC builds in the MSVC env command prompt, or:           ##
##                                                                   ##
## 2) MSYS and MinGW must be used, with /mingw mounted under the MSYS##
##    root.                                                          ##
##                                                                   ##
## If SDL_INCLUDE_DIR and SDL_LIBRARY are not set when doing Windows ##
## builds, then the SDL include path and library path are assumed to ##
## be equal to CMAKE_PREFIX_PATH. Additional searching is done, but  ##
## it might not work on Windows.                                     ##
#######################################################################
CMAKE_MINIMUM_REQUIRED( VERSION 2.6)
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

PROJECT(ComputerGraphicsExplained)
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

#For some reason, this IF always fails. Commented out until fixed.
IF( ${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR} )
  MESSAGE( FATAL_ERROR
  "Stopping you from doing an in-source build. Bad, BAD :-)!\n"
  "Rather do:\n"
  "mkdir MyBuild\n"
  "cd MyBuild\n"
  "cmake <params> ..\n"
  "make\n")
  ELSE()
    MESSAGE( STATUS "Good, you're doing an out-source build. Keep it that way." )
ENDIF()

IF( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
  SET( CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
ENDIF()

IF( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  SET( CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
ENDIF()

IF( APPLE )
  MESSAGE( FATAL_ERROR "Sorry, this makefile doesn't support OSX yet." )
ENDIF()

IF( NOT CMAKE_PREFIX_PATH )
  MESSAGE( WARNING "CMAKE_PREFIX_PATH not set. Attempting to guess.." )
  IF( WIN32 )
    IF( MSVC )
      SET( CMAKE_PREFIX_PATH "$ENV{VCINSTALLDIR};$ENV{WindowsSdkDir}" )
      IF( NOT CMAKE_PREFIX_PATH )
        MESSAGE(FATAL_ERROR "Couldn't find the VCINSTALLDIR and WindowsSdkDir environment variables." )
      ENDIF()
    ELSEIF( MINGW AND MSYS )
      SET(CMAKE_PREFIX_PATH "/mingw")
    ENDIF()
  ELSEIF( LINUX )
    SET(CMAKE_PREFIX_PATH "/usr")
  ENDIF()
  MESSAGE( STATUS "CMAKE_PREFIX_PATH set to ${CMAKE_PREFIX_PATH}" )
ENDIF()

IF( NOT SDL_INCLUDE_DIR AND SDL_LIBRARY )
  MESSAGE( FATAL_ERROR "Both the SDL_INCLUDE_DIR path and SDL_LIBRARY must be set." )
ELSEIF( SDL_INCLUDE_DIR AND NOT SDL_LIBRARY )
  MESSAGE( FATAL_ERROR "Both the SDL_INCLUDE_DIR path and SDL_LIBRARY must be set." )
ELSEIF( NOT SDL_INCLUDE_DIR AND NOT SDL_LIBRARY )
  MESSAGE( STATUS "SDL_INCLUDE_DIR and SDL_LIBRARY not set. Using CMAKE_PREFIX_PATH instead for searching." )
  FIND_PACKAGE( SDL REQUIRED )
  IF( NOT SDL_FOUND )
    MESSAGE( FATAL_ERROR 
    "Couldn't find the location of the SDL library. Either set CMAKE_PREFIX_PATH to be your"
    " path prefix (the directory under bin, lib and include), or set SDL_INCLUDE_DIR and SDL_LIBRARY" )
  ELSE ( NOT SDL_FOUND )
    MESSAGE( STATUS "Found the SDL library under ${SDL_INCLUDE_DIR} and ${SDL_LIBRARY}. Continuing with build." )
  ENDIF()
ELSEIF( SDL_INCLUDE_DIR AND SDL_LIBRARY )
  SET(SDL_FOUND "YES")
ENDIF()

MESSAGE( STATUS )
MESSAGE( STATUS "------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_PREFIX_PATH = \"${CMAKE_PREFIX_PATH}\"")
MESSAGE( STATUS "SDL_FOUND = ${SDL_FOUND}" )
MESSAGE( STATUS "SDL_LIBRARY = ${SDL_LIBRARY}" )
MESSAGE( STATUS "SDL_INCLUDE_DIR = ${SDL_INCLUDE_DIR}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "------------------------------------------------------------------------------" )
MESSAGE( STATUS )

# Force variables into the cache
SET( SDL_FOUND "${SDL_FOUND}" CACHE BOOL
  "The SDL library found status" FORCE )
SET( SDL_INCLUDE_DIR "${SDL_INCLUDE_DIR}" CACHE PATH
  "The SDL library include directory" FORCE )
SET( SDL_LIBRARY "${SDL_LIBRARY}" CACHE PATH
  "The SDL path to the SDL library" FORCE )
SET( CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" CACHE PATH
  "The prefix path to the toolchain's /bin /include and /lib" FORCE )

include_directories( AFTER "${SDL_INCLUDE_DIR}" )
include_directories( AFTER "${CMAKE_SOURCE_DIR}/include" )

SET( CGE_EXAMPLES
  projection
  clipping
)
FOREACH(example ${CGE_EXAMPLES})
  ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${example} ${CMAKE_BINARY_DIR}/${example}/bin )
ENDFOREACH()


