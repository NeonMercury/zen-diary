#include "stdafx.h"
#include "ZenDiary.h"
#include "utf8.h"

namespace ZenDiary
{
	namespace Helpers
	{
		namespace String
		{
			std::string To()
			{
				return std::string();
			}

			std::string ExtractPath(const std::string &fullname)
			{
				size_t index = fullname.find_last_of("\\");
				if (index == std::string::npos)
				{
					index = fullname.find_last_of("/");
					if (index == std::string::npos)
					{
						return std::string();
					}
				}

				return fullname.substr(0, index + 1);
			}

			std::string ExtractExtension(const std::string &fullname)
			{
				size_t index = fullname.find_last_of(".");
				if (index == std::string::npos)
				{
					return std::string();					
				}

				return fullname.substr(index + 1);
			}

			std::string FilenameToUrl(const std::string &filename)
			{
				std::stringstream stream;
				stream << "file:///" << filename;
				return stream.str();
			}	

			std::string GenerateString(const std::string &alphabet, size_t count)
			{
				if (alphabet.length() == 0 || count == 0)
				{
					return std::string();
				}

				std::stringstream result;

				const size_t length = alphabet.length();

				for (size_t i = 0; i < count; i++)
				{
					size_t index = rand() % length;
					result << alphabet[index];
				}

				return result.str();
			}

			std::string ToUtf8(const std::wstring &src)
			{
				std::string result;				
				utf8::utf32to8(src.c_str(), src.c_str() + src.length(), std::back_inserter(result));

				return result;
			}

			std::string FormatTime(const SYSTEMTIME &value)
			{
				auto z = [](const int &v) -> std::string
				{
					std::string s;
					s = Helpers::String::To(v);
					if (s.length() < 2)
					{
						return "0" + s;
					}

					return s;
				};

				std::stringstream stream;
				stream << z(value.wYear) << "-" << z(value.wMonth) << "-" << z(value.wDay) << " " <<
					z(value.wHour) << ":" << z(value.wMinute) << ":" << z(value.wSecond);

				return stream.str();
			}

			std::string ToLower(const std::string &data)
			{
				return boost::to_lower_copy(data);
			}

			std::string ToUpper(const std::string &data)
			{
				return boost::to_upper_copy(data);
			}
		}
	}
}