#include "DataFormats/Common/interface/KDTree.h"
#include "DataFormats/Common/interface/KDBox.h"

#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace edm;

int main(){
	//random list of numbers to sort
	vector<float> test = {55,85,11,87,97,90,18,41,4,62,73,2,30,17,56,65,93,19,64,91};
	
	KDTree<float,1> tree;
	tree.build(test);
	
	//print tree
	tree.print_as_sideways_tree();
	
	//range search
	KDBox<1> region;
	region.dimmin[0] = 0;
	region.dimmax[0] = 50;
	vector<float*> found_nodes;
	tree.search(region,found_nodes);
	
	//print results
	for(auto node : found_nodes){
		cout << *node << ", ";
	}
	cout << endl;
	
	//nearest neighbor
	float neighbor = 36;
	const float* result = 0;
	float dist = 0;
	tree.findNearestNeighbor(neighbor,result,dist);
	
	//print results
	cout << "Nearest to " << neighbor << ": " << *result << " (" << dist << " away)" << endl;
}