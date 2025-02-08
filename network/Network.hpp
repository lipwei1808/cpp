#pragma once

#include "String.hpp"

int connectToHost(const char* hostname, const char* port);
bool Send(int fd, const String& s);
bool Receive(int fd, String& s);
