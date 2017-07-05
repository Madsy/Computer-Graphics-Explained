PROJECT(affine_texture_mapping)
SET( ${PROJECT_NAME}_SOURCES
  main.cpp
  clipplane.cpp
  meshgen.cpp
  rasterizer.cpp
  texture.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${SDL_LIBRARY} ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ZLIB_LIBRARIES})
