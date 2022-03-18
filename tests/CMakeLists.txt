find_package(Threads REQUIRED)

# set Test executable variable
set(TEST_EXECUTABLE test_boilerplate)

add_executable(
  ${TEST_EXECUTABLE}
  main.cpp
  unit/library_test.cpp
)


target_compile_features(test_boilerplate PUBLIC cxx_std_20)

if (BUILD_CONAN)
        target_link_libraries(test_boilerplate boilerplate_library ${CONAN_LIBS}  Threads::Threads)
else()
        # GTest library
        CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG release-1.10.0
        VERSION 1.10.0
        OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
        )
        target_link_libraries(test_boilerplate boilerplate_library gtest gtest_main)
endif()

include_directories(${gtest_SOURCE_DIR}/boilerplate_library ${gtest_SOURCE_DIR})

target_link_libraries(test_boilerplate boilerplate_library)

# Create testing target and redirect its output to `Testing` folder
add_test(NAME test_boilerplate COMMAND test_boilerplate WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)

set_tests_properties(test_boilerplate
        PROPERTIES
        TIMEOUT 30
)

include(FindValgrind)

if(VALGRIND_FOUND)

        add_custom_target(valgrind_test
                COMMAND ${VALGRIND_PROGRAM} --leak-check=full $<TARGET_FILE:${TEST_EXECUTABLE}>
                DEPENDS ${TEST_EXECUTABLE}
                )
endif()