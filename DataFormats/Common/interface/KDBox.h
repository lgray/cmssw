#ifndef EDM_KDBOX_H
#define EDM_KDBOX_H

#include <vector>
#include <array>

namespace edm {
	template<unsigned Dim>
	class KDBox {
		public:
			//constructors
			KDBox();
			template<typename... Ts>
			KDBox(Ts... dimargs);
			
			//destructor
			~KDBox();

			std::array<float, Dim> dimmin; 
			std::array<float, Dim> dimmax; 
	};
}

#endif