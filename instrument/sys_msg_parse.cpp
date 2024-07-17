
#include "sys_msg_parse.h"
#include <charconv>
#include <iostream>

using namespace AutoSys;

const std::vector<string> SCPI_Command::TrsPara = { "TURN","LX", "Q", "LK", "CX", "CXD", "ZX", "ACR", "DCR", "PS", "BL", "PH" };

std::string_view AutoSys::MsgParse::trimmed(std::string_view sv)
{
	if (sv.empty()) return sv;

	if (sv.back() == '\n')
		sv = sv.substr(0, sv.size() - 1);
	if (sv.back() == '\r')
		sv = sv.substr(0, sv.size() - 1);
	return sv;
}

std::vector<std::string_view> AutoSys::MsgParse::splitString(std::string_view sv, char ch)
{
	std::vector<std::string_view> datas;

	int npos = 0;
	for (size_t i = 0; i < sv.size(); ++i)
	{
		if (sv[i] == ch) {
			if (i > npos)
				datas.emplace_back(&sv[npos], i - npos);
			npos = i + 1;
		}
	}
	if (npos < sv.size())
		datas.emplace_back(&sv[npos], sv.size() - npos);

	return datas;
}

std::optional<Identify> AutoSys::MsgParse::parseIdentifyMsg(std::string_view msg)
{
	auto smpf = trimmed(msg);
	auto vec = splitString(smpf, ',');
	if (vec.size() != 4)
		return std::nullopt;
	
	std::optional<Identify> opt{ std::in_place };
	opt.value().hardware = vec[0];
	opt.value().software = vec[1];
	opt.value().sn = vec[2];
	opt.value().date = vec[3];

	return opt;
}

std::optional<string> AutoSys::MsgParse::parseLoadFileMsg(std::string_view msg)
{
	auto smpf = trimmed(msg);
	if (msg.empty())
		return std::optional<string>();
	return std::optional<string>(smpf);
}

std::optional<FetchMode> AutoSys::MsgParse::parseFetchModeMsg(std::string_view msg)
{
	auto smpf = trimmed(msg);
	if (msg.empty())
		return std::optional<FetchMode>();

	int num = 0;
	auto res = std::from_chars(smpf.data(), smpf.data() + smpf.size(), num);
	if (res.ec == std::errc::invalid_argument)
		return std::nullopt;

	return static_cast<FetchMode>(num);
}

std::optional<TrigState> AutoSys::MsgParse::parseTrigStateMsg(std::string_view msg)
{
	if (msg.empty())
		return std::optional<TrigState>();
	if (msg.substr(0, trig_state_prefix.size()) != trig_state_prefix)
		return std::nullopt;

	if (msg.find("Eom") != std::string_view::npos)
		return TrigState::EOM;
	if (msg.find("Pause") != std::string_view::npos)
		return TrigState::PAUSE;
	return TrigState::UNKNOWN;
}

std::vector<TransScanAsciiData> AutoSys::MsgParse::parseTransScanAData(std::string_view msg)
{
	//#seg1,<pri,sec,paraIdx,comp,val;>*PriSet*SecSet\n 
	//其中seg1表示总比较结果,0(未比较), '1'(合格),'2|3|4'(不合格)
	std::vector<TransScanAsciiData> datas;
	if (msg.empty())
		return datas;
	if (msg.substr(0, trans_scan_prefix.size()) != trans_scan_prefix)
		return datas;

	auto smpf = trimmed(msg);
	int seg1 = smpf[trans_scan_prefix.size()] - '0';
	
	constexpr auto vec2Data = [](std::vector<std::string_view>& vec) {
		//string_view to int/float/...
		constexpr auto str_view_cvt = [](std::string_view sv, auto& val) {
			return std::from_chars(sv.data(), sv.data() + sv.size(), val);
			};
		TransScanAsciiData data;
		str_view_cvt(vec[0], data.pri);
		str_view_cvt(vec[1], data.sec);
		str_view_cvt(vec[2], data.paraIdx);
		str_view_cvt(vec[3], data.cmp);
		str_view_cvt(vec[4], data.val);
		return data;
		};
	
	auto lines = splitString(smpf.substr(2,smpf.size()), ';');
	for (auto& line : lines)
	{
		auto vec = splitString(line, ',');
		if (vec.size() != 5)
			continue;
		datas.emplace_back(vec2Data(vec));
	}
	return datas;
}
