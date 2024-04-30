#include "MemoryMapping.h"

#include <iostream>

MemoryMapping::MemoryMapping()
{

}

MemoryMapping::MemoryMapping(const std::string& filename)
{
	if (!open(filename)) { close(); }
}

bool MemoryMapping::open(const std::string& filename)
{
	// Create the file handle to the specified file so a process has a way to identify the file.
	// For the usage in this program, only reading a file is needed.
	m_hFile = ::CreateFileA(
		filename.c_str(),         // name of file
		GENERIC_READ,             // requested access mode of read. must have the same access of the file mapping object
		0,                        // prevent processes from sharing and prevent the file from being opened again until the handle is closed
		NULL,                     // no security attributes needed for the context of this program
		OPEN_EXISTING,            // only open existing files and will throw an error if a file doesn't exist
		FILE_ATTRIBUTE_READONLY,  // only allow the file to be read
		NULL);                    // template file parameter ignored when opening an existing file

	// Get the point to the structure that will stores the file size in bytes
	LARGE_INTEGER result;
	if (!GetFileSizeEx(m_hFile, &result))
	{
		std::cerr << "ERROR: couldn't get file size.\n";
		return false;
	}

	// The compiler supports 64-bit integers so grab the 64-bit filesize integer from the structure
	uint64_t filesize = 0;
	filesize = static_cast<uint64_t>(result.QuadPart);

	// Create the file mapping object
	m_hFileMapping = ::CreateFileMapping(
		m_hFile,                  // name of the file handle
		NULL,                     // no security attributes being used
		PAGE_READONLY,            // set read only for views and the file handle must be set to read access rights
		0,						  // a value of zero for this and the next argument sets the maximum size of the
		0,                        // file mapping object to the size of the file in the file handle
		NULL);                    // multiple mapping objects won't be access each other so no name needed for this object
	if (!m_hFileMapping)
	{
		std::cerr << "ERROR: File Mapping object failed to create.\n";
		return false;
	}

	// Setup of parameters for the part of the file to be set in the view.
 
	// The offset is where the view should begin in the file and it has to be a 
	// multiple of the allocation granularity of the system.
	uint64_t offset = 0;

	// The number of bytes of the file that is to be mapped to the view.
	size_t mappedBytes = 0;
	if (offset > filesize)
	{
		std::cerr << "ERROR: Outside of the end of the file.\n";
		return false;
	}

	// Set the number of bytes to be mapped to the amount of bytes in the file if mappedBytes becomes larger than the file size.
	if (offset + mappedBytes > filesize)
	{
		mappedBytes = size_t(filesize - offset);
	}

	DWORD offsetLow = DWORD(offset & 0xFFFFFFFF);
	DWORD offsetHigh = DWORD(offset >> 32);

	// Map a view of the file
	m_mapViewOfFile = ::MapViewOfFile(
		m_hFileMapping,          // name of the file mapping object
		FILE_MAP_READ,           // the requested access of read which the file mapping object must have the same access status
		offsetHigh,              // this and the next argument specify where the view begins in the file
		offsetLow,
		mappedBytes);            // the number of bytes of the file to be mapped to the view
	if (m_mapViewOfFile == NULL)
	{
		std::cerr << "ERROR: Failed creating the map view of the file.\n";
		return false;
	}

	return true;
}

void MemoryMapping::close()
{
	// must clean up the resources in reverse order of their creation
	// and zero out the memory address they were located at
	::UnmapViewOfFile(m_mapViewOfFile);
	m_mapViewOfFile = nullptr;
	::CloseHandle(m_hFileMapping);
	m_hFileMapping = NULL;
	::CloseHandle(m_hFile);
	m_hFile = NULL;
}

char* MemoryMapping::getData()
{
	return static_cast<char*>(m_mapViewOfFile);
}