#include "../include/OctreeNode.h"
#include "../include/OctreeFile.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	OctreeFile c("D:/test.octree", true);
	c.readHeader();
}