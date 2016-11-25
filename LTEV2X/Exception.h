#pragma once
#include<stdexcept>
#include<string>

class LTEV2X_Exception :public std::logic_error {
public:
	LTEV2X_Exception(std::string s) :std::logic_error(s) {}
};