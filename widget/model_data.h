#pragma once

#include <map>
#include <vector>
#include <numeric>
#include "prod_data.h"
#include "prod_table.h"
#include "qvariant_cvt.hpp"

template <typename T>
class ModelData
{
public:
	/*state of tableView rows*/
	enum DataState
	{
		Origin = 0x0,
		Insert = 0x1 << 0,
		Update = 0x1 << 1,
		Remove = 0x1 << 2
	};

	void insert(T&& data);	//emplace_back row

	void remove(size_t ri);	//remove row

	void update(size_t row, size_t col, QVariant& var);

	void set_data(std::vector<T>&& vec);

	inline size_t rows() const;			//display rows

	inline size_t cols() const;			//display cols

	QVariant elem_to_qvar(size_t row, size_t col) const;
	void elem_from_qvar(size_t row, size_t col, const QVariant& var);


	std::vector<T> datas;
	std::vector<size_t> view;	//table显示内容ids
	std::map<size_t, DataState> changed; //状态有改变的数据
};

template<typename T>
inline void ModelData<T>::insert(T&& data)
{
	datas.emplace_back(std::forward<T>(data));
	auto st = ModelData::DataState::Insert;
	auto idx = datas.size() - 1;
	changed.insert_or_assign(idx, st);
	view.emplace_back(idx);
}

template<typename T>
inline void ModelData<T>::remove(size_t ri)
{
	auto st = ModelData::DataState::Remove;
	changed.insert_or_assign(ri, st);
	view.erase(view.begin() + ri);
	//并非真的删除,只是view不显示,同步时数据库删除
}

template<typename T>
inline void ModelData<T>::update(size_t row, size_t col, QVariant& var)
{
	auto di = view[row];
	qutils::cvt_to_element(var, datas[di], col);
	changed[di] = changed[di] | DataState::Update;
}

template<typename T>
inline void ModelData<T>::set_data(std::vector<T>&& vec)
{
	changed.clear();
	view.swap(std::vector<size_t>(vec.size()));
	std::iota(view.begin(), view.end(), 0);
	datas = std::forward<decltype(vec)>(vec);
}

template<typename T>
inline size_t ModelData<T>::rows() const
{
	return view.size();
}

template<typename T>
inline size_t ModelData<T>::cols() const
{
	static constexpr int cols = boost::pfr::tuple_size_v<T>;
	return cols;
}

template<typename T>
inline QVariant ModelData<T>::elem_to_qvar(size_t row, size_t col) const
{
	return qutils::cvt_from_element(datas[view[row]], col);
}

template<typename T>
inline void ModelData<T>::elem_from_qvar(size_t row, size_t col, const QVariant& var)
{
	qutils::cvt_to_element(var, datas[view[row]], col);
}

