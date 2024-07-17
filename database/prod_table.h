#pragma once

#include "prod_data.h"
#include "sqlite_orm/sqlite_orm.h"

namespace ProdDB 
{
	using namespace sqlite_orm;
	using namespace ProdData;

	//custom define sqlite_orm tables
	template <typename T>
	inline auto table_from()
	{
		throw std::invalid_argument("undefined sqlite orm table");
	}

	template<>
	inline auto table_from<Produce>()
	{
		return make_table<Produce>("produce",
			make_column("id", &Produce::id, primary_key().autoincrement()),
			make_column("sn", &Produce::sn, unique()),
			make_column("model", &Produce::model),
			make_column("state", &Produce::state),
			make_column("date", &Produce::date, default_value<std::string>("datetime('now', 'localtime')"))
		);
	}

	template<>
	inline auto table_from<ProdDCR>()
	{
		return make_table<ProdDCR>("prod_dcr",
			make_column("id", &ProdDCR::id),
			make_column("line", &ProdDCR::line),
			make_column("flag", &ProdDCR::flag),
			make_column("dcr_val", &ProdDCR::dcr_val)
		);
	}

	template<>
	inline auto table_from<ProdShort>()
	{
		return make_table<ProdShort>("prod_short",
			make_column("id", &ProdShort::id),
			make_column("line_a", &ProdShort::line_a),
			make_column("line_b", &ProdShort::line_b),
			make_column("flag", &ProdShort::flag),
			make_column("short_val", &ProdShort::short_val)
		);
	}

	/* create a sqlite_orm storage with custom schema */
	inline auto create_storage(const std::string& db_name)
	{
		return make_storage(db_name,
			table_from<Produce>(),
			table_from<ProdDCR>(),
			table_from<ProdShort>()
		);
	}

	using DBStorage = decltype(create_storage(""));
}