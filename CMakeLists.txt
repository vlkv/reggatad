cmake_minimum_required(VERSION 2.8)
project(ReggataDaemon)

# To create Eclipse CDT project, follow instructions: https://cmake.org/Wiki/Eclipse_CDT4_Generator


file(GLOB_RECURSE sources      src/main/*.cpp src/main/scanner/*.cc src/main/parser/*.cc)
file(GLOB_RECURSE sources_test src/test/*.cpp)
file(GLOB_RECURSE data resources/*)


add_executable(reggatad ${sources} ${data})

target_compile_options(reggatad PUBLIC -std=c++1y -Wall)

target_include_directories(reggatad PUBLIC src/main src/main/scanner src/main/parser)

file(COPY ${data} DESTINATION resources)
file(COPY reggatad.conf DESTINATION ".")

###############################################################################
## dependencies ###############################################################
###############################################################################

find_package(Boost 1.61.0 COMPONENTS filesystem system REQUIRED)


# set the POCO paths and libs
set(POCO_PREFIX      "/usr/local")
set(POCO_INCLUDE_DIR "${POCO_PREFIX}/include")
set(POCO_LIB_DIR     "${POCO_PREFIX}/lib")
set(POCO_LIBS        "${POCO_LIB_DIR}/libPocoNetd.so"
                     "${POCO_LIB_DIR}/libPocoUtild.so"
                     "${POCO_LIB_DIR}/libPocoJSONd.so"
                     "${POCO_LIB_DIR}/libPocoXMLd.so"
                     "${POCO_LIB_DIR}/libPocoFoundationd.so"
                     "pthread")

target_include_directories(reggatad PRIVATE "${POCO_INCLUDE_DIR}")


target_link_libraries(reggatad PUBLIC
  ${Boost_LIBRARIES}
  ${POCO_LIBS}
)



###############################################################################
## testing ####################################################################
###############################################################################

find_package(GTest)

if(GTEST_FOUND)
  add_executable(unit_tests ${sources_test} ${sources})

  # This define is added to prevent collision with the main.
  # It might be better solved by not adding the source with the main to the
  # testing target.
  target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

  # This allows us to use the executable as a link library, and inherit all 
  # linker options and library dependencies from it, by simply adding it as dependency.
  set_target_properties(reggatad PROPERTIES ENABLE_EXPORTS on)

  target_link_libraries(unit_tests PUBLIC
    ${GTEST_BOTH_LIBRARIES}
    reggatad
  )

  target_include_directories(unit_tests PUBLIC
    ${GTEST_INCLUDE_DIRS} # doesn't do anything on linux
  )
  
endif()

###############################################################################
## packaging ##################################################################
###############################################################################

install(TARGETS reggatad DESTINATION reggatad_destination)

install(DIRECTORY resources DESTINATION reggatad_destination)

set(CPACK_PACKAGE_NAME "ReggataDaemon")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
