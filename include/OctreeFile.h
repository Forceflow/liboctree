#pragma once

// STL includes
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
// Custom includes
#include "OctreeNode.h"
#include "OctreeDataDescriptor.h"

// ENUM FOR STATE? (EMPTY / FILLED)

// A class to interact with an Octree file and its various file streams
class OctreeFile {
public:
	// Base filename
	std::string base_filename;
	// Associated filestreams (C++ version)
	std::fstream file_header;
	std::fstream file_node;
	std::fstream file_data;
	// Grid dimensions
	unsigned int grid[3];
	// Nodes info
	size_t node_count;
	// Data info
	size_t data_count;
	size_t data_size;
	// Data descriptors
	std::vector<OctreeDataDescriptor> data_descriptors;

	// constructor
	inline OctreeFile::OctreeFile(std::string &file_location, bool creation) : 
	base_filename(file_location.substr(0, file_location.find_last_of("."))), node_count(0), data_count(0), data_size(0), grid{0,0,0} {
		openFilestreams();
	}

	// Read size_data bytes from data file, returns current read position
	inline size_t OctreeFile::readData(byte* data) {
		size_t readpos = file_data.tellg();
		file_data.read((char*)data, data_size);
		return readpos;
	}

	// Read size_data bytes from data file at position
	inline void OctreeFile::readData(byte* data, size_t position) {
		file_data.seekg(position);
		file_data.read((char*)data, data_size);
		return;
	}

	// Read size_data bytes from address and write to data file
	// Returns position (bytes) where this was written.
	inline size_t OctreeFile::appendData(const byte* data) {
		// Go to end of file
		file_data.seekg(file_data.end);
		size_t writepos = file_data.tellp();
		// Write data
		file_data.write((char*)data, data_size);
		// Cleanup
		data_count++;
		return writepos;
	}

	// Read size_node bytes from node file, returns current read position
	inline size_t OctreeFile::readNode(OctreeNode& node) {
		size_t readpos = file_node.tellg();
		file_node.read((char*) &(node.childmask), OctreeNode::getSize());
		return readpos;
	}

	// Write size_node bytes to node file
	// Returns position (bytes) where this was written.
	inline size_t OctreeFile::appendNode(const OctreeNode& node) {
		// Go to end of file
		file_node.seekg(file_node.end);
		size_t writepos = file_node.tellp();
		// Write Node data 
		// TODO: Can probably do this in one write
		file_node.write((char*) &(node.childmask), sizeof(node.childmask));
		file_node.write((char*) &(node.children_base), sizeof(node.children_base));
		file_node.write((char*) &(node.data), sizeof(node.data));
		// Cleanup
		node_count++;
		return writepos;
	}

	// read data from octree header
	inline bool OctreeFile::readHeader() {
		std::string word;
		// Check that this is an #octree file
		file_header >> word;
		if (word.compare("#octree") != 0) { std::cout << "Error: first word of .octree file reads [" << word.c_str() << "] instead of #octree" << std::endl; exit(1); }
		// Read header and save data
		bool done = false;
		while (file_header.good() && !done) {
			file_header >> word;
			if (word.compare("END") == 0) { done = true; }
			else if (word.compare("node_count") == 0) { file_header >> this->node_count; }
			else if (word.compare("data_count") == 0) { file_header >> this->data_count; }
			else if (word.compare("data_size") == 0) { file_header >> this->data_size; }
			else if (word.compare("grid") == 0) {
				file_header >> grid[0];
				file_header >> grid[1];
				file_header >> grid[2];
			}
			else if (word.compare("data_descriptor") == 0) {
				OctreeDataDescriptor desc;
				file_header >> desc.data_name;
				file_header >> desc.start_byte;
				file_header >> desc.end_byte;
				this->data_descriptors.push_back(desc);
			}
			else {
				std::cout << "  unrecognized keyword [" << word << "], skipping" << std::endl;
				char c; do { c = file_header.get(); } while (file_header.good() && (c != '\n'));
			}
		}
	}

	// delete original header file write again (TIP: use truncate)
	inline void OctreeFile::writeHeader() {

	}

	inline void OctreeFile::openFilestreams() {
		std::string filename = base_filename + ".octree";
		file_header.open(filename.c_str(), std::ios::in | std::ios::out); // Text-based
		filename = base_filename + ".octreenodes";
		file_node.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::binary); // Binary
		filename = base_filename + ".octreedata";
		file_data.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::binary); // Binary
	}

	inline void OctreeFile::flushFilestreams() {
		file_header.flush();
		file_node.flush();
		file_data.flush();
	}

	inline void OctreeFile::closeFilestreams() {
		file_header.close();
		file_node.close();
		file_data.close();
	}

	// destructor shoul do this as well
};

//// Write an octree header to a file
//inline void writeOctreeHeader(const std::string &filename, const OctreeInfo &i){
//	ofstream outfile;
//	outfile.open(filename.c_str(), ios::out);
//	outfile << "#octreeheader 1" << endl;
//	outfile << "gridlength " << i.gridlength << endl;
//	outfile << "n_nodes " << i.n_nodes << endl;
//	outfile << "n_data " << i.n_data << endl;
//	outfile << "END" << endl;
//	outfile.close();
//}
//
//// Parse a given octree header, store info in OctreeInfo struct
//inline int parseOctreeHeader(const std::string &filename, OctreeInfo &i){
//	cout << "  reading octree header from " << filename << " ... " << endl;
//	ifstream headerfile;
//	headerfile.open(filename.c_str(), ios::in);
//
//	i.base_filename = filename.substr(0,filename.find_last_of("."));
//
//	string line; bool done = false;
//	headerfile >> line >> i.version;
//	if (line.compare("#octreeheader") != 0) {cout << "    Error: first line reads [" << line << "] instead of [#octreeheader]" << endl; return 0;}
//
//	while(headerfile.good() && !done) {
//		headerfile >> line;
//		if (line.compare("END") == 0) done = true; // when we encounter data keyword, we're at the end of the ASCII header
//		else if (line.compare("gridlength") == 0) {headerfile >> i.gridlength;}
//		else if (line.compare("n_nodes") == 0) {headerfile >> i.n_nodes;}
//		else if (line.compare("n_data") == 0) {headerfile >> i.n_data;}
//		else { cout << "  unrecognized keyword [" << line << "], skipping" << endl;
//		char c; do { c = headerfile.get(); } while(headerfile.good() && (c != '\n'));
//		}
//	}
//
//	headerfile.close();
//	return 1;
//}
