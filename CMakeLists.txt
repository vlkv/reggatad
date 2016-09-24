cmake_minimum_required(VERSION 2.8) # setting this is required
project(example_project)            # this sets the project name

file(GLOB_RECURSE sources      src/main/*.cpp src/main/*.h)
file(GLOB_RECURSE sources      src/main/scanner/*.cc src/main/scanner/*.h src/main/scanner/*.hh src/main/scanner/*.ih)
file(GLOB_RECURSE sources      src/main/parser/*.cc src/main/parser/*.h src/main/parser/*.ih)
file(GLOB_RECURSE sources_test src/test/*.cpp)
file(GLOB_RECURSE data resources/*)


add_executable(example ${sources} ${data})

target_compile_options(example PUBLIC -std=c++1y -Wall)

target_include_directories(example PUBLIC src/main src/main/scanner src/main/parser)

file(COPY ${data} DESTINATION resources)

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

target_include_directories(example PRIVATE "${POCO_INCLUDE_DIR}")


target_link_libraries(example PUBLIC
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
  set_target_properties(example PROPERTIES ENABLE_EXPORTS on)

  target_link_libraries(unit_tests PUBLIC
    ${GTEST_BOTH_LIBRARIES}
    example
  )

  target_include_directories(unit_tests PUBLIC
    ${GTEST_INCLUDE_DIRS} # doesn't do anything on linux
  )
  
endif()

###############################################################################
## packaging ##################################################################
###############################################################################

install(TARGETS example DESTINATION example_destination)

install(DIRECTORY resources DESTINATION example_destination)

set(CPACK_PACKAGE_NAME "MyExample")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
