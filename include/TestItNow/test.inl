#pragma once

#include <iostream>
#include "TestItNow/test.hpp"

#include "TestItNow/testManager.hpp"



namespace tin {
	Test::Test(const std::string &name, Callback test) {
		std::cout << "Constructor of test " << name << std::endl;
		tin::TestManager::addTest(name, test);
		std::cout << "Constructor of test " << name << " ended" << std::endl;
	}


} // namespace tin