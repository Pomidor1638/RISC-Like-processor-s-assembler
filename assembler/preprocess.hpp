#pragma once

#include "common.hpp"

class Preprocess
{
public:

	std::string preprocess(const std::string& source);

protected:

	bool in_macro_budy;

};