
#include <iostream>
#include "sys_msg_parse.h"
#include "auto_test_system.h"
#include "fmt/printf.h"

int main(int argc, char* argv[])
{
	std::cout << "hddd\n";
	return 0;

	AutoTestSystem sys;
	sys.connectToHost("192.168.22.209", 45454);

	auto info = sys.querySystemIdentify();
	fmt::print("{}, {}, {}, {}\n", info.hardware, info.sn, info.software, info.date);

	sys.setFetchMode(AutoSys::FetchMode::RT_STATE);	//设置fetch模式
	_sleep(100);

	while (true)
	{
		auto st = sys.trigger2();
		if (st == AutoSys::TrigState::EOM)
			break;
	}

	/*sys.trigger();
	_sleep(200);
	sys.trigger();*/

	auto datas = sys.queryTransScanADatas();
	for (auto& data : datas)
	{
		fmt::print("pri:{}, sec:{}, paraIdx:{}, cmp:{}, val:{}\n",
			data.pri, data.sec, data.paraIdx, data.cmp, data.val);
	}

	return 0;
}