#pragma once

#include <windows.h>
#include <string>

class MemoryMapping
{
	HANDLE		m_hFile				= NULL;
	HANDLE		m_hFileMapping		= NULL;
	void*		m_mapViewOfFile		= nullptr;

	MemoryMapping();

	bool open(const std::string& filename);

public:

	MemoryMapping(const std::string& filename);

	char* getData();
	void close();
};