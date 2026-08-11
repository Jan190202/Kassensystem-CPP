# Qt-Integration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# VS-Generiert
add_executable(KassensystemCPP "Kassensystem.cpp" "Kassensystem.h" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET KassensystemCPP PROPERTY CXX_STANDARD 20)
endif()

# Qt-Integration
find_package(Qt6 REQUIRED COMPONENTS Widgets)

target_link_libraries(KassensystemCPP PRIVATE Qt6::Widgets)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

add_custom_command(TARGET KassensystemCPP POST_BUILD
    COMMAND "${WINDEPLOYQT_EXECUTABLE}" 
        "$<TARGET_FILE:KassensystemCPP>"
        --no-translations
    COMMENT "Deploying Qt dependencies..."
)