#pragma once

#include <optional>
#include <memory>
#include <type_traits>
#include <QVariant>
#include <QByteArray>
#include "boost/pfr.hpp"

namespace qutils 
{
	//use std::true_type?
	template<typename>   constexpr bool is_optional_impl = false;
	template<typename T> constexpr bool is_optional_impl<std::optional<T>> = true;
	template<> constexpr bool is_optional_impl<std::nullopt_t> = true;
	template<typename T>
	constexpr bool is_optional = is_optional_impl<std::remove_cv_t<std::remove_reference_t<T>>>;

	template<typename>	 constexpr bool is_std_ptr_impl = false;
	template<typename T> constexpr bool is_std_ptr_impl<std::shared_ptr<T>> = true;
	template<typename T> constexpr bool is_std_ptr_impl<std::unique_ptr<T>> = true;
	template<typename T>
	constexpr bool is_std_ptr = is_std_ptr_impl<std::remove_cv_t<std::remove_reference_t<T>>>;

	/*sqlite-orm struct members to QVariant*/
	template<typename T>
	QVariant cvt_qvariant(T&& t)
	{
		if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string>) {
			//return QVariant(QString::fromLocal8Bit(t.c_str()));//solve MSVC Chinese garbled
			return QVariant(QByteArray(t.c_str()));
		}
		else if constexpr (is_optional<T>) {
			if (t.has_value()) return cvt_qvariant(t.value());
			else return QVariant();
		} 
		else if constexpr (is_std_ptr<T>) {
			if (t != nullptr) return cvt_qvariant(*t);
			else return QVariant();
		}
		else {
			return QVariant::fromValue(std::forward<T>(t));
		}
	}

	/*QVariant to sqlite-orm struct members*/
	template<typename T>
	void qvariant_cvt(const QVariant& var, T& t) 
	{
		if constexpr (std::is_same_v<std::remove_reference_t<T>, std::string>) {
			t = var.toByteArray().constData();
		}
		else if constexpr (is_optional<T>) {
			using Ty = std::remove_reference_t<decltype(t.value())>;
			if (!t.has_value()) {
				t = std::make_optional<Ty>();
			} 
			qvariant_cvt(var, *(t.operator->()));
		}
		else if constexpr (is_std_ptr<T>) {
			using Ty = std::remove_reference_t<decltype(*t)>;
			if (t == nullptr) {
				t = std::make_unique<Ty>();
			}
			qvariant_cvt(var, *t);
		}
		else {
			t = qvariant_cast<T>(var);
		}
	}


	/*reference https://stackoverflow.com/questions/21062864/ */
	/*use struct element index convert element to qvariant*/
	template<int N, typename T, typename F>
	decltype(auto) cvt_one(T& t, F func)
	{
		return func(boost::pfr::get<N>(t));
	}

	template<typename T, typename F, int... Is>
	decltype(auto) cvt_from_element(T& t, int index, F func, std::index_sequence<Is...>)
	{
		//using RT = decltype(cvt_one<0>(t, func)); //RT = QVariant
		using RT = QVariant;
		using FT = RT(T&, F);
		static constexpr FT* arr[] = {&cvt_one<Is, T, F>...};
		return arr[index](t, func);
	}

	template<typename T, typename F>
	decltype(auto) cvt_from_element(T& t, int index, F func)
	{
		constexpr int N = boost::pfr::tuple_size_v<T>;
		return cvt_from_element(t, index, func, std::make_index_sequence<N>{});
	}

	template<typename T>
	decltype(auto) cvt_from_element(T& t, int index)
	{
		constexpr auto func = [](auto&& elem){return cvt_qvariant(elem); };
		return cvt_from_element(t, index, func);
	}

	/*use index convert qvariant to struct element*/
	template<int N, typename V, typename T, typename F>
	void cvt_elem(V& v, T& t, F func)
	{
		func(v, boost::pfr::get<N>(t));
	}

	template<typename V, typename T, typename F, int... Is>
	void cvt_to_element(V& v, T& t, int index, F func, std::index_sequence<Is...>)
	{
		using FT = void(V&, T&, F);
		//获取模板函数地址 std::addressof(fun<Tx, Ty>) or &func<Tx, Ty>
		static constexpr FT* arr[] = { &cvt_elem<Is, V, T, F>... }; 
		arr[index](v, t, func);
	}

	template<typename V, typename T, typename F>
	void cvt_to_element(V& v, T& t, int index, F func)
	{
		constexpr int N = boost::pfr::tuple_size_v<T>;
		cvt_to_element(v, t, index, func, std::make_index_sequence<N>{});
	}

	template<typename V, typename T>
	void cvt_to_element(V& v, T& t, int index)
	{
		auto func = [](auto&& var, auto&& elem) {qutils::qvariant_cvt(var, elem); };
		cvt_to_element(v, t, index, func);
	}
}

