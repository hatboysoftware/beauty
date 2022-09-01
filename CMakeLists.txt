cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(beauty VERSION 0.1.0)

include(CTest)
enable_testing()

set(BEAUTY_SOURCES
  src/HTTPClient.cpp
  src/RequestBuilder.cpp
  src/Response.cpp
  src/Error.cpp
  src/JSONResponse.cpp
)
add_library(beauty ${HTTP_ENGINE_SOURCES})

target_include_directories(beauty PUBLIC include)
target_include_directories(beauty PRIVATE src)

# root_certificates requires bigobj
if(MSVC)
  ADD_DEFINITIONS(/bigobj)
  SET(_wIN32_WiNDOwS 0x0A00) # Support Windows 10
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
  SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /bigobj")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
else()
  add_compile_options(-Wa,-mbig-obj)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Boost
set(BOOST_USE_STATIC_LIBS Yes)
find_package(Boost REQUIRED COMPONENTS json container)

target_link_libraries(beauty PRIVATE
  OpenSSL::SSL
  OpenSSL::Crypto
  Boost::container
  INTERFACE Boost::json
)

########################
# simple-server
set(SIMPLE_SERVER_SOURCES
  examples/simple-server.cpp
)
add_executable(simple_server ${SIMPLE_SERVER_SOURCES})
add_dependencies(simple_server beauty)
target_link_libraries(simple_server beauty)

########################
# simple-client
set(SIMPLE_CLIENT_SOURCES
  examples/simple-client.cpp
)
add_executable(simple_client ${SIMPLE_CLIENT_SOURCES})
add_dependencies(simple_client beauty)
target_link_libraries(simple_client beauty)

