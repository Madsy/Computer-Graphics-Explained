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
  ELSEIF( APPLE )
    
  ENDIF()
  MESSAGE( STATUS "CMAKE_PREFIX_PATH set to ${CMAKE_PREFIX_PATH}" )
ENDIF()

#########################################
##        Find the SDL library         ##
#########################################
IF( NOT SDL_INCLUDE_DIR AND SDL_LIBRARY )
  MESSAGE( FATAL_ERROR "Both the SDL_INCLUDE_DIR path and SDL_LIBRARY must be set." )
ELSEIF( SDL_INCLUDE_DIR AND NOT SDL_LIBRARY )
  MESSAGE( FATAL_ERROR "Both the SDL_INCLUDE_DIR path and SDL_LIBRARY must be set." )
ELSE( )
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

#########################################
##        Find the DevIL library       ##
#########################################
FIND_PACKAGE( DevIL REQUIRED )
IF( NOT IL_FOUND )
    MESSAGE( FATAL_ERROR 
    "Couldn't find the location of the libDevIL library. Either set CMAKE_PREFIX_PATH to be your"
    " path prefix (the directory under bin, lib and include), or set IL_INCLUDE_DIR, IL_LIBRARIES and ILU_LIBRARIES" )
ELSE ( NOT IL_FOUND )
    MESSAGE( STATUS "Found the libPNG library under ${PNG_INCLUDE_DIR} and ${PNG_LIBRARIES}. Continuing with build." )
ENDIF()

#########################################
##        Find the ZLIB library        ##
#########################################
IF( NOT ZLIB_INCLUDE_DIRS AND ZLIB_LIBRARIES )
  MESSAGE( FATAL_ERROR "Both the ZLIB_INCLUDE_DIRS path and ZLIB_LIBRARIES must be set." )
ELSEIF( ZLIB_INCLUDE_DIRS AND NOT ZLIB_LIBRARIES )
  MESSAGE( FATAL_ERROR "Both the ZLIB_INCLUDE_DIRS path and ZLIB_LIBRARIES must be set." )
ELSE( )
  MESSAGE( STATUS "ZLIB_INCLUDE_DIRS and ZLIB_LIBRARIES not set. Using CMAKE_PREFIX_PATH instead for searching." )
  FIND_PACKAGE( ZLIB REQUIRED )
  IF( NOT ZLIB_FOUND )
    MESSAGE( FATAL_ERROR 
    "Couldn't find the location of the zlib library. Either set CMAKE_PREFIX_PATH to be your"
    " path prefix (the directory under bin, lib and include), or set ZLIB_INCLUDE_DIRS and ZLIB_LIBRARIES" )
  ELSE ( NOT ZLIB_FOUND )
    MESSAGE( STATUS "Found the zlib library under ${ZLIB_INCLUDE_DIRS} and ${ZLIB_LIBRARIES}. Continuing with build." )
  ENDIF()
ELSEIF( ZLIB_INCLUDE_DIRS AND ZLIB_LIBRARIES )
  SET(ZLIB_FOUND "YES")
ENDIF()



# Force variables into the cache
SET( CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" CACHE PATH
  "The prefix path to the toolchain's /bin /include and /lib" FORCE )

include_directories( AFTER "${SDL_INCLUDE_DIR}" )
include_directories( AFTER "${IL_INCLUDE_DIR}" )
include_directories( AFTER "${ZLIB_INCLUDE_DIRS}" )
include_directories( AFTER "${CMAKE_SOURCE_DIR}/include" )

SET( CGE_EXAMPLES
  projection
  clipping
  rasterizer
  affine_texture_mapping
  perspective_texture_mapping
)
FOREACH(example ${CGE_EXAMPLES})
  ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${example} ${CMAKE_BINARY_DIR}/${example}/bin )
ENDFOREACH()


