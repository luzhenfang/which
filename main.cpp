#include <iostream>
#include <functional>
#include <regex>
#include <fstream>
#include <filesystem>
#include <map>
#include <thread>
#include <set>
#include <mutex>
#include "smartline.h"

/**
*	@file main.cpp
*	@author luckyFang
*	@desc ����c++17 �ļ���������
*		����ԭ��: ��ȡ$PATH��������,����Ŀ¼�б�
*			���̱߳���Ŀ¼,���뵽�����С�
*		�������ݽṹ: map<string,set>
*	@date 2021.2.28
*/

#define __VERSION__ "1.1"

// ����ģʽ
#define __DEBUG__ false

#define STATUS_FIND  true
#define STATUS_NOT_FOUND  false

auto container = std::make_shared < std::map < std::string, std::set <std::string>>> ();

// ����Ŀ¼��
auto traverseDir(std::string dir)noexcept {
	static std::mutex mtx;
	std::lock_guard<std::mutex>guard(mtx);

	dir = (dir.substr(dir.length() - 1, dir.length()) == ";") ? dir.substr(0, dir.length() - 2) : dir;

	if (dir.substr(dir.length() - 1, dir.length()) != "\\") {
		dir += "\\";
	}


	try {
		std::filesystem::recursive_directory_iterator end{};

		for (auto it = std::filesystem::recursive_directory_iterator{ dir }; it != end;++it) {
			auto stem = it->path().stem().string();
			auto extension = it->path().extension().string();
			auto path = it->path().string();
			auto key = stem + extension;

			if (!container->count(key)) {
				// �������򴴽�
				auto set = std::make_shared<std::set<std::string>>();
				set->insert(std::move(path));
				container->insert(std::pair(std::move(key), *set));
				continue;
			}


			// ������������
			auto i = container->find(key);
			auto& set = i->second;
			set.insert(std::move(path));
		}
	}
	catch (std::filesystem::filesystem_error) {
		// ignore
		// std::cout <<e.what()<< std::endl;
	}
}

// ����ָ��key��value
auto findEnv(std::string line) {
	 // ��ȷ����
	if (auto it = container->find(line); it != container->end()) return std::make_tuple(it->second,STATUS_FIND);
	// �����ȷ����û�ҵ������ȱʡֵ����
	if (auto it = container->find(line + ".exe"); it != container->end()) return std::make_tuple(it->second, STATUS_FIND);
	return std::make_tuple(std::set<std::string>(), STATUS_NOT_FOUND);
}




auto registerCmd(int argc,char**argv) {
	// ���������н�����
	smarline::Parser parser;
	// ע��������
	parser.add("-version", "-v", []() {
		std::cout << "which version \"" <<
			__VERSION__ << "\"" <<
			std::endl;
		});
	parser.add({
		{ "-help","--help" },
		{ "-h", "--h" }
		}, []() {
			std::cout << "Usage --"
				<< std::endl;
			std::cout << "\t" << "-v or -version �鿴�汾��Ϣ" << std::endl;
			std::cout << "\t" << "-h or -help �鿴������Ϣ" << std::endl;
		});
	parser.parse(argc, argv);
}

// ��ʼ��
auto init(int argc, char** argv) {
	// ע�ỷ������
	registerCmd(argc, argv);

	// ��ȡ��������
	std::string  env{ getenv("PATH") };
	std::regex pattern{ R"(.*?;+)" };
	std::regex_iterator start{ env.begin(),env.end(),pattern };
	decltype(start) end;


	for (auto it = start; it != end; ++it) {
		std::string dir{ (*it).str() };
		std::thread t(traverseDir, dir.substr(0, dir.length() - 1));
		t.join();
	}

}




int main(int argc, char* argv[]) {
	init(argc,argv);
	// �ܵ�����
	if (argc != 2) exit(-1);


	auto [result, status] = findEnv(argv[1]);
	if (!status) {
		std::cout << "not find anything!" << std::endl;
		return 0;
	}
	// ������
	std::for_each(result.begin(), result.end(), [](auto it) {
		std::cout << it << std::endl;
	});

	return 0;

#if __DEBUG__
	traverseDir(R"(E:\adb\)");

	for (auto it = container->begin(); it != container->end();++it) {
		std::cout << it->first << std::endl;
		auto vec = it->second;
		for (auto i = vec.begin(); i != vec.end(); ++i) {
			std::cout << "\t" << *i << std::endl;
		}
	}
	return 0;
#endif
}



