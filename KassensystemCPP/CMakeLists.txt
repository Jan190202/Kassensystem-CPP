# Qt-Integration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.h)
add_executable(KassensystemCPP ${SOURCES} ${HEADERS})

# prevent interference with accidental macro-usage in external libs (slots, ...)
target_compile_definitions(KassensystemCPP PRIVATE QT_NO_KEYWORDS) 

target_include_directories(KassensystemCPP PRIVATE src)

# C++23
target_compile_features(KassensystemCPP PRIVATE cxx_std_23)

# Qt-Integration
find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(KassensystemCPP PRIVATE Qt6::Widgets)

# Python integration
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
get_filename_component(PYTHON_HOME_DIR "${Python3_RUNTIME_LIBRARY_RELEASE}" DIRECTORY)
target_compile_definitions(KassensystemCPP PRIVATE PYTHON_HOME_DIR="${PYTHON_HOME_DIR}")

# pybind11 integration
add_subdirectory(extern/pybind11)
target_link_libraries(KassensystemCPP PRIVATE pybind11::embed)

# copy python3xx.dll into build
add_custom_command(TARGET KassensystemCPP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Python3_RUNTIME_LIBRARY_RELEASE}"
        "$<TARGET_FILE_DIR:KassensystemCPP>"
)

# gather all python scripts, create scripts folder in build, copy them there for interpretation at runtime
file(GLOB PYTHON_SCRIPTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/ocr/*.py")

add_custom_command(TARGET KassensystemCPP POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:KassensystemCPP>/scripts")

foreach(script ${PYTHON_SCRIPTS})
    add_custom_command(TARGET KassensystemCPP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${script}"
            "$<TARGET_FILE_DIR:KassensystemCPP>/scripts/"
    )
endforeach()

# deploy Qt dependencies
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

add_custom_command(TARGET KassensystemCPP POST_BUILD
    COMMAND "${WINDEPLOYQT_EXECUTABLE}" 
        "$<TARGET_FILE:KassensystemCPP>"
        --no-translations
)