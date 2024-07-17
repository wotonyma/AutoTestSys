#pragma once

#include <QObject>
#include <memory>
#include "sys_msg_parse.h"

class QTcpSocket;

class AutoTestSystem : public QObject
{
	Q_OBJECT

public:
	AutoTestSystem(QObject *parent = nullptr);
	~AutoTestSystem();

	bool connectToHost(const QString& ip, int port);
	void command(const QString& cmd);

	bool loadFile(const std::string& filename);

	void setFetchMode(AutoSys::FetchMode mode);

	void trigger();
	AutoSys::TrigState trigger2();

	AutoSys::Identify querySystemIdentify();
	AutoSys::FetchMode queryFetchMode();
	AutoSys::string queryLoadFile();
	AutoSys::TransScanAsciiDatas queryTransScanADatas(size_t paraIdx = 8);

private:
	std::unique_ptr<QTcpSocket>  _socket;
};
