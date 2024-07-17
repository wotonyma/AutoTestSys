#include "auto_test_system.h"
#include <QTcpSocket>
#include <exception>
#include <thread>
#include "sys_msg_parse.h"
#include "fmt/format.h"

using namespace AutoSys;

AutoTestSystem::AutoTestSystem(QObject *parent)
	: QObject(parent)
{
	_socket = std::make_unique<QTcpSocket>();
}

AutoTestSystem::~AutoTestSystem()
{
	_socket->disconnectFromHost();
}

bool AutoTestSystem::connectToHost(const QString& ip, int port)
{
	_socket->connectToHost(ip, port);
	return _socket->waitForConnected();
}

void AutoTestSystem::command(const QString& cmd)
{
	QString line = cmd + "\n";
	_socket->write(line.toUtf8());
	_socket->flush();
}

bool AutoTestSystem::loadFile(const std::string& filename)
{
	auto cmd = fmt::format(SCPI_Command::cmd_load_file, filename);
	command(cmd.c_str());
	std::this_thread::sleep_for(std::chrono::seconds(3));

	auto cur_file = queryLoadFile();
	if (filename == cur_file)
		return true;
	return false;
}

void AutoTestSystem::setFetchMode(AutoSys::FetchMode mode)
{
	int im = static_cast<int>(mode);
	auto cmd = fmt::format(SCPI_Command::set_fetch_mode, im);
	command(cmd.c_str());
}

void AutoTestSystem::trigger()
{
	command(SCPI_Command::cmd_trig.data());
}

/// <summary>
/// 仅当fectchmode = RT_STATE有效
/// </summary>
/// <returns>返回数据测试状态</returns>
AutoSys::TrigState AutoTestSystem::trigger2()
{
	command(SCPI_Command::cmd_trig.data());
	if (!_socket->waitForReadyRead(3000))
		throw std::runtime_error("trigger response timeout!");
	auto bytes = _socket->readLine();
	auto state = AutoSys::MsgParse::parseTrigStateMsg({ bytes });
	if (!state.has_value())
		throw std::runtime_error("trigger response error!");
	return state.value();
}

AutoSys::Identify AutoTestSystem::querySystemIdentify()
{
	command(SCPI_Command::query_idn.data());
	if (!_socket->waitForReadyRead())
		throw std::runtime_error("query system identify response timeout!");
	auto bytes = _socket->readLine();
	auto res = AutoSys::MsgParse::parseIdentifyMsg({ bytes });
	if (!res.has_value())
		throw std::runtime_error("query system identify response error!");
	return res.value();
}

/// <summary>
/// 查询触发后数据返回模式
/// </summary>
/// <returns>0:不自动返回 1:自动返回 2:仅返回状态</returns>
AutoSys::FetchMode AutoTestSystem::queryFetchMode()
{
	command(SCPI_Command::query_fetch_mode.data());
	if (!_socket->waitForReadyRead())
		throw std::runtime_error("query fetch mode response timeout!");
	auto bytes = _socket->readLine();
	auto res = AutoSys::MsgParse::parseFetchModeMsg({ bytes });
	if (!res.has_value())
		throw std::runtime_error("query fetch mode response error!");
	return res.value();
}

AutoSys::string AutoTestSystem::queryLoadFile()
{
	command(SCPI_Command::query_load_file.data());
	if (!_socket->waitForReadyRead())
		throw std::runtime_error("query load file response timeout!");
	auto bytes = _socket->readLine();
	auto res = AutoSys::MsgParse::parseLoadFileMsg({ bytes });
	if (!res.has_value())
		throw std::runtime_error("query load file response error!");
	return res.value();
}

/// <summary>
/// 主动查询测试数据
/// </summary>
/// <param name="paraIdx">变压器扫描相关参数id;详情请参考TH2840手册章节 9.2.20 TRS变压器扫描命令集</param>
/// <returns>测试结果数组</returns>
AutoSys::TransScanAsciiDatas AutoTestSystem::queryTransScanADatas(size_t paraIdx)
{
	auto paraIdx_str = AutoSys::SCPI_Command::TrsPara[paraIdx];
	auto cmd = fmt::format(SCPI_Command::fetch_trs, paraIdx_str);
	command(cmd.c_str());
	if (!_socket->waitForReadyRead())
		throw std::runtime_error("query trs data response timeout!");
	auto bytes = _socket->readLine();
	auto datas = AutoSys::MsgParse::parseTransScanAData({ bytes });
	return datas;
}
