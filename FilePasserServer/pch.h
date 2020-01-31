// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef PCH_H
#define PCH_H
#define BUF_SIZE 1000000
#define PATH_SIZE 100
#define FILE_NAME_SIZE 50

// add headers that you want to pre-compile here
#include "framework.h"
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <future>
using namespace std;

#endif //PCH_H
