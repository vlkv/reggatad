cmake_minimum_required(VERSION 2.8)
project(ReggataDaemon)

file(GLOB_RECURSE sources_lib 
    src/*.cpp 
    src/cmd/*.cpp 
    src/scanner/*.cc 
    src/parser/*.cc)
file(GLOB_RECURSE sources_main main/main.cpp)
file(GLOB_RECURSE sources_test test/*.cpp)

add_library(reggata_lib STATIC ${sources_lib})
target_compile_options(reggata_lib PUBLIC -std=c++1y -Wall)
target_include_directories(reggata_lib PUBLIC
    src
    src/cmd
    src/scanner
    src/parser)

add_executable(reggatad ${sources_main})
target_compile_options(reggatad PUBLIC -std=c++1y -Wall)
target_include_directories(reggatad PUBLIC src src/scanner src/parser)

file(COPY reggatad.conf DESTINATION ".")
file(COPY test_data DESTINATION ".")

###############################################################################
## dependencies ###############################################################
###############################################################################

find_package(Boost 1.61.0 COMPONENTS filesystem system log REQUIRED)
add_definitions(-DBOOST_LOG_DYN_LINK=1)

set(POCO_PREFIX      "/usr/local")
set(POCO_INCLUDE_DIR "${POCO_PREFIX}/include")
set(POCO_LIB_DIR     "${POCO_PREFIX}/lib")
set(POCO_LIBS        "${POCO_LIB_DIR}/libPocoNetd.so"
                     "${POCO_LIB_DIR}/libPocoUtild.so"
                     "${POCO_LIB_DIR}/libPocoJSONd.so"
                     "${POCO_LIB_DIR}/libPocoXMLd.so"
                     "${POCO_LIB_DIR}/libPocoFoundationd.so"
                     "pthread")
target_include_directories(reggata_lib PRIVATE "${POCO_INCLUDE_DIR}")

set(ROCKSDB_INCLUDE_DIR "/usr/local/include")
target_include_directories(reggata_lib PRIVATE "${ROCKSDB_INCLUDE_DIR}")

set(ROCKSDB_LIBS "/usr/local/lib/librocksdb.a"
		"/usr/lib/x86_64-linux-gnu/libsnappy.a"
		"/usr/lib/x86_64-linux-gnu/libbz2.a")

set(USR_INCLUDE_DIR "/usr/include")
target_include_directories(reggata_lib PRIVATE "${USR_INCLUDE_DIR}")

target_link_libraries(reggata_lib PUBLIC
  ${Boost_LIBRARIES}
  ${POCO_LIBS}
  ${ROCKSDB_LIBS}
)

target_link_libraries(reggatad PUBLIC
    reggata_lib
    ${Boost_LIBRARIES}
)


###############################################################################
## testing ####################################################################
###############################################################################

find_package(GTest)

if(GTEST_FOUND)
  add_executable(unit_tests ${sources_test})

  # This define is added to prevent collision with the main.
  # It might be better solved by not adding the source with the main to the
  # testing target.
  target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

  # This allows us to inherit all 
  # linker options and library dependencies from it, by simply adding it as dependency.
  set_target_properties(reggata_lib PROPERTIES ENABLE_EXPORTS on)

  target_link_libraries(unit_tests PUBLIC
    reggata_lib    
    ${GTEST_BOTH_LIBRARIES}
  )

  target_include_directories(unit_tests PUBLIC
    ${GTEST_INCLUDE_DIRS} # doesn't do anything on linux
  )
  
endif()

###############################################################################
## packaging ##################################################################
###############################################################################

install(TARGETS reggatad DESTINATION reggatad_destination)

set(CPACK_PACKAGE_NAME "ReggataDaemon")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
