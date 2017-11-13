#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "OctreeNode.h"

using namespace std;

struct data_descriptor {
	// Base filename
	string name;
	// Associated filestreams
	unsigned int start_byte;
	unsigned int end_byte;
};

// Internal format to interact with an octree file and its associates
struct OctreeFile {
	// Base filename
	string base_filename;
	// Associated filestreams
	fstream file_node;
	fstream file_data;
	// Grid dimensions
	unsigned int grid[3];
	// Nodes info
	size_t node_count;
	size_t node_size;
	// Data info
	size_t data_count;
	size_t data_size;
	// Data descriptors
	vector<data_descriptor> data_descriptors;

	// Read size_data bytes from data file
	size_t readData(byte* data) {
		size_t readpos = file_data.tellg();
		file_data.read((char*) data, data_size);
		return readpos;
	}

	// Read size_data bytes from data file at position
	size_t readData(byte* data, size_t position) {
		file_data.seekg(position);
		file_data.read((char*)data, data_size);
		return position;
	}

	// Read size_node bytes from node file
	size_t readNode(byte* data) {
		size_t readpos = file_node.tellg();
		file_node.read((char*)data, node_size);
		return readpos;
	}

	// Read size_data bytes from address and write to data file
	// Returns position (bytes) where this was written.
	size_t appendData(const byte* data) {
		// Go to end of file
		file_data.seekg(file_data.end);
		size_t writepos = file_data.tellp();
		// Write data
		file_data.write((char*) data, data_size);
		// Cleanup
		data_count++;
		return writepos;
	}

	// Write size_node bytes to node file
	// Returns position (bytes) where this was written.
	size_t appendNode(const OctreeNode& node) {
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
};

inline OctreeFile readOctreeHeader(const string filename) {
	// Open header
	std::cout << "Reading octree header from " << filename.c_str() << " ... " << endl;
	ifstream headerfile;
	headerfile.open(filename.c_str());
	std::string word;

	// Start building OctreeFile struct
	OctreeFile octreefile;
	octreefile.base_filename = filename.substr(0, filename.find_last_of("."));

	// Check that this is an #octree file
	headerfile >> word;
	if (word.compare("#octree") != 0) { cout << "Error: first word reads [" << word.c_str() << "] instead of #octree" << endl; exit(1); }

	bool done = false;
	while (headerfile.good() && !done) {
		headerfile >> word;
		if (word.compare("END") == 0) { done = true; }
		else if (word.compare("node_count") == 0) { headerfile >> octreefile.node_count; }
		else if (word.compare("node_size") == 0) { headerfile >> octreefile.node_size; }
		else if (word.compare("data_count") == 0) { headerfile >> octreefile.data_count; }
		else if (word.compare("data_size") == 0) { headerfile >> octreefile.data_size; }
		else if (word.compare("grid") == 0) {
			headerfile >> octreefile.grid[0];
			headerfile >> octreefile.grid[1];
			headerfile >> octreefile.grid[2];
		}
		else if (word.compare("data_descriptor") == 0) {
			data_descriptor desc;
			headerfile >> desc.name;
			headerfile >> desc.start_byte;
			headerfile >> desc.end_byte;
			octreefile.data_descriptors.push_back(desc);
		}
		else {
			cout << "  unrecognized keyword [" << word << "], skipping" << endl;
			char c; do { c = headerfile.get(); } while (headerfile.good() && (c != '\n'));
		}
	}
}

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
