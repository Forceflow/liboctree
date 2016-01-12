#ifndef OCTREENODE_H_
#define OCTREENODE_H_

#include <cstring>

#define NOCHILDREN 0
#define NODATA 0

typedef char byte;


// We only need 4 bits to depict child offsets, which range from 0-7

// Byte which depicts no children in c1 and c2
// 1111 1111
// ^-c1 ^-c2

// An SVO node. Only contains child pointers, extend this if you want parent pointers as well
class OctreeNode
{
public:
	size_t children_base; // pointer to first child
	char children;

	byte data; // pointer first child

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
	if (memcmp(children_offset, LEAF, 8 * sizeof(char)) == 0){
		return true;
	}
	return false;
}

// If the data pointer is NODATA, there is no data
inline bool OctreeNode::hasData() const{
	return !(data == NODATA);
}

#endif /* OCTREENODE_H_ */
