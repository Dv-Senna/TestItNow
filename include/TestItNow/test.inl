#pragma once

#include "TestItNow/test.hpp"

#include "TestItNow/testManager.hpp"



namespace tin {
	Test::Test(const std::string &name, Callback test) {
		tin::TestManager::addTest(name, test);
	}


} // namespace tin