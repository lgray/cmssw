#include "DataFormats/Common/interface/KDBox.h"

namespace edm {
	template<unsigned Dim>
	KDBox<Dim>::KDBox() {}

	template<unsigned Dim>
	template<typename... Ts>
	KDBox<Dim>::KDBox(Ts... dimargs) {
		static_assert(sizeof...(dimargs) == 2 * Dim, "KDBox: Constructor requires 2*Dim args");
		std::vector<float> dims = {dimargs...};

		for (unsigned i = 0; i < Dim; ++i){
			dimmin[i] = dims[2 * i];
			dimmax[i] = dims[2 * i + 1];
		}
	}
	
	template<unsigned Dim>
	KDBox<Dim>::~KDBox() {}
	
}

template class edm::KDBox<1>;
