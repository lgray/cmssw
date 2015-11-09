#include "DataFormats/Common/interface/KDTree.h"
#include "DataFormats/Common/interface/KDMetric.h"
#include "DataFormats/Common/interface/KDBox.h"
#include <vector>
#include <array>
#include <utility>
#include <cmath>
#include <limits>
#include <iostream>

/**************************************************************************
Design for zero-overhead kd-trees from the Templatized Geometry Library:
Copyright (c) 2003-2005 Oliver Kreylos
http://idav.ucdavis.edu/~okreylos/ResDev/NearestNeighbors/index.html
Available under the GNU General Public License
***************************************************************************/

namespace edm {
	template <class Type, unsigned Dim>
	KDTree<Type,Dim>::KDTree() : root(-1) {}
	
	template <class Type, unsigned Dim>
	KDTree<Type,Dim>::~KDTree() { clear(); }
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::build(std::vector<Type>& eltList){
		if (eltList.size()){
			//build the KDTree recursively
			//first filling data vector and getting initial bounding box
			//(storing the index of the root of the tree)
			root = recBuild(0, data.size()-1, 0, fillAndBound(eltList));
		}
	}

	template <class Type, unsigned Dim>
	int KDTree<Type,Dim>::median(int low, int high){
		return (high - low)/2 + low;
	}
	
	template <class Type, unsigned Dim>
	KDBox<Dim> KDTree<Type,Dim>::fillAndBound(std::vector<Type>& eltList){
		//reserve space in data vector
		data.reserve(eltList.size());
		boxes.resize(eltList.size());
		
		//make box to store bounds
		KDBox<Dim> box;
		unsigned ctr = 0;
		
		//loop over element list
		for(auto& elt : eltList){
			//store address of original element
			data.push_back(&elt);
			
			//update bounds with this element
			if(ctr==0){
				for(unsigned i = 0; i < Dim; ++i){
					box.dimmin[i] = box.dimmax[i] = KDMetric::length<Type,Dim>(&elt,i);
				}
			}
			else{
				for(unsigned i = 0; i < Dim; ++i){
					box.dimmin[i] = std::min(KDMetric::length<Type,Dim>(&elt,i),box.dimmin[i]);
					box.dimmax[i] = std::max(KDMetric::length<Type,Dim>(&elt,i),box.dimmax[i]);
				}
			}
			
			++ctr;
		}
		
		//put the box where it needs to go in order to start building the tree
		boxes[median(0,data.size()-1)] = box;
		return box;
	}
	
	template <class Type, unsigned Dim>
	int KDTree<Type,Dim>::medianSearch(int low, int high, unsigned depth){
		//calculate the index of this node
		int mid = median(low,high);
		const unsigned thedim = depth % Dim;

		int l = low;
		int m = high;

		//sort data array around median
		while (l < m){
			float eltlen = KDMetric::length<Type,Dim>(data[mid],thedim);
			int i = l;
			int j = m;

			do{
				while (KDMetric::length<Type,Dim>(data[i],thedim) < eltlen) ++i;
				while (KDMetric::length<Type,Dim>(data[j],thedim) > eltlen) --j;

				if (i <= j){
					std::swap(data[i],data[j]);
					i++;
					j--;
				}
			}
			while (i <= j);

			if (j < mid) l = i;
			if (i > mid) m = j;
		}

		return mid;
	}
	
	template <class Type, unsigned Dim>
	int KDTree<Type,Dim>::recBuild(int low, int high, unsigned depth, const KDBox<Dim>& region){
		if(high < low) {
			return low;
		}
		else if (high==low){
			//nothing to sort
			boxes[low] = region;
			return low;
		}
		else{
			//find median of specified subset
			int medianId = medianSearch(low, high, depth);
			boxes[medianId] = region;

			//split the current plane into 2 halfplanes
			KDBox<Dim> leftRegion = boxes[medianId];
			KDBox<Dim> rightRegion = boxes[medianId];

			const unsigned thedim = depth % Dim;
			float medianVal = KDMetric::length<Type,Dim>(data[medianId],thedim);
			leftRegion.dimmax[thedim] = medianVal;
			rightRegion.dimmin[thedim] = medianVal;

			//recursively build the son nodes
			//(leaving the median node in its place)
			++depth;
			recBuild(low, medianId-1, depth, leftRegion);
			recBuild(medianId+1, high, depth, rightRegion);
			
			return medianId;
		}
	}
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::search(const KDBox<Dim>& searchBox, std::vector<Type*>& resList){
		if(root>-1){
			closestNeighbors = &resList;
			recSearch(0, data.size()-1, searchBox);
			closestNeighbors = nullptr;
		}
	}
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::recSearch(int low, int high, const KDBox<Dim>& searchBox){
		if (high<low){
			return;
		}
		else if (high==low){
			//leaf case
			//check if point inside the rectangle/area
			bool isInside = true;

			for (unsigned i = 0; i < Dim; ++i){
				const auto thedim = KDMetric::length<Type,Dim>(data[low],i);
				isInside *= thedim >= searchBox.dimmin[i] && thedim <= searchBox.dimmax[i];
			}

			if (isInside) closestNeighbors->push_back(data[low]);
		}
		else{
			//node case
			//calculate the index of this node
			int mid = median(low,high);
			
			//check if node region is fully contained in the rectangle
			bool isFullyContained = true;
			bool hasIntersection = true;

			for (unsigned i = 0; i < Dim; ++i){
				//std::cout << "node " << *(data[mid]) << ": region = " << boxes[mid].dimmin[i] << ", " << boxes[mid].dimmax[i] << std::endl;
				const auto regionmin = boxes[mid].dimmin[i];
				const auto regionmax = boxes[mid].dimmax[i];
				isFullyContained *= (regionmin >= searchBox.dimmin[i] && regionmax <= searchBox.dimmax[i]);
				hasIntersection *= (regionmin < searchBox.dimmax[i] && regionmax > searchBox.dimmin[i]);
			}

			if (isFullyContained){
				addSubtree(low, high);
			}
			else if (hasIntersection){
				//search the subtrees and the current node
				recSearch(low, mid-1, searchBox);
				recSearch(mid, mid, searchBox);
				recSearch(mid+1, high, searchBox);
			}
		}
	}
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::addSubtree(int low, int high){
		if (high<low){
			return;
		}
		else{
			//add this node
			int mid = median(low,high);
			addSubtree(low,mid-1);
			closestNeighbors->push_back(data[mid]);
			addSubtree(mid+1,high);
		}
	}
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::findNearestNeighbor(const Type& point, const Type*& result, float& distance){
		//ensure correct default values
		if( nullptr != result || distance != std::numeric_limits<float>::max() ) {
			result = nullptr;
			distance = std::numeric_limits<float>::max();
		}
		if (root > -1) {
			const Type* best_match = nullptr;
			recNearestNeighbor(0, data.size()-1, 0, point, best_match, distance);
			if( distance != std::numeric_limits<float>::max() ) {
				result = best_match;
				distance = std::sqrt(distance);
			}
		}
	}

	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::recNearestNeighbor(int low, int high, unsigned depth, const Type& point, const Type*& best_match, float& best_dist){
		if(high < low) return;
		
		//calculate the index of this node
		int mid = median(low,high);
		
		const unsigned thedim = depth % Dim;
		const float dist_current = KDMetric::dist2<Type,Dim>(&point,data[mid]);
		const float dist_to_axis = KDMetric::dist_to_axis<Type,Dim>(&point,data[mid],thedim);
	
		//compare to this node and see if it's a better match
		//if it is, update the result
		if(dist_current < best_dist){
			best_dist = dist_current;
			best_match = data[mid];
		}
		
		if( dist_to_axis < 0.f ) 
			recNearestNeighbor(low,mid-1,depth+1,point,best_match,best_dist);
		else 
			recNearestNeighbor(mid+1,high,depth+1,point,best_match,best_dist);
	
		//now check if the radius to best crosses the splitting axis     
		if( best_dist > dist_to_axis*dist_to_axis ) {
			//if it does, traverse the other side of the axis to check for a new best
			if( dist_to_axis < 0.f ) 
				recNearestNeighbor(mid+1,high,depth+1,point,best_match,best_dist);
			else
				recNearestNeighbor(low,mid-1,depth+1,point,best_match,best_dist);
		}
	}
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::clear(){
		data.clear();
		std::vector<Type*>().swap(data);
		boxes.clear();
		std::vector<KDBox<Dim> >().swap(boxes);
	}
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::print_as_sideways_tree(){
		print_as_sideways_tree(0,data.size()-1,0);
	}
	
	template <class Type, unsigned Dim>
	void KDTree<Type,Dim>::print_as_sideways_tree(int low, int high, unsigned depth) {
		if(high<low) return;
		int mid = median(low,high);
		print_as_sideways_tree(mid+1, high, depth+1);
		for (unsigned i=0; i<depth; ++i) std::cout << "    ";
		std::cout << *(data[mid]) << std::endl;
		//for (unsigned i=0; i<depth; ++i) std::cout << "    " << "        ";
		//std::cout << *(data[mid]) << ": [" << boxes[mid].dimmin[0] << "," << boxes[mid].dimmax[0] << "]" << std::endl;
		print_as_sideways_tree(low, mid-1, depth+1);
	}
}

template class edm::KDTree<float,1>;
