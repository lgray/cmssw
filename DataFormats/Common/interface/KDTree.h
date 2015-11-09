#ifndef EDM_KDTREE_H
#define EDM_KDTREE_H

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
	//forward declarations
	template<unsigned Dim> class KDBox;
	
	template <class Type, unsigned Dim>
	class KDTree {
		public:
			//default constructor
			KDTree();
			//destructor
			~KDTree();
			
			//build KDTree from element list
			void build(std::vector<Type>& eltList);
			//range search
			void search(const KDBox<Dim>& searchBox, std::vector<Type*>& resList);
			//nearest neighbor search
			void findNearestNeighbor(const Type& point, const Type*& result, float& distance);
			
			//accessors
			bool empty();
			int size();
	
			//debugging
			void print_as_sideways_tree();
	
		private:
			//member variables
			int root;
			std::vector<Type*> data;
			std::vector<KDBox<Dim> > boxes;
			std::vector<Type*> *closestNeighbors;
			
			//navigator functions
			int median(int low, int high);
			
			//helper functions
			KDBox<Dim> fillAndBound(std::vector<Type>& eltList);
			int medianSearch(int low, int high, unsigned depth);
			int recBuild(int low, int high, unsigned depth, const KDBox<Dim>& region);
			void recSearch(int low, int high, const KDBox<Dim>& searchBox);
			void addSubtree(int low, int high);
			void recNearestNeighbor(int low, int high, unsigned depth, const Type& point, const Type*& best_match, float& best_dist);
			void clear();
			void print_as_sideways_tree(int low, int high, unsigned depth);
	};
}

#endif