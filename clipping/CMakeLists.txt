PROJECT(Clipping)
SET( ${PROJECT_NAME}_SOURCES
  main.cpp
  clipplane.cpp
  line_exact.cpp
  meshgen.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${SDL_LIBRARY} )
