#pragma once

#include <cstring>
#include <cstdint>

typedef uint8_t byte;

#define NOCHILDREN 0
#define NODATA 0

// Pre-computed masks for checking specific bit
static const byte CHILD_CHECK_MASK[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
// Pre-computed masks for masking upto a specific bit
static const byte CHILD_COUNT_MASK[] = { 1, 3, 7, 15, 31, 63, 127, 255 };

// Countbits LUT using macro by Hallvard Furuseth
static const byte CHILD_BITS_SET[256] =
{
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
	B6(0), B6(1), B6(1), B6(2)
};

class OctreeNode
{
public:
	byte childmask; // a byte indicating which children exist
	uint32_t children_base; // child location
	uint32_t data; // data location

	OctreeNode();
	bool hasChild(int_fast8_t i) const;
	uint32_t getChildPos(int_fast8_t i) const;
	bool isLeaf() const;
	bool hasData() const;
	bool isNull() const;
};

// Default constructor
inline OctreeNode::OctreeNode() : data(0), children_base(0), childmask(0) {
}

// Check if this Node has a child at position i
inline bool OctreeNode::hasChild(int_fast8_t i) const {
	return childmask & CHILD_CHECK_MASK[i];
}

// Get the full index of the child at position i
inline uint32_t OctreeNode::getChildPos(int_fast8_t i) const {
	return !hasChild(i) ? 0 : children_base + CHILD_BITS_SET[childmask & CHILD_COUNT_MASK[i]] - 1;
}

// If this node doesn't have any children, it's a leaf node
inline bool OctreeNode::isLeaf() const {
	return (childmask == NOCHILDREN);
}

// If the data pointer is NODATA, there is no data
inline bool OctreeNode::hasData() const {
	return !(data == NODATA);
}

// If this node doesn't have data and is a leaf node, it's a null node
inline bool OctreeNode::isNull() const {
	return isLeaf() && !hasData();
}
