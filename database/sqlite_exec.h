#pragma once

#include <string>
#include <filesystem>

struct sqlite3;

class SQLiteExec
{
public:
	SQLiteExec(const std::string& db_name);
	~SQLiteExec();
	void exec_sql(const std::string& sql) const;
	void exec_sql_script(const std::filesystem::path& sql_file) const;

	using sqlite_callback = int(*)(void* NotUsed, int argc, char** argv, char** azColName);
	sqlite_callback cb; //查询回调
	static int default_callback(void* NotUsed, int argc, char** argv, char** azColName);

private:
	sqlite3* db;
};