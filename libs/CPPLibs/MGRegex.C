#ifndef __CPPLibs_MGRegex_C__
#define __CPPLibs_MGRegex_C__

#include "MGRegex.h"

namespace MGRegex {

void TrimSelf(std::string& str) {
	const unsigned long first = str.find_first_not_of(' ');
	const unsigned long last  = str.find_last_not_of(' ');
	if (first == std::string::npos || last == std::string::npos) str.clear();
	else str.assign((str.begin() + first), (str.begin() + last + 1));
}


std::string Trim(const std::string& str) {
	std::string newStr = str;
	TrimSelf(newStr);
	return newStr;
}


void EraseSelf(std::string& str, const char target) {
	str.erase(std::remove_if(str.begin(), str.end(), ([&](const char& ch)->bool{return (target==ch);}) ), str.end());
}


std::string Erase(const std::string& str, const char target) {
	std::string newStr = str;
	EraseSelf(newStr, target);
	return newStr;
}

void ReplaceSelf(std::string& str, const std::string& expr, const std::string& fmt) {
	try {
		str = std::regex_replace(str, std::regex(expr), fmt);
	}
	catch (const std::regex_error& err) { 
		MGSys::ShowError(STR("<< ReplaceSelf >>  %s", err.what()),
		MGSys::Messages({
			STR("SOURCE : \"%s\"", str.c_str()), 
			STR("REGEX  : \"%s\"", expr.c_str())
		}));
	}
}

std::string Replace(const std::string& str, const std::string& expr, const std::string& fmt) {
	std::string newStr = str;
	ReplaceSelf(newStr, expr, fmt);
	return newStr;
}


std::vector<std::string> Split(const std::string& str, const std::string& expr) {
	const std::string&& trimStr = Trim(str);
	if (trimStr.empty()) return std::vector<std::string>();
	try {
        std::regex re(expr);
        const std::sregex_token_iterator end;
        std::sregex_token_iterator it(trimStr.cbegin(), trimStr.cend(), re, -1);
		
        std::vector<std::string> strvec;
        while (it != end) strvec.push_back(*it++);
		return strvec;
	}
	catch (const std::regex_error& err) { 
		MGSys::ShowError(STR("<< Split >>  %s", err.what()),
		MGSys::Messages({
			STR("SOURCE : \"%s\"", str.c_str()), 
			STR("REGEX  : \"%s\"", expr.c_str())
		}));
		return std::vector<std::string>(); 
	}
}


std::vector<std::string> Match(const std::string& str, const std::string& expr) {
	const std::string&& trimStr = Trim(str);
	if (trimStr.empty()) return std::vector<std::string>();
	try {
        std::regex re(expr);
        const std::sregex_token_iterator end;
        std::sregex_token_iterator it(trimStr.cbegin(), trimStr.cend(), re);
        
        std::vector<std::string> strvec;
        while (it != end) strvec.push_back(*it++);
		return strvec;
	}
	catch (const std::regex_error& err) { 
		MGSys::ShowError(STR("<< Match >>  %s", err.what()),
		MGSys::Messages({
			STR("SOURCE : \"%s\"", str.c_str()), 
			STR("REGEX  : \"%s\"", expr.c_str())
		}));
		return std::vector<std::string>(); 
	}
}


std::string StringIntegral(const std::string& str) { 
	std::vector<std::string>&& strvec = Match(str, Formula::kIntegral);
	if (strvec.empty()) return std::string();
	else return strvec.at(0);
}


template <class IntType = long long, typename std::enable_if<std::is_integral<IntType>::value, int>::type = 0>
std::pair<bool, IntType> ConvertFromStringToIntegral(const std::string& str) {
	std::vector<std::string>&& strvec = Match(str, Formula::kIntegral);
	if (strvec.empty()) return std::make_pair(false, IntType(0));
	else {
		try {
			long long tmpval = std::stoll(strvec.at(0));
			IntType value = static_cast<IntType>(tmpval);
			if (value != tmpval) throw std::out_of_range(CSTR("static_cast<%s>(%s) failure", typeid(IntType).name(), typeid(long long).name()));
			return std::make_pair(true, value);
		}
		catch (const std::logic_error & err) {
			MGSys::ShowError(STR("<< ConvertFromStringToIntegral >>  %s", err.what()));
			return std::make_pair(false, IntType(0));
		}
	}
}

std::string StringFloat(const std::string& str) { 
	std::vector<std::string>&& strvec = Match(str, Formula::kFloat);
	if (strvec.empty()) return std::string();
	else return strvec.at(0);
}


template <class RealType = long double, typename std::enable_if<std::is_floating_point<RealType>::value, int>::type = 0>
std::pair<bool, RealType> ConvertFromStringToFloat(const std::string& str) {
	std::vector<std::string>&& strvec = Match(str, Formula::kFloat);
	if (strvec.empty()) return std::make_pair(false, RealType(0.0));
	else {
		try {
			long double tmpval = std::stold(strvec.at(0));
			RealType value = static_cast<RealType>(tmpval);
			if (!std::isfinite(tmpval) || !std::isfinite(value)) throw std::out_of_range("is not finite");
			return std::make_pair(true, value);
		}
		catch (const std::logic_error & err) {
			MGSys::ShowError(STR("<< ConvertFromStringToFloat >>  %s", err.what()));
			return std::make_pair(false, RealType(0.0));
		}
	}
}

} // namespace MGRegex

#endif // __CPPLibs_MGRegex_C__
