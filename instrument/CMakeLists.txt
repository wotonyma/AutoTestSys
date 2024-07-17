
set(instr_inc "auto_test_system.h" "sys_msg_parse.h")
set(instr_src "auto_test_system.cpp" "sys_msg_parse.cpp")

target_sources(${proj_exec} PUBLIC ${instr_inc} ${instr_src} )

target_include_directories(${proj_exec} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}/lib/cmake") #注意要用大写的APPEND
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Network REQUIRED)       #查找Qt6或Qt5，导入Widgets模块
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Network REQUIRED)  #查找Qt具体版本，导入Widgets模块
find_package(fmt CONFIG REQUIRED)

target_link_libraries(${proj_exec} 
	PUBLIC 
		Qt${QT_VERSION_MAJOR}::Core
		Qt${QT_VERSION_MAJOR}::Network
		Qt${QT_VERSION_MAJOR}::Widgets
		fmt::fmt-header-only
)
