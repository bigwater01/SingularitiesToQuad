#include <iostream>
#include <fstream>
#include <string>
#include "tmesh.h"
#include "titerators.h"

using namespace std;
using namespace TMeshLib;

int main()
{
	char filename[] = "data/eight.t";
	ifstream is(filename);
	if(is.fail()) {
		cerr << "error in opening file " << string(filename) << endl;
		return -1;
	}
	CTMesh * tmesh = new CTMesh();
	tmesh->_load_t(filename);
	
	cout << "#tets = " << tmesh->numTets() << endl;
	
	cout << "traverse all edges: " << endl;
	for (TMeshEdgeIterator eit(tmesh); !eit.end(); ++eit)
	{
		CEdge * e = *eit;
		cout << "(" << e->vertex1()->id() << ", " << e->vertex2()->id() << ")" << endl;
	}
	cout << "done" << endl;

	return 0;
}