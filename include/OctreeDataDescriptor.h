#pragma once

#include <string>

// Ideally we want pre-defined descriptors and get-functions for certain data types (ENUMS?)

struct OctreeDataDescriptor {
	// Base filename
	std::string data_name;
	// Associated filestreams
	unsigned int start_byte;
	unsigned int end_byte;
};