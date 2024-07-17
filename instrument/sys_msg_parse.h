#pragma once

#include <string>
#include <vector>
#include <optional>

namespace AutoSys
{
	using std::string;
	using std::vector;

	struct Identify {
		string hardware;
		string software;
		string sn;
		string date;
	};

	//变压器扫描单行ASCII数据定义
	struct TransScanAsciiData
	{
		short pri;
		short sec;
		short paraIdx;	//参数功能索引;如dcr=8
		short cmp;		//比较结果0x01= true; 0x2 = false
		float val;		//具体测试结果
	};
	using TransScanAsciiDatas = std::vector<TransScanAsciiData>;


	enum class FetchMode
	{
		RT_VOID = 0,	//trig后不返回数据
		RT_DATA = 1,	//trig后自动返回数据
		RT_STATE = 2	//trig后不返回数据，返回测试状态(变压器分页扫描)
	};

	//仅fetchMode = 2有效(表示触发测试结果后返回结束状态，不直接返回结果)
	enum class TrigState
	{
		UNKNOWN = 0,
		EOM = 1,	//finish. end of message
		PAUSE = 2
	};

	//消息解析类
	class MsgParse
	{
	public :
		static std::string_view trimmed(std::string_view sv);	//去掉\r\n或\n

		static std::vector<std::string_view> splitString(std::string_view sv, char ch);

		static std::optional<Identify> parseIdentifyMsg(std::string_view msg);

		static std::optional <string> parseLoadFileMsg(std::string_view msg);

		static std::optional<FetchMode> parseFetchModeMsg(std::string_view msg);

		static std::optional<TrigState> parseTrigStateMsg(std::string_view msg);

		static std::vector<TransScanAsciiData> parseTransScanAData(std::string_view msg); //变压器扫描ascii数据

		static constexpr std::string_view trig_state_prefix = "Trig";
		static constexpr std::string_view trans_scan_prefix = "#";

	};

	//command 指令中的{}代表可变参数
	class SCPI_Command
	{
	public:
		static constexpr std::string_view query_idn = "*IDN?";

		static constexpr std::string_view set_fetch_mode = "FETCH:AUTO {}";

		static constexpr std::string_view query_fetch_mode = "FETCH:AUTO?";

		static constexpr std::string_view query_load_file = "MMEM:LOAD?";

		static constexpr std::string_view cmd_trig = "TRIG";

		static constexpr std::string_view cmd_load_file = "MMEM:LOAD {}";

		static constexpr std::string_view fetch_auto = "FETCH?";

		//{"TURN","LX", "Q", "LK", "CX", "CXD", "ZX", "ACR", "DCR", "PS", "BL", "PH"}
		static const std::vector<string> TrsPara;
		static constexpr std::string_view fetch_trs = "TRS:ADATA:{}?";	//查询变压器测试结果
	};

}

