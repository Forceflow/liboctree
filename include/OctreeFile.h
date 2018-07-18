#pragma once

// STL includes
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
// Custom includes
#include "OctreeNode.h"
#include "OctreeDataDescriptor.h"

const std::string OCTREE_FILE_HEADER_EXTENSION = ".octree";
const std::string OCTREE_FILE_NODE_EXTENSION = ".octreenodes";
const std::string OCTREE_FILE_DATA_EXTENSION = ".octreedata";

// A class to interact with an Octree file and its various file streams
class OctreeFile {
public:
	// Base filename
	std::string base_filename_;
	// Associated filestreams (C++ version)
	std::fstream file_header_;
	std::fstream file_node_;
	std::fstream file_data_;
	// Grid dimensions
	unsigned int grid_[3];
	// Nodes info
	size_t node_count_;
	// Data info
	size_t data_count_;
	size_t data_size_;
	// Data descriptors
	std::vector<OctreeDataDescriptor> data_descriptors_;

	// constructor
	inline OctreeFile::OctreeFile(std::string &file_location, bool creation) :
		base_filename_(file_location.substr(0, file_location.find_last_of("."))), node_count_(0), data_count_(0), data_size_(0), grid_{ 0,0,0 } {
		openFilestreams();
	}

	// DATA READ
	// Read size_data bytes from data file, returns position was just read
	inline size_t OctreeFile::readData(byte* data) {
		size_t readpos = file_data_.tellg();
		file_data_.read((char*)data, data_size_);
		return readpos;
	}

	// DATA READ TARGETED
	// Read size_data bytes from data file at position
	inline void OctreeFile::readData(byte* data, const size_t position) {
		file_data_.seekg(position);
		file_data_.read((char*)data, data_size_);
		return;
	}

	// DATA WRITE
	// Read size_data bytes from address and write to data file
	// Returns position (bytes) where this was written.
	inline size_t OctreeFile::appendData(const byte* data) {
		// Go to end of file
		file_data_.seekg(file_data_.end);
		size_t writepos = file_data_.tellp();
		// Write data
		file_data_.write((char*)data, data_size_);
		// Cleanup
		data_count_++;
		return writepos;
	}

	// DATA WRITE TARGETED
	// Read size_data bytes from address and write to data file at position
	inline void OctreeFile::writeData(const byte* data, const size_t position) {
		// Go to end of file
		file_data_.seekg(position);
		// Write data
		file_data_.write((char*)data, data_size_);
	}

	// NODE READ
	// Read size_node bytes from node file
	// Returns position that was just read
	inline size_t OctreeFile::readNode(OctreeNode* node) {
		size_t readpos = file_node_.tellg();
		file_node_.read((char*) &(node->childmask_), OctreeNode::getSize());
		return readpos;
	}

	// NODE READ TARGETED
	// Read size_node bytes from node file, at position
	inline void OctreeFile::readNode(OctreeNode* node, const size_t position) {
		file_node_.seekg(position);
		file_node_.read((char*) &(node->childmask_), OctreeNode::getSize());
		return;
	}

	// NODE WRITE
	// Write size_node bytes to node file
	// Returns position (bytes) where this was written.
	inline size_t OctreeFile::appendNode(const OctreeNode* node) {
		// Go to end of file
		file_node_.seekg(file_node_.end);
		size_t writepos = file_node_.tellp();
		// Write Node data (TODO: Can probably do this in one write)
		file_node_.write((char*) &(node->childmask_), sizeof(node->childmask_));
		file_node_.write((char*) &(node->children_base_), sizeof(node->children_base_));
		file_node_.write((char*) &(node->data_), sizeof(node->data_));
		// Cleanup
		node_count_++;
		return writepos;
	}

	// NODE WRITE TARGETED
	// Write size_node bytes to node file at specific position
	inline void OctreeFile::writeNode(const OctreeNode* node, const size_t position) {
		// Go to specific position
		file_node_.seekg(position);
		// Write Node data (TODO: Can probably do this in one write)
		file_node_.write((char*) &(node->childmask_), sizeof(node->childmask_));
		file_node_.write((char*) &(node->children_base_), sizeof(node->children_base_));
		file_node_.write((char*) &(node->data_), sizeof(node->data_));
		// Cleanup
		node_count_++;
		return;
	}

	// READ DATA FROM OCTREE HEADER
	inline bool OctreeFile::readHeader() {
		std::string word;
		// Check that this is an #octree file
		file_header_ >> word;
		if (word.compare("#octree") != 0) { std::cout << "Error: first word of .octree file reads [" << word.c_str() << "] instead of #octree" << std::endl; exit(1); }
		// Read header and save data
		bool done = false;
		while (file_header_.good() && !done) {
			file_header_ >> word;
			if (word.compare("END") == 0) { done = true; }
			else if (word.compare("node_count") == 0) { file_header_ >> this->node_count_; }
			else if (word.compare("data_count") == 0) { file_header_ >> this->data_count_; }
			else if (word.compare("data_size") == 0) { file_header_ >> this->data_size_; }
			else if (word.compare("grid") == 0) {
				file_header_ >> grid_[0];
				file_header_ >> grid_[1];
				file_header_ >> grid_[2];
			}
			else if (word.compare("data_descriptor") == 0) {
				OctreeDataDescriptor desc;
				file_header_ >> desc.data_name;
				file_header_ >> desc.start_byte;
				file_header_ >> desc.end_byte;
				this->data_descriptors_.push_back(desc);
			}
			else {
				std::cout << "  unrecognized keyword [" << word << "], skipping" << std::endl;
				char c; do { c = file_header_.get(); } while (file_header_.good() && (c != '\n'));
			}
		}
	}

	// WRITE OCTREE HEADER
	inline bool OctreeFile::writeHeader() {
		openFileStream(file_header_, OCTREE_FILE_HEADER_EXTENSION, std::ios::out | std::ios::trunc); // open file in trunc mode
		const char SPACE[2] = " ";
		file_header_ << "#octree" << std::endl;
		file_header_ << "node_count" << SPACE <<  
		// TODO: write rest of header
	}

	inline void OctreeFile::openFilestreams() {
		openFileStream(file_header_, OCTREE_FILE_HEADER_EXTENSION, std::ios::in | std::ios::out); // Text-based I/O
		openFileStream(file_node_, OCTREE_FILE_NODE_EXTENSION, std::ios::in | std::ios::out | std::ios::binary); // Binary I/O
		openFileStream(file_node_, OCTREE_FILE_DATA_EXTENSION, std::ios::in | std::ios::out | std::ios::binary); // Binary I/O
	}

	inline void OctreeFile::flushFilestreams() {
		file_header_.flush();
		file_node_.flush();
		file_data_.flush();
	}

	inline void OctreeFile::closeFilestreams() {
		file_header_.close();
		file_node_.close();
		file_data_.close();
	}

	// Flush and close file streams before this object goes out of scope
	// Rest of the members (string/vector) will destruct themselves on object life end
	inline void OctreeFile::~OctreeFile() {
		flushFilestreams();
		closeFilestreams();
	}

private:
	inline std::fstream openFileStream(std::fstream &stream, const std::string &extension, std::ios::ios_base::openmode mode) {
		std::string filename = base_filename_ + extension;
		stream.open(filename.c_str(), mode);
	}
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
