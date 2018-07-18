#include "../include/OctreeNode.h"
#include "../include/OctreeFile.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	OctreeFile c(string("D:/test.octree"), true);
	c.readHeader();
}