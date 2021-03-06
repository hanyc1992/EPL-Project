/*
* Vector_PhaseC_unittests.cpp
* EE380L - Spring 2015
*
* Tests for Vector_PhaseC are organized into three sections: PhaseC, PhaseC1, and
* PhaseC2, corresponding to the C, C*, and C** requirements respectively. These tests
* are independent, so you may comment out the ones that you are not utilizing, and
* Google Test will run accordingly.
*
* These tests are not complete. Write additional tests on your own to test
* the rest of the functionality of your program. The tests used to grade your
* project will be more robust than those included in this file.
*/
#include <list>
#include <cstdint>
#include <stdexcept>
#include "gtest/gtest.h"
#include "Vector.h"
#define PHASE_C

using epl::vector;
using std::begin;
using std::end;

/*****************************************************************************************/
// Phase C Tests
/*****************************************************************************************/
#if defined(PHASE_C0_0) | defined(PHASE_C)
TEST(PhaseC, direct_init_list) {
	vector<int> w{ 0, 1, 2 };
	EXPECT_EQ(3, w.size());
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(i, w[i]);
	}
}
#endif

#if defined(PHASE_C0_1) | defined(PHASE_C)
TEST(PhaseC, begin_vector) {
	vector<int> x{ 1, 2, 3 };
	auto it = begin(x);
	const auto& it2 = end(x);
	EXPECT_EQ(1, *it);
	EXPECT_TRUE(end(x) == it2);
}
#endif

#if defined(PHASE_C0_2) | defined(PHASE_C)
TEST(PhaseC, RandIteOperations) {
	vector<int> x{ 1, 2, 3, 4 };
	vector<int>::const_iterator ite1 = x.begin();
	EXPECT_EQ(*ite1, 1);
	EXPECT_EQ(ite1[3], 4);
	auto ite2(ite1);
	EXPECT_TRUE(ite1 == ite2);
	ite2 = ite2 + 1;
	EXPECT_TRUE(ite1 < ite2);
	ite2 += 1;
	EXPECT_TRUE(ite1 < ite2);
	ite1++; ++ite1;
	EXPECT_TRUE(ite1 == ite2);
	EXPECT_EQ(*ite1, 3);
	--ite1; ite1--;
	EXPECT_EQ(ite2 - ite1, 2);
	ite2 = ite2 - 1;
	ite2 -= 1;
	EXPECT_EQ(ite2 - ite1, 0);
}
#endif

/*****************************************************************************************/
// Phase C* Tests
/*****************************************************************************************/
#if defined(PHASE_C1_0) | defined(PHASE_C)
TEST(PhaseC1, range_for_loop) {
	vector<int32_t> x(10);

	int32_t k = 0;
	for (auto& v : x) {
		v = k++;
	}

	const vector<int32_t>& y = x;
	int32_t s = 0;
	for (const auto& v : y) {
		s += v;
	}

	EXPECT_EQ(45, s);
}
#endif

#if defined(PHASE_C1_1) | defined(PHASE_C)
TEST(PhaseC1, invalid_iterator) {
	vector<uint32_t> x(10);
	auto it = x.begin();
	x.pop_front();
	try {
		*it;
		FAIL();
	}
	catch (epl::invalid_iterator exc) {
		SUCCEED();
	}
	catch (...) {
		FAIL();
	}
}
#endif

#if defined(PHASE_C1_2) | defined(PHASE_C)
TEST(PhaseC1, RandIteConstructor) {
	vector<int> x{ 1, 2, 3, 4 };
	vector<int>::const_iterator ite1 = x.begin();
	vector<int>::const_iterator ite2 = x.begin();
	//vector<int>::iterator ite1 = x.begin();
	//vector<int>::iterator ite2 = x.begin();
	ite1++; ite1++;
	ite2 += 1;

	vector<int> y(ite2, ite1);
	EXPECT_EQ(y[0], 2);
	EXPECT_EQ(y.size(), 1);
	EXPECT_THROW(y[1], std::out_of_range);
}
#endif

/*****************************************************************************************/
// Phase C** Tests
/*****************************************************************************************/
#if defined(PHASE_C2_0) | defined(PHASE_C)
TEST(PhaseC2, ItrExceptSevere) {
	vector<int> x(1);
	auto itr = x.begin();
	x.pop_back();
	try {
		int a;
		a = *itr;
		FAIL();
		*itr = a;
	}
	catch (epl::invalid_iterator ii) {
		EXPECT_EQ(ii.level, epl::invalid_iterator::SEVERE);
	}
	catch (...) {
		FAIL();
	}
}
#endif

#if defined(PHASE_C2_1) | defined(PHASE_C)
TEST(PhaseC2, ItrExceptModerate) {
	vector<int> x(3), y{ 1,2,3 };
	auto xi = x.begin();
	x = y;
	try {
		*xi = 5;
		FAIL();
	}
	catch (epl::invalid_iterator ex) {
		EXPECT_EQ(ex.level, epl::invalid_iterator::MODERATE);
	}
	catch (...) {
		FAIL();
	}
}
#endif

#if defined(PHASE_C2_2) | defined(PHASE_C)
TEST(PhaseC2, ItrExceptMild) {
	vector<int> x(3);
	auto itr = x.begin();
	x.pop_back();
	try {
		volatile int a;
		a = *itr;
		FAIL();
		*itr = a;
	}
	catch (epl::invalid_iterator ii) {
		EXPECT_EQ(ii.level, epl::invalid_iterator::MILD);
	}
	catch (...) {
		FAIL();
	}
}
#endif




/*****************************************************************************************/
// Phase C*** Tests -- My Test1
/*****************************************************************************************/
#if defined(PHASE_C3_0) | defined(PHASE_C)
TEST(PhaseC3, iterator_constructor_vector) {
	vector<int> w{ 0, 1, 2 };
	vector<int> x{ w.begin(), w.end() };
	int i = 0;
	for (auto ptr = x.begin(); ptr!=x.end(); ++ptr, ++i) {
		EXPECT_EQ(i, *ptr);
	}
}
#endif

#if defined(PHASE_C3_1) | defined(PHASE_C)
TEST(PhaseC3, iterator_constructor_pointer) {
	int w[7] = { 0,1,2,3,4,5,6 };
	vector<int> x{ w, w+5 };
	int i = 0;
	for (auto ptr = x.begin(); ptr != x.end(); ++ptr, ++i) {
		EXPECT_EQ(i, *ptr);
	}
}
#endif

#if defined(PHASE_C3_2) | defined(PHASE_C)
TEST(PhaseC3, iterator_constructor_list) {
	std::list<int> w{ 0, 1, 2 ,3,4,5,6 }; 
	vector<int> x{ w.begin(), w.end() };
	//vector<int> x{ w.end(), w.begin() };
	int i = 0;
	for (auto ptr = x.begin(); ptr != x.end(); ++ptr, ++i) {
		EXPECT_EQ(i, *ptr);
	}
}
#endif

/*****************************************************************************************/
// Phase C*** Tests -- My Test2
/*****************************************************************************************/
#if defined(PHASE_C4_0) | defined(PHASE_C)
TEST(PhaseC4, yen_0) {
	vector<int> v{ 1, 2, 3 };
	auto ite = v.end();
	v.pop_back();
	try {
		*ite = 5;
		FAIL();
	}
	catch (epl::invalid_iterator ttt0) {
		EXPECT_EQ(ttt0.level, epl::invalid_iterator::MILD);
	}
	catch (...) {
		FAIL();
	}
}
#endif

#if defined(PHASE_C4_1) | defined(PHASE_C)
TEST(PhaseC4, yen_1) {
	vector<int> v{ 1, 2, 3 };
	auto ite = v.end() - 1;
	v.pop_back();
	try {
		*ite = 5;
		FAIL();
	}
	catch (epl::invalid_iterator ttt1) {
		EXPECT_EQ(ttt1.level, epl::invalid_iterator::SEVERE);
	}
	catch (...) {
		FAIL();
	}
}
#endif

#if defined(PHASE_C4_2) | defined(PHASE_C)
TEST(PhaseC4, yen_2) {
	vector<int> v{ 1, 2, 3 };
	auto ite = v.begin();
	v.pop_front();
	try {
		*ite = 5;
		FAIL();
	}
	catch (epl::invalid_iterator ttt2) {
		EXPECT_EQ(ttt2.level, epl::invalid_iterator::MILD);
	}
	catch (...) {
		FAIL();
	}
}
#endif

#if defined(PHASE_C4_3) | defined(PHASE_C)
TEST(PhaseC4, yen_3) {
	vector<int> v{ 1 }; // only one integer in the vector
	auto ite = v.begin();
	v.pop_front();       // This line can do v.pop_back(). The result is the same.
	try {
		*ite = 5;
		FAIL();
	}
	catch (epl::invalid_iterator ttt3) {
		EXPECT_EQ(ttt3.level, epl::invalid_iterator::SEVERE);
	}
	catch (...) {
		FAIL();
	}
}
#endif