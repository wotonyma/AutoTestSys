#include "prod_storage.h"
#include "prod_table.h"
#include <fmt/format.h>
#include <algorithm>

using namespace ProdDB;


std::string ProdDB::Date::toString()
{
	return fmt::format("{:04d}-{:02d}-{:02d}", year, month, day);
}

struct ProdDBStorage::_Storage
{
public:
	DBStorage db;
	_Storage(const std::string& db_name)
		: db(ProdDB::create_storage(db_name))
	{
	}
};


ProdDBStorage::ProdDBStorage(const std::string& db_name) 
	: _s(std::make_unique<_Storage>(db_name))
{
}

ProdDBStorage::~ProdDBStorage() = default;

ProdDBStorage::ProdDBStorage(ProdDBStorage&& rhs) = default;
ProdDBStorage& ProdDBStorage::operator=(ProdDBStorage&& rhs) = default;

std::optional<Produce> ProdDBStorage::queryProductBySn(const std::string& sn)
{
	auto objs = _s->db.iterate<Produce>(where(c(&Produce::sn) == sn));
	if (!objs.empty()) {
		return *objs.begin();
	}
	return std::nullopt;
}

std::vector<Produce> ProdDB::ProdDBStorage::queryProductLastNDays(int n_days) const
{
	auto str = fmt::format("{:+} day", n_days * -1);
	auto last_n_day = _s->db.select(date("now", "localtime", str)).front();
	return query<Produce>(where(c(&Produce::date) >= last_n_day));
}

std::vector<Produce> ProdDB::ProdDBStorage::queryProductByDate(Date date)
{
	return query<Produce>(where(c(&Produce::date) == date.toString()));
}

std::vector<Produce> ProdDB::ProdDBStorage::queryProductByDate(Date from, Date to)
{
	//return query<Produce>(between(c(&Produce::date), from.toString(), to.toString()));//(from, to)
	return query<Produce>(where(c(&Produce::date) >= from.toString() && c(&Produce::date) <= to.toString())); //[from, to]
}

std::vector<ProdDCR> ProdDB::ProdDBStorage::queryLaneDCR(const std::string& sn)
{
	//auto view = _s->db.iterate<ProdDCR>(where(is_equal(&ProdDCR::id, select(&Produce::id, where(is_equal(&Produce::sn, sn))))));
	return query<ProdDCR>(where(is_equal(&ProdDCR::id,
			select(&Produce::id, where(is_equal(&Produce::sn, sn)))))
		);
}

std::vector<ProdDCR> ProdDB::ProdDBStorage::queryLaneDCRGreaterThan(const std::string& sn, float val)
{
	return query<ProdDCR>(where(
		is_equal(&ProdDCR::id, select(&Produce::id, where(is_equal(&Produce::sn, sn)))) &&
		greater_than(&ProdDCR::dcr_val, val))
	);
}

std::vector<ProdDCR> ProdDB::ProdDBStorage::queryLaneDCRLesserThan(const std::string& sn, float val)
{
	return query<ProdDCR>(where(
		is_equal(&ProdDCR::id, select(&Produce::id, where(is_equal(&Produce::sn, sn)))) &&
		lesser_than(&ProdDCR::dcr_val, val))
	);
}
