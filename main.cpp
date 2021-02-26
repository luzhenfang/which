#include <iostream>
#include <functional>
#include <regex>
#include <fstream>
#include <filesystem>
#include <map>
#include <thread>
#include <mutex>
#include "smartline.h"

#define __VERSION__ "1.0"

auto container = std::make_shared<std::map<std::string, std::string>>();
std::mutex mu{};



// 遍历目录下文件
auto traverseDir(std::string dir) {
	dir = (dir.substr(dir.length() - 1, dir.length()) == ";") ? dir.substr(0, dir.length() - 2) : dir;

	if (dir.substr(dir.length() - 1, dir.length()) != "\\") {
		dir += "\\";
	}

	/*
	std::filesystem::path path{dir};
	if (!std::filesystem::exists(path)) {
		return std::string{ "null" };
	}
	*/

	try {
		std::filesystem::recursive_directory_iterator iter{ dir };
		for (const auto& it : iter) {
			if (it.is_directory()) {
				continue;
			}
			//std::cout <<"\t\t"<< it.path().filename() << std::endl;
			auto stem = it.path().stem().string();
			auto extension = it.path().extension().string();
			std::lock_guard<std::mutex>guard(mu);
			
			// m_lock->lock();
			 (*container).insert(std::make_pair(stem+extension, dir));
			// m_lock->unlock();
		}
	}
	catch (std::filesystem::filesystem_error e) {
		// ignore
		// std::cout <<e.what()<< std::endl;
	}
}



// 查找指定key的value
auto findEnv(std::string line) {

	 // 精确查找
	auto it = container->find(line);
	if (it != container->end()) {
		return it->second + it->first;
	}
	// 如果精确查找没找到则采用缺省值查找
	it = (*container).find(line+".exe");
	if (it != container->end()){
		return it->second + it->first;
	}
	return std::string{ "not find anything!" };
}


int main(int argc, char* argv[]) {

	// clock_t startTime, endTime;
	// startTime = clock();

	smarline::Parser parser;

	parser.add("-version", "-v",[]() {
		std::cout << "which version \"" <<
			__VERSION__ << "\"" <<
			std::endl;
	});

	// 在解析列表内
	if (parser.parse(argc, argv)) {
		return 0;
	}



	// 读取环境变量
	std::string  env{ getenv("PATH") };
	std::regex pattern{ R"(.*?;+)" };
	std::regex_iterator start{ env.begin(),env.end(),pattern };
	decltype(start) end;


	for (auto it = start; it != end; ++it) {
		std::string dir{ (*it).str() };
		std::thread t(traverseDir,dir.substr(0, dir.length() - 1));
		t.detach();
	}
	// endTime = clock();
	std::this_thread::sleep_for(std::chrono::seconds(1));

	/*
	char chInput[512];
	sprintf(chInput, "total:%d\n", container->size());
	OutputDebugStringA(chInput);
	*/


	// 管道参数
	if (argc == 2) {
		std::cout << findEnv(argv[1]) << std::endl;
		return 0;
	}


	return 0;
}

