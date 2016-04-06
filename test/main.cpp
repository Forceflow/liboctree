#include "../include/OctreeNode.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	OctreeNode n;
	n.data = 0;
	n.children_base = 2;
	n.children = (byte) 146;
	for (int i = 0; i < 8; i++) {
		cout << "Child " << i << " Is there: " << n.hasChild(i) << " Location: " << n.getChildPos(i) << endl;
	}
}