#pragma once

#include <cstdio>
#include <cstdlib>

#include "testManager.hpp"


tin::Result test1() {
	return tin::Result::ePassed;
};


int main(int argc, char **argv) {
	tin::TestManager::addTest("test1", test1);
	tin::TestManager::runTest("test2");
	tin::TestManager::runTest("test1");
	tin::TestManager::runTest("test3");
	return EXIT_SUCCESS;
}


#include "testManager.inl"
#include "test.inl"