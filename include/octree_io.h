#ifndef OCTREE_IO_H_
#define OCTREE_IO_H_

#include <cstdio>
#include <tuple>
#include <cstring>
#include <vector>
#include <fstream>
#include "OctreeNode.h"

using namespace std;

// Define types
typedef tuple<string, unsigned int, unsigned int> field_descriptor; // descriptor of data field (name, starting byte, end byte)

// Internal format to interact with an octree file and its associates
struct OctreeInfo {
	int version;
	bool valid;

	// Base filename
	string base_filename;
	// associated files
	fstream file_nodes;
	fstream file_data;
	
	size_t grid_length;

	// nodes info
	size_t n_node;
	size_t size_node;
	
	// data info
	size_t n_data;
	size_t size_data;
	vector<field_descriptor> data_description;

	//// check if all files required exist
	//bool filesExist() const{
	//	string header = base_filename + string(".octree");
	//	string nodes = base_filename + string(".octreenodes");
	//	string data = base_filename + string(".octreedata");
	//	return (file_exists(header) && file_exists(nodes) && file_exists(data));
	//}

	// read size_data bytes from data file
	size_t readData(byte* data) {
		size_t readpos = file_data.tellg();
		file_data.read(data, size_data);
		return readpos;
	}

	// Read size_data bytes from address and write to data file
	// Returns position (bytes) where this was written.
	size_t writeData(const byte* data) {
		file_data.seekg(file_data.end);
		size_t writepos = file_data.tellp();
		file_data.write(data, size_data);
		return writepos;
	}

	// read size_data bytes from data file
	size_t readNode(byte* data) {
		size_t readpos = file_data.tellg();
		file_data.read(data, size_data);
		return readpos;
	}

	// read size_node bytes from address and write to node file
	// Returns position (bytes) where this was written.
	size_t writeNode(const byte* node) {
		file_nodes.seekg(file_nodes.end);
		size_t writepos = file_nodes.tellp();
		file_nodes.write(node, size_node);
		return writepos;
	}




};

size_t writeVoxelData(FILE* f, const VoxelData &v, size_t &b_data_pos);
void readVoxelData(FILE* f, VoxelData &v);
size_t writeNode(FILE* node_out, const Node &n, size_t &b_node_pos);
inline void readNode(FILE* f, Node &n);

void writeOctreeHeader(const std::string &filename, const OctreeInfo &i);
int parseOctreeHeader(const std::string &filename, OctreeInfo &i);

// Write a data point to file
inline size_t writeVoxelData(FILE* f, const VoxelData &v, size_t &b_data_pos){
	fwrite(&v.morton, VOXELDATA_SIZE, 1, f);
	b_data_pos++;
	return b_data_pos-1;
}

// Read a data point from a file
inline void readDataPoint(FILE* f, VoxelData &v){
	v.morton = 0;
	fread(&v.morton, VOXELDATA_SIZE, 1, f);
}

// Write an octree node to file
inline size_t writeNode(FILE* node_out, const Node &n, size_t &b_node_pos){
	fwrite(& n.data, sizeof(size_t), 3, node_out);
	b_node_pos++;
	return b_node_pos-1;
}

// Read a Node from a file
inline void readNode(FILE* f, Node &n){
	fread(& n.data, sizeof(size_t), 3, f);
}

// Write an octree header to a file
inline void writeOctreeHeader(const std::string &filename, const OctreeInfo &i){
	ofstream outfile;
	outfile.open(filename.c_str(), ios::out);
	outfile << "#octreeheader 1" << endl;
	outfile << "gridlength " << i.gridlength << endl;
	outfile << "n_nodes " << i.n_nodes << endl;
	outfile << "n_data " << i.n_data << endl;
	outfile << "END" << endl;
	outfile.close();
}

// Parse a given octree header, store info in OctreeInfo struct
inline int parseOctreeHeader(const std::string &filename, OctreeInfo &i){
	cout << "  reading octree header from " << filename << " ... " << endl;
	ifstream headerfile;
	headerfile.open(filename.c_str(), ios::in);

	i.base_filename = filename.substr(0,filename.find_last_of("."));

	string line; bool done = false;
	headerfile >> line >> i.version;
	if (line.compare("#octreeheader") != 0) {cout << "    Error: first line reads [" << line << "] instead of [#octreeheader]" << endl; return 0;}

	while(headerfile.good() && !done) {
		headerfile >> line;
		if (line.compare("END") == 0) done = true; // when we encounter data keyword, we're at the end of the ASCII header
		else if (line.compare("gridlength") == 0) {headerfile >> i.gridlength;}
		else if (line.compare("n_nodes") == 0) {headerfile >> i.n_nodes;}
		else if (line.compare("n_data") == 0) {headerfile >> i.n_data;}
		else { cout << "  unrecognized keyword [" << line << "], skipping" << endl;
		char c; do { c = headerfile.get(); } while(headerfile.good() && (c != '\n'));
		}
	}

	headerfile.close();
	return 1;
}

#endif