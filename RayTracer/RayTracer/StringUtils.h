#pragma once
#include "stdafx.h"

namespace StringUtils
{
	std::vector<std::string> SplitStr(const std::string &inputStr, char delim)
	{
		std::stringstream ss;
		ss.str(inputStr);
		std::string item;

		std::vector<std::string> splitStr;
		while (std::getline(ss, item, delim))
		{
			splitStr.push_back(item);
		}

		return splitStr;
	}
}