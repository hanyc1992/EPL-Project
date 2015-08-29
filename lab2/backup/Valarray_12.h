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
#include <functional>
#include <cmath>
#include <complex>
#include <limits.h>
#include <type_traits>

using std::vector; // during development and testing
				   // using epl::vector; // after submission

template <typename T>
class valarray;

/****************************************************apply proxy begin**********************************************************/
//template<typename T>
//class valarray;
//
//template <typename OP, typename Valarray_Type>
//class APPLY_PROXY {
//private:
//	OP op;
//	const Valarray_Type& array;
//public:
//	//using result_type = OP::result_type;
//	APPLY_PROXY(OP op, const Valarray_Type& array) :op(op), array(array) {};
//	operator valarray<typename Valarray_Type::value_type>() {
//		valarray<Valarray_Type::value_type> result(array.size());
//		for (uint64_t i = 0; i < array.size(); ++i) {
//			result[i] = op(array[i]);
//		}
//		return result;
//	}
//};
/****************************************************apply proxy end**********************************************************/

/***********************************************type choose --- to ref begin***************************************************/
template <typename T>
struct to_ref { using type = const T; };

template <typename T>
struct to_ref<valarray<T>> { using type = const valarray<T>&; };

template <typename T>
using Ref = typename to_ref<T>::type;
/***********************************************type choose --- to ref end***************************************************/

/**********************************choose_type<int, double>::type is double & complex added************************************/
template <typename T>
struct rank;
template <> struct rank<int> { static constexpr int value = 1; };
template <> struct rank<float> { static constexpr int value = 2; };
template <> struct rank<double> { static constexpr int value = 3; };
template <typename T> struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };

template <int R>
struct stype;
template <> struct stype<1> { using type = int; };
template <> struct stype<2> { using type = float; };
template <> struct stype<3> { using type = double; };

template <typename T> struct is_complex : public std::false_type {};
template <typename T> struct is_complex<std::complex<T>> : public std::true_type{};

template <bool p, typename T> struct ctype;
template <typename T> struct ctype<true, T> { using type = std::complex<T>; };
template <typename T> struct ctype<false, T> { using type = T; };

template <typename T1, typename T2>
struct choose_type {
	static constexpr int t1_rank = rank<T1>::value;
	static constexpr int t2_rank = rank<T2>::value;
	static constexpr int max_rank = (t1_rank < t2_rank) ? t2_rank : t1_rank;

	using my_stype = typename stype<max_rank>::type;

	static constexpr bool t1_comp = is_complex<T1>::value;
	static constexpr bool t2_comp = is_complex<T2>::value;
	static constexpr  bool my_comp = t1_comp || t2_comp;

	using type = typename ctype<my_comp, my_stype>::type;
};
/**********************************choose_type<int, double>::type is double & complex added************************************/

/*****************************************************valarray class begin***********************************************************/
template<typename T>
class valarray : public vector<T> {
public:
	using value_type = T;

	valarray() :vector<T>() {};
	valarray(uint64_t n) :vector<T>(n) {};
	valarray(std::initializer_list<T> args) :vector<T>(args) {};

	template <typename FUNC>
	value_type accumulate(FUNC func) {
		if (size()) {
			auto s = (*this)[0];
			for (uint64_t i = 1; i < size(); ++i)
				s = func(s, (*this)[i]);
			return s;
		}
		else
			return value_type{};
	}

	value_type sum() {
		return accumulate(std::plus<value_type>{});
	}

//	const value_type& apply(int i) const { return operator[](i); }

	//template <typename FUNC>
	//auto apply(FUNC func) {
	//	return APPLY_PROXY<FUNC, valarray<T>>{func, *this};
	//}
};
/*****************************************************valarray class end***********************************************************/

/*****************************************************class Scalar begin*********************************************************/
template<typename T>
class Scalar {
private:
	T s;
public:
	using value_type = T;
	Scalar(T v) :s(v) {};
	T operator[](uint64_t i)const { return s; };
	uint64_t size() const { return INT_MAX; };//int_max is not accurate but is OK
};
/*****************************************************class Scalar end*********************************************************/

/***************************************************** operation begin******************************************************/
template <typename LHS, typename RHS>
struct AddOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return apply_sub(x, y, is_complex<LHS>(), is_complex<RHS>());
	}

	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::false_type) {
		return x + y;
	}////primitive + primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::true_type) {
		return type{ x + y.real(), y.imag() };
	}////primitive + complex
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::false_type) {
		return type{ x.real() + y, x.imag() };
	}////complex + primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::true_type) {
		return type{ x.real() + y.real(), x.imag() + y.imag() };
	}////complex + complex
};

template <typename LHS, typename RHS>
struct SubOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return apply_sub(x, y, is_complex<LHS>(), is_complex<RHS>());
	}

	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::false_type) {
		return x - y;
	}////primitive - primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::true_type) {
		return type{ x - y.real(), -y.imag() };
	}////primitive - complex
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::false_type) {
		return type{ x.real() - y, x.imag() };
	}////complex - primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::true_type) {
		return type{ x.real() - y.real(), x.imag() - y.imag() };
	}////complex - complex
};

template <typename LHS, typename RHS>
struct MulOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return apply_sub(x, y, is_complex<LHS>(), is_complex<RHS>());
	}

	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::false_type) {
		return x * y;
	}////primitive * primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::true_type) {
		return type{ x * y.real(), x * y.imag() };
	}////primitive * complex
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::false_type) {
		return type{ x.real() * y, x.imag() * y };
	}////complex * primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::true_type) {
		return type{ x.real() * y.real() - x.imag() * y.imag(), x.imag() * y.real() + x.real() * y.imag() };
	}////complex * complex
};

template <typename LHS, typename RHS>
struct DivOp {
	using type = typename choose_type<LHS, RHS>::type;
	static inline type apply(const LHS & x, const RHS & y) {
		return apply_sub(x, y, is_complex<LHS>(), is_complex<RHS>());
	}

	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::false_type) {
		return x / y;
	}////primitive / primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::false_type, std::true_type) {
		return type{ x*y.real() / (y.real()*y.real() + y.imag()*y.imag()), -x*y.imag() / (y.real()*y.real() + y.imag()*y.imag()) };
	}////primitive / complex
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::false_type) {
		return type{ x.real() / y, x.imag() / y };
	}////complex / primitive
	static inline type apply_sub(const LHS & x, const RHS & y, std::true_type, std::true_type) {
		return type{ (x.real()*y.real() + x.imag()*y.imag()) / (y.real()*y.real() + y.imag()*y.imag()), (x.imag()*y.real() - x.real()*y.imag()) / (y.real()*y.real() + y.imag()*y.imag()) };
	}////complex / complex
};

template <typename T>
struct NegOp {
	using type = T;
	static inline type apply(const T& t) {
		return -t;
	}
};
/*if using the following instead, there will be error in complex<float> + int*/
//template <typename LHS, typename RHS>
//struct AddOp {
//	using type = typename choose_type<LHS, RHS>::type;
//	static inline type apply(const LHS & x, const RHS & y) {
//		return type{ x } + type{ y };
//	}
//};
//
//template <typename LHS, typename RHS>
//struct SubOp {
//	using type = typename choose_type<LHS, RHS>::type;
//	static inline type apply(const LHS & x, const RHS & y) {
//		return type{ x } - type{ y };
//	}
//};
//
//template <typename LHS, typename RHS>
//struct MulOp {
//	using type = typename choose_type<LHS, RHS>::type;
//	static inline type apply(const LHS & x, const RHS & y) {
//		return type{ x } * type{ y };
//	}
//};
//
//template <typename LHS, typename RHS>
//struct DivOp {
//	using type = typename choose_type<LHS, RHS>::type;
//	static inline type apply(const LHS & x, const RHS & y) {
//		return type{ x } / type{ y };
//	}
//};
/***************************************************** operation end******************************************************/

/******************************************************** proxy begin***********************************************************/
template <typename OP, typename LHS, typename RHS>
class BinaryOp {
private:
	Ref<LHS> lhs;
	Ref<RHS> rhs;
public:
	using value_type = typename choose_type<typename LHS::value_type, typename RHS::value_type>::type;

	BinaryOp(OP op, const LHS& lhs, const RHS& rhs) :lhs(lhs), rhs(rhs) {};
	~BinaryOp() {};

	value_type operator[](uint64_t i) const {
		return OP::apply(lhs[i], rhs[i]);
	}

	BinaryOp& operator=(const BinaryOp& that) {
		lhs = that.lhs; rhs = that.rhs;
		return *this;
	}

	uint64_t size() const {
		uint64_t lsize = lhs.size(); uint64_t rsize = rhs.size();
		return std::min(lsize, rsize);
	};

	template <typename T>
	operator valarray<T>() const {
		valarray<T> result(size());
		for (uint64_t i = 0; i < size(); ++i) {
			result[i] = operator[](i);
		}
		return result;
	};

	template <typename FUNC>
	value_type accumulate(FUNC func) {
		if (size()) {
			auto s = (*this)[0];
			for (uint64_t i = 1; i < size(); ++i)
				s = func(s, (*this)[i]);
			return s;
		}
		else
			return value_type{};
	};

	value_type sum() {
		return accumulate(std::plus<value_type>{});
	};

	class iterator : public std::iterator<std::forward_iterator_tag, value_type> {
	private:
		const BinaryOp<OP, LHS, RHS>* obj;
		uint64_t pos;
	public:
		iterator(const BinaryOp<OP, LHS, RHS>* obj, uint64_t pos) :obj(obj), pos(pos) {};
		iterator& operator++() { ++pos; return *this; };
		value_type operator*() { return (*obj).operator[](pos); };
		bool operator==(const iterator& that) { return pos == that.pos; };
		bool operator!=(const iterator& that) { return pos != that.pos; };
	};
	iterator begin(void) { return iterator(this, 0); };
	iterator end(void) { return iterator(this, size()); };
};

template <typename OP, typename T>
class UnaryOp {
private:
	Ref<T> t;
public:
	using value_type = typename T::value_type;

	UnaryOp(OP op, const T& t) :t(t) {};
	~UnaryOp() {};

	value_type operator[](uint64_t i) const {
		return OP::apply(t[i]);
	}

	UnaryOp& operator=(const UnaryOp& that) {
		t = that.t;
		return *this;
	}

	uint64_t size() const {
		return t.size();
	}

	template <typename TT>
	operator valarray<TT>() const {
		valarray<TT> result(size());
		for (uint64_t i = 0; i < size(); ++i) {
			result[i] = operator[](i);
		}
		return result;
	}

	template <typename FUNC>
	value_type accumulate(FUNC func) {
		if (size()) {
			auto s = (*this)[0];
			for (uint64_t i = 1; i < size(); ++i)
				s = func(s, (*this)[i]);
			return s;
		}
		else
			return value_type{};
	}

	value_type sum() {
		return accumulate(std::plus<value_type>{});
	}

	class iterator : public std::iterator<std::forward_iterator_tag, value_type> {
	private:
		const UnaryOp<OP, T>* obj;
		uint64_t pos;
	public:
		iterator(const UnaryOp<OP, T>* obj, uint64_t pos) :obj(obj), pos(pos) {};
		iterator& operator++() { ++pos; return *this; };
		value_type operator*() { return (*obj).operator[](pos); };
		bool operator==(const iterator& that) { return pos == that.pos; };
		bool operator!=(const iterator& that) { return pos != that.pos; };
	};
	iterator begin(void) { return iterator(this, 0); };
	iterator end(void) { return iterator(this, size()); };
};
/********************************************************proxy end***********************************************************/

/********************************************************operator overload begin****************************************************/
/*check if a type is BinaryOp/UnaryOp/valarray ***** can use is_BUv<T>::value */
template <typename T>
struct is_buv :public std::false_type {};

template <typename T>
struct is_buv<valarray<T>> :public std::true_type{};

template <typename OP, typename LHS, typename RHS>
struct is_buv<BinaryOp<OP, LHS, RHS>> :public std::true_type{};

template <typename OP, typename T>
struct is_buv<UnaryOp<OP, T>> :public std::true_type{};
/*check if a type is BinaryOp/UnaryOp/valarray ***** can use is_BUv<T>::value */

/******************************add begin***********************************/
template <typename LHS, typename RHS>///////buv + buv
typename std::enable_if<is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<AddOp<typename LHS::value_type, typename RHS::value_type>, LHS, RHS>>::type operator+(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	AddOp<value_type_L, value_type_R> addop{};
	return BinaryOp<AddOp<value_type_L, value_type_R>, LHS, RHS>{addop, lhs, rhs};
}
template <typename LHS, typename RHS>///////scalar + buv
typename std::enable_if<!is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<AddOp<LHS, typename RHS::value_type>, Scalar<LHS>, RHS>>::type operator+(const LHS& lhs, const RHS& rhs) {
	using value_type_L = LHS;
	using value_type_R = typename RHS::value_type;
	AddOp<value_type_L, value_type_R> addop{};
	return BinaryOp<AddOp<value_type_L, value_type_R>, Scalar<LHS>, RHS>{addop, Scalar<LHS>{lhs}, rhs};
}
template <typename LHS, typename RHS>///////buv + scalar
typename std::enable_if<is_buv<LHS>::value && !is_buv<RHS>::value, BinaryOp<AddOp<typename LHS::value_type, RHS>, LHS, Scalar<RHS>>>::type operator+(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = RHS;
	AddOp<value_type_L, value_type_R> addop{};
	return BinaryOp<AddOp<value_type_L, value_type_R>, LHS, Scalar<RHS>>{addop, lhs, Scalar<RHS>{rhs}};
}
/******************************sub begin***********************************/
template <typename LHS, typename RHS>///////buv - buv
typename std::enable_if<is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<SubOp<typename LHS::value_type, typename RHS::value_type>, LHS, RHS>>::type operator-(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	SubOp<value_type_L, value_type_R> subop{};
	return BinaryOp<SubOp<value_type_L, value_type_R>, LHS, RHS>{subop, lhs, rhs};
}
template <typename LHS, typename RHS>///////scalar - buv
typename std::enable_if<!is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<SubOp<LHS, typename RHS::value_type>, Scalar<LHS>, RHS>>::type operator-(const LHS& lhs, const RHS& rhs) {
	using value_type_L = LHS;
	using value_type_R = typename RHS::value_type;
	SubOp<value_type_L, value_type_R> subop{};
	return BinaryOp<SubOp<value_type_L, value_type_R>, Scalar<LHS>, RHS>{subop, Scalar<LHS>{lhs}, rhs};
}
template <typename LHS, typename RHS>///////buv - scalar
typename std::enable_if<is_buv<LHS>::value && !is_buv<RHS>::value, BinaryOp<SubOp<typename LHS::value_type, RHS>, LHS, Scalar<RHS>>>::type operator-(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = RHS;
	SubOp<value_type_L, value_type_R> subop{};
	return BinaryOp<SubOp<value_type_L, value_type_R>, LHS, Scalar<RHS>>{subop, lhs, Scalar<RHS>{rhs}};
}
/******************************mul begin***********************************/
template <typename LHS, typename RHS>///////buv * buv
typename std::enable_if<is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<MulOp<typename LHS::value_type, typename RHS::value_type>, LHS, RHS>>::type operator*(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	MulOp<value_type_L, value_type_R> mulop{};
	return BinaryOp<MulOp<value_type_L, value_type_R>, LHS, RHS>{mulop, lhs, rhs};
}
template <typename LHS, typename RHS>///////scalar * buv
typename std::enable_if<!is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<MulOp<LHS, typename RHS::value_type>, Scalar<LHS>, RHS>>::type operator*(const LHS& lhs, const RHS& rhs) {
	using value_type_L = LHS;
	using value_type_R = typename RHS::value_type;
	MulOp<value_type_L, value_type_R> mulop{};
	return BinaryOp<MulOp<value_type_L, value_type_R>, Scalar<LHS>, RHS>{mulop, Scalar<LHS>{lhs}, rhs};
}
template <typename LHS, typename RHS>///////buv * scalar
typename std::enable_if<is_buv<LHS>::value && !is_buv<RHS>::value, BinaryOp<MulOp<typename LHS::value_type, RHS>, LHS, Scalar<RHS>>>::type operator*(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = RHS;
	MulOp<value_type_L, value_type_R> mulop{};
	return BinaryOp<MulOp<value_type_L, value_type_R>, LHS, Scalar<RHS>>{mulop, lhs, Scalar<RHS>{rhs}};
}
/******************************div begin***********************************/
template <typename LHS, typename RHS>///////buv / buv
typename std::enable_if<is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<DivOp<typename LHS::value_type, typename RHS::value_type>, LHS, RHS>>::type operator/(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = typename RHS::value_type;
	DivOp<value_type_L, value_type_R> divop{};
	return BinaryOp<DivOp<value_type_L, value_type_R>, LHS, RHS>{divop, lhs, rhs};
}
template <typename LHS, typename RHS>///////scalar / buv
typename std::enable_if<!is_buv<LHS>::value && is_buv<RHS>::value, BinaryOp<DivOp<LHS, typename RHS::value_type>, Scalar<LHS>, RHS>>::type operator/(const LHS& lhs, const RHS& rhs) {
	using value_type_L = LHS;
	using value_type_R = typename RHS::value_type;
	DivOp<value_type_L, value_type_R> divop{};
	return BinaryOp<DivOp<value_type_L, value_type_R>, Scalar<LHS>, RHS>{divop, Scalar<LHS>{lhs}, rhs};
}
template <typename LHS, typename RHS>///////buv / scalar
typename std::enable_if<is_buv<LHS>::value && !is_buv<RHS>::value, BinaryOp<DivOp<typename LHS::value_type, RHS>, LHS, Scalar<RHS>>>::type operator/(const LHS& lhs, const RHS& rhs) {
	using value_type_L = typename LHS::value_type;
	using value_type_R = RHS;
	DivOp<value_type_L, value_type_R> divop{};
	return BinaryOp<DivOp<value_type_L, value_type_R>, LHS, Scalar<RHS>>{divop, lhs, Scalar<RHS>{rhs}};
}
/******************************neg begin***********************************/
template <typename T>
typename std::enable_if<is_buv<T>::value, UnaryOp<NegOp<typename T::value_type>, T>>::type operator-(const T& t) {
	using value_type = typename T::value_type;
	NegOp<value_type> negop{};
	return UnaryOp<NegOp<value_type>, T>{negop, t};
}
/********************************************************operator overload end**************************************************/

struct IdentityFunction {
	double operator()(double x) const { return x; }
};


#endif /* _Valarray_h */

