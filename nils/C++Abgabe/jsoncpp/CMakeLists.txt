cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project (jsondemo
	VERSION 1.0
	DESCRIPTION "Eine Applikation um jsoncpp zu demonstrieren")

# C++ Standart
set(CMAKE_CXX_STANDARD 20)

# Definition hinzufuegen
# add_definitions(-D....)

# Unterschiedliche Versionen für Debug und Release
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3 -s -pipe")

# Astyle Option in make
add_custom_target (astyle
	COMMAND
	COMMAND astyle -r ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*.cpp --exclude=build
	VERBATIM
)

# Exportdatei fuer die Einstellungen von CMake
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/include/config.h)

IF (MINGW)
	# Includedieren von JSON-CPP unter Windows
	include_directories(${PROJECT_SOURCE_DIR}/jsoncpp/include)
  	link_directories(${PROJECT_SOURCE_DIR}/jsoncpp/lib)
ENDIF ()

# Includes
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/path/include)

# Hier wird die Zielapplikation erstellt
add_executable(${PROJECT_NAME} main.cpp)

# Subdirectory
add_subdirectory(path)

# Library hinzufügen
IF (MINGW)
   target_link_libraries(${PROJECT_NAME} pathlib jsoncpp wsock32 ws2_32)
ELSE (MINGW)
   target_link_libraries(${PROJECT_NAME} pathlib jsoncpp)
ENDIF ()
