#include "../include/OctreeNode.h"
#include "../include/OctreeFile.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	OctreeFile c(string("D:/test.octree"), true);
	std::fstream file;
	file.open("D:/test.jeroen", std::ios::out | std::ios::binary);
	file.seekg(400);
	byte jeroen = 1;
	file.write((char*)&jeroen, 1);
	file.close();
	c.readHeader();
}