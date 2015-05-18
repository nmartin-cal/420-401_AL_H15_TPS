cmake_minimum_required(VERSION 2.8)
#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Qt5Widgets QUIET)
find_package(Qt5Gui QUIET)
find_package(Qt5Core QUIET)
find_package(Qt5Network QUIET)

if (Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5Core_FOUND AND Qt5Network_FOUND)
    include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})
    add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS} ${Qt5Core_DEFINITIONS} ${Qt5Network_DEFINITIONS})
    list(APPEND LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES})
endif (Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5Core_FOUND AND Qt5Network_FOUND)

set(ALIENS_SRCS aliens/alien.cpp 
                aliens/alienEpoe.cpp 
                aliens/alienOg.cpp 
                aliens/alienUqomua.cpp 
                aliens/alienYuhq.cpp 
                aliens/alienSmart.cpp)

QT5_WRAP_CPP(MOCS_SERVER src/gui.h server/server.h)
QT5_ADD_RESOURCES(RCS_SERVER sprites.qrc)
add_executable(simul_server server/simul_server.cpp 
                            server/server.cpp 
                            src/game.cpp 
                            src/map.cpp 
                            src/gui.cpp
                            src/constants.cpp 
                            aliens/alienRemote.cpp
                            ${ALIENS_SRCS}
                            ${MOCS_SERVER} ${RCS_SERVER})
target_link_libraries(simul_server ${LIBRARIES})

QT5_WRAP_CPP(MOCS_CLIENT client/client.h)
add_executable(simul_client client/simul_client.cpp 
                            client/client.cpp 
                            client/clientprotocol.cpp 
                            src/constants.cpp 
                            ${ALIENS_SRCS} 
                            ${MOCS_CLIENT})
target_link_libraries(simul_client ${LIBRARIES})
