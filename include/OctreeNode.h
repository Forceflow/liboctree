#ifndef OCTREENODE_H_
#define OCTREENODE_H_

#include <cstring>

#define NOCHILDREN 0
#define NODATA 0

typedef unsigned char byte;
static byte BYTE_MASK[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

// An SVO node. Only contains child pointers, extend this if you want parent pointers as well
class OctreeNode
{
public:
	size_t data;
	size_t children_base; // pointer to first child
	byte children;

	OctreeNode();
	bool hasChild(int_fast8_t i) const;
	size_t getChildPos(int_fast8_t i) const;
	bool isLeaf() const;
	bool hasData() const;
	bool isNull() const;
};

// Default constructor
inline OctreeNode::OctreeNode() : data(0), children_base(0) {
	memset(children_offset, (char) NOCHILD, 8);
}

// Check if this Node has a child at position i
inline bool OctreeNode::hasChild(int_fast8_t i) const{
	return !(children_offset[i] == NOCHILD);
}

// Get the full index of the child at position i
inline size_t OctreeNode::getChildPos(int_fast8_t i) const{
	if(children_offset[i] == NOCHILD){
		return 0;
	} else {
		return children_base + children_offset[i];
	}
}

// If this node doesn't have data and is a leaf node, it's a null node
inline bool OctreeNode::isNull() const{
	return isLeaf() && !hasData();
}

// If this node doesn;t have any children, it's a leaf node
inline bool OctreeNode::isLeaf() const{
	if (children == NOCHILDREN){
		return true;
	}
	return false;
}

// If the data pointer is NODATA, there is no data
inline bool OctreeNode::hasData() const{
	return !(data == NODATA);
}

#endif /* OCTREENODE_H_ */
