set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${proj_exec} "main.cpp")

list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}/lib/cmake") #注意要用大写的APPEND
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Network REQUIRED)       #查找Qt6或Qt5，导入Widgets模块
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Network REQUIRED)  #查找Qt具体版本，导入Widgets模块

find_package(fmt CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(SqliteOrm CONFIG REQUIRED)

set(widget_inc "db_widget.h" "DBAbstractTableModel.h")
set(widget_src "db_widget.cpp" "DBAbstractTableModel.cpp")

target_sources(${proj_exec} PUBLIC ${widget_inc} ${widget_src})

find_package(Boost REQUIRED COMPONENTS pfr)
target_link_libraries(${proj_exec} PRIVATE Boost::pfr)


