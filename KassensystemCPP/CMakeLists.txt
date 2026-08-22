# Qt-Integration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# VS-Generiert
# add_executable(KassensystemCPP
#  src/gui/Kassensystem.cpp
#  src/gui/Kassensystem.h
#  src/main.cpp
#)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.h)
add_executable(KassensystemCPP ${SOURCES} ${HEADERS})

target_include_directories(KassensystemCPP PRIVATE src) 
	# src als root pfad für die include suche hinzufügen 
	# -> relative pfade müsse nicht von jeder includierenden Datei definiert werden (z.B. #include "gui/Kassensystem.h" statt #include "../logic/beispiel.h")

target_compile_features(KassensystemCPP PRIVATE cxx_std_23)

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