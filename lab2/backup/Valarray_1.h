// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h

#include <cstdint>
#include <algorithm>

using std::vector; // during development and testing
// using epl::vector; // after submission

template<typename T>
class valarray : public vector<T> {
public:
	using value_type = T;

	valarray() :vector<T>() {};
	valarray(uint64_t n) :vector<T>(n) {};
	valarray(std::initializer_list<T> args) :vector<T>(args) {};

	//T& apply(int i) { return operator[](i); }
	const T& apply(int i) const { return operator[](i); }

};

template <typename addtype>
struct AddOp{
	static inline addtype apply(const addtype & x, const addtype & y) {
		return x + y;
	}
};


template <typename OP, typename LHS, typename RHS>
class BinaryOp {
private:
	const LHS& lhs;
	const RHS& rhs;
public:
	BinaryOp(OP op, const LHS& lhs, const RHS& rhs) :lhs(lhs), rhs(rhs) {};
	~BinaryOp() {};
	inline double apply(int i) const{//////////////////////////////////////////return type & valarray<double>::apply
		return OP::apply(lhs.apply(i), rhs.apply(i));
	}

	uint64_t size()const { 
		uint64_t lsize = lhs.size(); uint64_t rsize = rhs.size();
		return std::min(lsize, rsize);
	};

	template <typename T>
	operator valarray<T>() {
		valarray<T> result(size());
		for (uint64_t i = 0; i < size(); ++i) {
			result[i] = apply(i);
		}
		return result;
	}
};


template <typename LHS, typename RHS>
BinaryOp<AddOp<double>, LHS, RHS> operator+(const LHS& lhs, const RHS& rhs) {
	AddOp<double> addop{};//////////////////////////////////////////////////////////////////////////type
	return BinaryOp<AddOp<double>, LHS, RHS>{addop, lhs, rhs};
}

























//template<typename F, typename G>
//struct AddProxy {
//	//F f;
//	//G g;
//	//AddProxy(F f, G g) :f(f), g(g) {};
//	//valarray<double> operator valarray<double>() { return f() + g(); }
//};
//
//template<typename TT>
//struct AddProxy<valarray<TT>, valarray<TT>> {
//	valarray<TT> f;
//	valarray<TT> g;
//	uint64_t size_num = std::min(f.size(), g.size());
//	AddProxy(valarray<TT> f, valarray<TT> g) : f(f), g(g) {};
//
//	uint64_t size(void) { return size_num };
//
//	operator valarray<TT>() {
//		valarray<TT> result(size_num);
//		auto ptr_f = f.begin();
//		auto ptr_g = g.begin();
//		for (auto ptr = result.begin(); ptr != result.end(); ++ptr) {
//			*ptr = *ptr_f + *ptr_g;
//			++ptr_f; ++ptr_g;
//		}
//		return result;
//	}
//};
//
//template <typename F, typename G>
//AddProxy<F, G> operator+(const F& f, const G& g) {
//	return AddProxy<F, G>{f, g};
//}


#endif /* _Valarray_h */

