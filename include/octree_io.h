#ifndef OCTREE_IO_H_
#define OCTREE_IO_H_

#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <fstream>
#include "OctreeNode.h"

using namespace std;

// Define types
typedef tuple<string, unsigned int, unsigned int> field_descriptor; // descriptor of data field (name, starting byte, end byte)

// Internal format to interact with an octree file and its associates
struct OctreeFile {
	// Base filename
	string base_filename;

	// Associated filestreams
	fstream file_nodes;
	fstream file_data;
	
	size_t grid_length;

	// Nodes info
	size_t n_node;
	size_t size_node;
	
	// Data info
	size_t n_data;
	size_t size_data;
	vector<field_descriptor> data_description;

	// read size_data bytes from data file
	size_t readData(byte* data) {
		size_t readpos = file_data.tellg();
		file_data.read((char*) data, size_data);
		return readpos;
	}

	// Read size_data bytes from address and write to data file
	// Returns position (bytes) where this was written.
	size_t writeData(const byte* data) {
		file_data.seekg(file_data.end);
		size_t writepos = file_data.tellp();
		file_data.write((char*) data, size_data);
		return writepos;
	}

	// read size_node bytes from data file
	size_t readNode(byte* data) {
		size_t readpos = file_data.tellg();
		file_data.read((char*) data, size_data);
		return readpos;
	}

	// read size_node bytes from address and write to node file
	// Returns position (bytes) where this was written.
	size_t writeNode(const byte* node) {
		file_nodes.seekg(file_nodes.end);
		size_t writepos = file_nodes.tellp();
		file_nodes.write((char*) node, size_node);
		return writepos;
	}
};

inline OctreeFile readOctreeFile(const string filename) {
	std::cout << "  reading octree header from " << filename.c_str() << " ... " << endl;
	ifstream headerfile;
	headerfile.open(filename.c_str());

	OctreeFile octreefile;
	octreefile.base_filename = filename.substr(0, filename.find_last_of("."));

	std::string line; 
	bool done = false;

	headerfile >> line;
	if (line.compare("#octree") != 0) { cout << "    Error: first line reads [" << line.c_str() << "] instead of [#octree]" << endl; exit(1); }
	octreefile.version = (headerfile.get());


	//while (headerfile.good() && !done) {
	//	headerfile >> line;
	//	if (line.compare("END") == 0) done = true; // when we encounter data keyword, we're at the end of the ASCII header
	//	else if (line.compare("gridlength") == 0) { headerfile >> i.gridlength; }
	//	else if (line.compare("n_nodes") == 0) { headerfile >> i.n_nodes; }
	//	else if (line.compare("n_data") == 0) { headerfile >> i.n_data; }
	//	else {
	//		cout << "  unrecognized keyword [" << line << "], skipping" << endl;
	//		char c; do { c = headerfile.get(); } while (headerfile.good() && (c != '\n'));
	//	}
	//}
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

#endif