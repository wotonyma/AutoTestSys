
set(db_inc "prod_data.h" "prod_table.h" "prod_storage.h" "sqlite_exec.h")
set(db_src "prod_storage.cpp" "sqlite_exec.cpp")

target_include_directories(${proj_exec} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${proj_exec} PRIVATE ${db_src})

find_package(fmt CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(SqliteOrm CONFIG REQUIRED)

target_link_libraries(${proj_exec} 
	PRIVATE 
		fmt::fmt-header-only 
		unofficial::sqlite3::sqlite3 
		sqlite_orm::sqlite_orm
)

#configure_file("prod_db.sql" "${CMAKE_BINARY_DIR}/config/prod_db.sql" COPYONLY) #OK
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/prod_db.sql" "${PROJECT_BINARY_DIR}/config/prod_db.sql")
