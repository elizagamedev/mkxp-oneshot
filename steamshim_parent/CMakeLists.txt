add_executable(steamshim WIN32
  steamshim_parent.cpp)

target_compile_definitions(steamshim PRIVATE
  -DGAME_LAUNCH_NAME="oneshot")

if(DEBUG)
  target_compile_definitions(steamshim PRIVATE
    -DSTEAMSHIM_DEBUG)
endif()

if(WIN32)
  target_sources(steamshim PRIVATE
    resources.rc)
endif()

target_link_libraries(steamshim PRIVATE
  CONAN_PKG::steamworks)
