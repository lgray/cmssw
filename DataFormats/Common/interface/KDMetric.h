#ifndef EDM_KDMETRIC_H
#define EDM_KDMETRIC_H

#include "FWCore/Utilities/interface/EDMException.h"
#include <vector>
#include <array>

namespace edm {
	namespace KDMetric {
		//implementation of length accessor
		//to be specialized for desired Type
		template <class Type>
		float length_impl(const Type* t1, unsigned thedim){
			Exception::throwThis(errors::UnimplementedFeature, "KDMetric: length access not implemented\n");
			return 0;
		}

		//length accessor for any dimension
		template <class Type, unsigned Dim>
		float length(const Type* t1, unsigned thedim){
			return length_impl<Type>(t1, thedim);
		}
		
		//distance functions
		template <class Type, unsigned Dim>
		float dist_to_axis(const Type* t1, const Type* t2, unsigned thedim){
			return length<Type,Dim>(t1,thedim) - length<Type,Dim>(t2,thedim);
		}
		template <class Type, unsigned Dim>
		float dist2(const Type* t1, const Type* t2){
			float d = 0.;

			for (unsigned i = 0 ; i < Dim; ++i){
				float diff = dist_to_axis<Type,Dim>(t1,t2,i);
				d += diff * diff;
			}

			return d;
		}
	}
}

/*
//example of specialization of metric:
template<>
float edm::KDMetric::length_impl<PFRecHit>(const PFRecHit* t1, unsigned thedim){
	if(thedim==0) return t1->x();
	else if(thedim==1) return t1->y();
	else if(thedim==2) return t1->z();
	else {
		Exception::throwThis(errors::UnimplementedFeature, "KDMetric: length access not implemented\n");
		return 0;
	}
}
*/

namespace edm{
	namespace KDMetric{
		template<>
		float length_impl<float>(const float* t1, unsigned thedim){ 
			if(thedim==0) return *t1;
			else {
				Exception::throwThis(errors::UnimplementedFeature, "KDMetric: length access not implemented\n");
				return 0;
			}
		}
	}
}

#endif