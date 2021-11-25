#include "pch.h"
#include "gtest/gtest.h"
// emu_6502.cpp : This file contains the 'main' function. Program execution begins and ends there.
// its a emulation for learning purposes
//

// Note in VS run the project in x86 mode as I didn't bother to apply the config for the x64

#if GTEST_OS_ESP8266 || GTEST_OS_ESP32
#if GTEST_OS_ESP8266
extern "C" {
#endif
	void setup() {
		testing::InitGoogleTest();
	}

	void loop() { RUN_ALL_TESTS(); }

#if GTEST_OS_ESP8266
}
#endif

#else

GTEST_API_ int main(int argc, char** argv) {
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
#endif