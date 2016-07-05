#pragma once
#include<stdexcept>
#include<string>

class Exp :public std::logic_error {
public:
	Exp(std::string s) :std::logic_error(s) {}
};