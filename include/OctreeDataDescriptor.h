#pragma once

#include <string>

// Ideally we want pre-defined descriptors and get-functions for certain data types (ENUMS?)

struct OctreeDataDescriptor {
	// Base filename
	std::string data_name = ""; // Yes, C++11 allows us to put default values here.
	// Associated filestreams
	unsigned int start_byte = 0;
	unsigned int end_byte = 0;
};