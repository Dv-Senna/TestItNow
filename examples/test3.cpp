#include "TestItNow/TestItNow.hpp"


TIN_START_TEST(test3)
	TIN_CHECK(1 + 1 != 2);
	TIN_NAMED_CHECK("check2", 1 + 1 != 2);
	TIN_MANDATORY_CHECK(1 + 1 == 2);
	TIN_NAMED_MANDATORY_CHECK("check4", 1 + 1 != 2);
TIN_END_TEST(test3, "");
