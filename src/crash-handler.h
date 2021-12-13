#pragma once

#include <stdexcept>

using namespace std;

class LowLevelCrashHandler {
    public:
        static void GenerateCrashReport(exception ex);
        static void upload(char* message);
};