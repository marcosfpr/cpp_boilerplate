#######################################################
### Source                                          ###
#######################################################

# Set your sources inside src/
set(
    SOURCES
        src/boilerplate_library/format.cpp
)

add_library(
    ${LIBRARY_NAME}
    ${SOURCES}
)

# if not BUILD_CONAN
if(NOT BUILD_CONAN)

    CPMFindPackage(
        NAME boost-cmake
        GITHUB_REPOSITORY marcosfpr/boost-cmake
        VERSION 1.71.0
        FIND_PACKAGE_ARGUMENTS ""
    )

endif()

add_definitions(-DBOOST_ALL_DYN_LINK)
    
find_package(Threads REQUIRED)

target_include_directories(
    ${LIBRARY_NAME}
    PUBLIC include
    ${Boost_INCLUDE_DIRS}
)


if(BUILD_CONAN)
    target_link_libraries(${LIBRARY_NAME} PRIVATE ${CONAN_LIBS} Threads::Threads)
else()
    target_link_libraries(${LIBRARY_NAME} PRIVATE Boost::boost Threads::Threads)
endif()

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)

include(CheckSymbolExists)

# Some hack to MSVC and boost
if(MSVC)
    add_definitions("/EHsc")
endif() 

# Some hack to not depend on FILE* internals
# https://github.com/alandefreitas/matplotplusplus/issues/4
check_symbol_exists(__fbufsize "stdio_ext.h" HAVE_FBUFSIZE)
if (HAVE_FBUFSIZE)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE SOURCE_HAS_FBUFSIZE)
endif()

# Another hack to check for min in Windows.h
# http://www.suodenjoki.dk/us/archive/2010/min-max.htm
check_symbol_exists(min "Windows.h" HAVE_WINDOWS_MINMAX)
if (HAVE_WINDOWS_MINMAX)
    target_compile_definitions(${LIBRARY_NAME} PUBLIC NOMINMAX)
endif()

if (BUILD_WITH_PEDANTIC_WARNINGS)
    if (MSVC)
        target_compile_options(${LIBRARY_NAME} PRIVATE /W4 /WX)
    else ()
        target_compile_options(${LIBRARY_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
        # Allow the warnings related to the bundled CImg
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            target_compile_options(${LIBRARY_NAME} PRIVATE -Wno-null-pointer-arithmetic -Wno-char-subscripts)
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${LIBRARY_NAME} PRIVATE -Wno-error=class-memaccess -Wno-class-memaccess)
        else ()
            message(ERROR "Cannot disable the relevant warnings for ${CMAKE_CXX_COMPILER_ID}")
        endif ()
    endif ()
endif ()

# Setting the public header
 set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/boilerplate_library/boilerplate_library.hpp")

# Install
if (BUILD_INSTALLER)
    # Install targets
    install(TARGETS ${LIBRARY_NAME}
            EXPORT ${LIBRARY_NAME}Targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_LIBDIR}
            )

    # Install headers
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.hpp"
            )
endif()
