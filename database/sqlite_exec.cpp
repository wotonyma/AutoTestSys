#include "sqlite_exec.h"
#include <sqlite3.h>
#include <fstream>

SQLiteExec::SQLiteExec(const std::string& db_name)
	: cb(nullptr)
	, db(nullptr)
{
	auto rc = sqlite3_open(db_name.c_str(), &db);
	if (rc != SQLITE_OK)
	{
		throw std::runtime_error(sqlite3_errmsg(db));
	}

	cb = default_callback;
}

SQLiteExec::~SQLiteExec()
{
	if (db != nullptr) 
	{
		sqlite3_close(db);
	}
}

void SQLiteExec::exec_sql(const std::string& sql) const
{
	char* zErrMsg = nullptr;
	auto rc = sqlite3_exec(db, sql.c_str(), cb, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::string msg(zErrMsg);
		sqlite3_free(zErrMsg);
		throw std::runtime_error(msg);
	}
}

void SQLiteExec::exec_sql_script(const std::filesystem::path& sql_file) const
{
	std::ifstream sql_fs(sql_file);
	if (!sql_fs.is_open())
	{
		throw std::runtime_error(sql_file.string());
	}

	std::istreambuf_iterator<char> beg(sql_fs), end;
	std::string sql_str(beg, end);
	exec_sql(sql_str);
}

int SQLiteExec::default_callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++) 
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
