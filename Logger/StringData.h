#pragma once
#include <string>

namespace CLog {

	class StringData
	{
		inline static const char* file_name = R"(clog.conf)";
		std::string m_data_string{};

	public:
		std::string read_data_string();

	private:
		std::string trim(const std::string& str, const std::string& whitespace = " \t");

		std::string reduce(const std::string& str, const std::string& fill = " ", 
			const std::string& whitespace = " \t");


		bool file_exists(const std::string& path_name);

		std::wstring get_exe_path();

		bool get_data_file_path(char buffer[]);
	};
}