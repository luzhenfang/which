#pragma once

#include <functional>
#include <vector>


using callback = std::function<void(void)>;

namespace smarline {
	class Parser
	{
	public:
		Parser()=default;
		~Parser()=default;
		void add(std::string l,std::string s, callback call) {
			vector.push_back(std::make_pair(std::make_pair(l,s),call));
		}

		void add(std::initializer_list<std::initializer_list<std::string>> list,callback call) {
			for (auto it = list.begin(); it != list.end(); ++it) {
				for (auto i = it->begin(); i != it->end(); ++i) {
					vector.push_back(std::make_pair(std::make_pair(*i, *i), call));
				}	
			}
		}
		auto parse(int argc,char*argv[]) {
			bool available = false;
			for (auto i = 0; i < argc; ++i) {
				auto cmd = argv[i];
				for (auto& e : vector) {
					auto pair = e.first;
					available = false;
					if (pair.first == cmd || pair.second == cmd) {
						available = true;
						e.second();
						exit(0);
					}
				}
			}
			return available;
		}
	private:
		std::vector<std::pair<std::pair<std::string, std::string>,callback>>vector;
		// std::pair<std::pair<std::string, std::string>,std::function<void(void)>>
	};

}