#pragma once

#include <optional>
#include <cstdint>
#include <string>

namespace ProdData {

	struct Produce
	{
		std::int32_t id;	//pk
		std::string sn;		//unique
		std::string model;
		std::int16_t state;
		std::optional<std::string> date;
	};

	struct ProdDCR
	{
		std::int32_t id;	//from produce.id
		std::uint16_t line;
		std::uint16_t flag;
		std::optional<float> dcr_val;
	};

	struct ProdShort
	{
		std::int32_t id; //from produce.id
		std::uint16_t line_a;
		std::uint16_t line_b;
		std::uint16_t flag;
		std::optional<float> short_val;
	};
}



