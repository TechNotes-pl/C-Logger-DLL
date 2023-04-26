#include "pch.h"

#include <string>
#include <iostream>

#include "StringData.h"

using namespace CLog;

#pragma region String utils

std::string StringData::read_data_string()
{
    if (char buffer[MAX_PATH] = { 0 }; get_data_file_path(buffer)) {
        std::ifstream data_file(buffer);
        if (data_file.is_open()) {
            std::string line;
            int i = 0;
            while (getline(data_file, line))
            {
                m_data_string += line;
                m_data_string += '\n';
                i++;
            }
            data_file.close();
        }
        else std::cout << "Error:: Unable to open file " << file_name << std::endl;
    }
    else
    {
        std::cout << "Error: File " << buffer << "not found" << std::endl;
    }

    return m_data_string;
}

std::string StringData::trim(const std::string& str, const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string StringData::reduce(const std::string& str,
    const std::string& fill = " ",
    const std::string& whitespace = " \t")
{
    // trim first
    auto result = trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}
#pragma endregion



bool StringData::file_exists(const std::string& path_name)
{
    struct stat buffer {};
    return stat(path_name.c_str(), &buffer) == 0;
}

std::wstring StringData::get_exe_path() 
{
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    const std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");

    return std::wstring(buffer).substr(0, pos);
}

bool StringData::get_data_file_path(char buffer[])
{
    const auto program_path = get_exe_path();

    // manually convert wstring to string
    for (int i = 0; i < static_cast<int>(program_path.size()); i++)
        buffer[i] = static_cast<char>(program_path[i]);

    strncat_s(buffer, MAX_PATH, "\\", 1);
    strncat_s(buffer, MAX_PATH, file_name, strlen(file_name));

    return file_exists(buffer) ? true : false;
}
