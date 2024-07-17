#pragma once

#include <memory>
#include <string>
#include <optional>
#include <vector>
#include "prod_data.h"

namespace ProdDB 
{
	using namespace ProdData;

	struct Date {
		int year;
		int month;
		int day;
		inline std::string toString();
	};

	class ProdDBStorage
	{
	public:
		ProdDBStorage(const std::string& db_name);
		~ProdDBStorage();

		ProdDBStorage(ProdDBStorage&& rhs);
		ProdDBStorage& operator=(ProdDBStorage&&);

		ProdDBStorage(const ProdDBStorage& rhs) = delete;
		ProdDBStorage& operator=(const ProdDBStorage& rhs) = delete;

		template<typename O>
		void insert(const O& o) const;

		template<typename It>
		void insert_range(It from, It to);

		template<typename T, typename Cond>
		std::vector<T> query(Cond&& cond) const;

		template<typename T, typename ...Args>
		decltype(auto) iterate_v(Args&&... args) const;

		template<typename T, typename ...Args>
		decltype(auto) select_t(T&& t, Args&&... args);

		std::optional<Produce> queryProductBySn(const std::string& sn);
		std::vector<Produce> queryProductLastNDays(int n_days) const;
		std::vector<Produce> queryProductByDate(Date date);
		std::vector<Produce> queryProductByDate(Date from, Date to);
		std::vector<ProdDCR> queryLaneDCR(const std::string& sn);
		std::vector<ProdDCR> queryLaneDCRGreaterThan(const std::string& sn, float val);
		std::vector<ProdDCR> queryLaneDCRLesserThan(const std::string& sn, float val);

	private:
		struct _Storage;
		std::unique_ptr<_Storage> _s;
	};

	template<typename O>
	void ProdDBStorage::insert(const O& o) const 
	{
		auto db = reinterpret_cast<ProdDB::DBStorage*>(_s.get());
		db->insert(o);
	}

	template<typename It>
	void ProdDBStorage::insert_range(It from, It to)
	{
		auto db = reinterpret_cast<ProdDB::DBStorage*>(_s.get());
		db->insert_range(from, to);
	}

	template<typename T, typename Cond>
	std::vector<T> ProdDBStorage::query(Cond&& cond) const
	{
		std::vector<T> objs;
		auto db = reinterpret_cast<ProdDB::DBStorage*>(_s.get());
		auto view = db->iterate<T>(std::forward<Cond>(cond));
		objs.reserve(view.size());
		std::copy(view.begin(), view.end(), std::back_inserter(objs));
		return objs;
	}

	template<typename T, typename ...Args>
	decltype(auto) ProdDBStorage::iterate_v(Args&&... args) const
	{
		/*can add log or Permissions etc.*/
		auto db = reinterpret_cast<ProdDB::DBStorage*>(_s.get());
		return db->iterate<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ...Args>
	decltype(auto) ProdDBStorage::select_t(T&& t, Args&&... args)
	{
		/*can add log or Permissions etc.*/
		auto db = reinterpret_cast<ProdDB::DBStorage*>(_s.get());
		return db->select(std::forward<T>(t), std::forward<Args>(args)...);
	}
}

