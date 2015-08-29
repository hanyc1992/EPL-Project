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




/**********************************************choose_type<int, double>::type is double************************************************/
template <typename T>
struct rank;
template <> struct rank<int> { static constexpr int value = 1; };
template <> struct rank<float> { static constexpr int value = 2; };
template <> struct rank<double> { static constexpr int value = 3; };

template <int R>
struct stype;
template <> struct stype<1> { using type = int; };
template <> struct stype<2> { using type = float; };
template <> struct stype<3> { using type = double; };

template <typename T1, typename T2>
struct choose_type {
	static constexpr int t1_rank = rank<T1>::value;
	static constexpr int t2_rank = rank<T2>::value;
	static constexpr int max_rank = (t1_rank < t2_rank) ? t2_rank : t1_rank;

	using type = typename stype<max_rank>::type;
};
/**********************************************choose_type<int, double>::type is double************************************************/


/*****************************************************valarray class begin***********************************************************/
template<typename T>
class valarray : public vector<T> {
public:
	using value_type = T;

	valarray() :vector<T>() {};
	valarray(uint64_t n) :vector<T>(n) {};
	valarray(std::initializer_list<T> args) :vector<T>(args) {};

	//T& apply(int i) { return operator[](i); }
	const value_type& apply(int i) const { return operator[](i); }

};
/*****************************************************valarray class end***********************************************************/

/*****************************************************binary operation begin******************************************************/
template <typename LHS, typename RHS>
struct AddOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return x + y;
	}
};

template <typename LHS, typename RHS>
struct SubOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return x - y;
	}
};

template <typename LHS, typename RHS>
struct MulOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return x * y;
	}
};

template <typename LHS, typename RHS>
struct DivOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return x / y;
	}
};


template <typename T>
struct NegOp{
	using type = T;
	static inline type apply(const T& t) {
		return -t;
	}
};
/*****************************************************binary operation end******************************************************/

/******************************************************** proxy begin***********************************************************/
template <typename OP, typename LHS, typename RHS>
class BinaryOp {
private:
	const LHS& lhs;
	const RHS& rhs;
public:
	using value_type = typename choose_type<typename LHS::value_type, typename RHS::value_type>::type;

	BinaryOp(OP op, const LHS& lhs, const RHS& rhs) :lhs(lhs), rhs(rhs) {};
	~BinaryOp() {};

	inline value_type apply(int i) const {
		return OP::apply(lhs.apply(i), rhs.apply(i));
	}

	uint64_t size()const {
		uint64_t lsize = lhs.size(); uint64_t rsize = rhs.size();
		return std::min(lsize, rsize);
	};

	operator valarray<value_type>() {
		valarray<value_type> result(size());
		for (uint64_t i = 0; i < size(); ++i) {
			result[i] = apply(i);
		}
		return result;
	}
};

template <typename OP, typename T>
class UnaryOp {
private:
	const T& t;
public:
	using value_type = typename T::value_type;

	UnaryOp(OP op, const T& t) :t(t) {};
	~UnaryOp() {};

	inline value_type apply(int i) const {
		return OP::apply(t.apply(i));
	}

	uint64_t size()const {
		return t.size();
	}

	operator valarray<value_type>() {
		valarray<value_type> result(size());
		for (uint64_t i = 0; i < size(); ++i) {
			result[i] = apply(i);
		}
		return result;
	}
};
/********************************************************proxy end***********************************************************/

/********************************************************operator overload begin*************************************************************/
template <typename LHS, typename RHS>
auto operator+(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	AddOp<value_type_L, value_type_R> addop{};
	return BinaryOp<AddOp<value_type_L, value_type_R>, LHS, RHS>{addop, lhs, rhs};
}

template <typename LHS, typename RHS>
auto operator-(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	SubOp<value_type_L, value_type_R> subop{};
	return BinaryOp<SubOp<value_type_L, value_type_R>, LHS, RHS>{subop, lhs, rhs};
}

template <typename LHS, typename RHS>
auto operator*(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	MulOp<value_type_L, value_type_R> mulop{};
	return BinaryOp<MulOp<value_type_L, value_type_R>, LHS, RHS>{mulop, lhs, rhs};
}

template <typename LHS, typename RHS>
auto operator/(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	DivOp<value_type_L, value_type_R> divop{};
	return BinaryOp<DivOp<value_type_L, value_type_R>, LHS, RHS>{divop, lhs, rhs};
}


template <typename T>
auto operator-(const T& t) {
	using value_type = typename T::value_type;
	NegOp<value_type> negop{};
	return UnaryOp<NegOp<value_type>, T>{negop, t};
}
/********************************************************operator overload end*************************************************************/
























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

