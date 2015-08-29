#include <iostream>
#include <vector>
#include "Valarray.h"


using std::cout;
using std::endl;

//using namespace epl;

int main(){
	valarray<int> v(3);
	cout << v<<endl;
	valarray<int> v1{ 1, 2, 3 };
	valarray<double> v2{ 4.1,5.2,6.3,7.4,8.5 };
	auto v3 = v2 + v2;
	v2 = v1;
	cout << v3 << endl;

	//valarray<std::complex<float>> aa{ std::complex<float>(1.0,1.3),std::complex<float>(1.0,1.0) ,std::complex<float>(1.0,1.0) ,std::complex<float>(1.0,1.0) ,std::complex<float>(1.0,1.0) };
	//valarray<double> aaa{ 3.2, 2, 6.2, 9, 12 };
	valarray<int> a{ 1,2,3,4 };
	//valarray<std::complex<double>> empty;

	//aa = aaa;
	//cout << aa << endl;
	////valarray<std::complex<int>> c_int{ std::complex<int>(1,1),std::complex<int>(1,1),std::complex<int>(1,1),std::complex<int>(1,1),std::complex<int>(1,1) };
	//auto aaaa = (aa + aaa * a);

	///***********************************************/
	//auto p = (aa + aaa * a).begin();
	//auto q = (aa + aaa * a).end();
	//while (p != q) {
	//	cout << *p << endl;
	//	++p;
	//}
	//cout << endl << endl << endl;
	///***********************************************/
	//valarray<int> x(10);
	//valarray<double> y(10);
	//for (auto const & p : x + y) {
	//	cout << p;
	//}
	//cout << endl << endl << endl;
	///***********************************************/
	//cout << aa + aaa * a << endl << endl;
	///************************************************/
	//for (auto ptr = aaaa.begin(); ptr != aaaa.end(); ++ptr)
	//	cout << *ptr << " ";
	//cout << endl;
	///************************************************/
	//for (const auto & p : -(aaa + aa)) {
	//	cout << p;
	//}
	///***********************************************/

	//cout.precision(12);
	//cout << endl << aaaa.accumulate(std::multiplies<std::complex<double>>{}) << endl;
	//cout << endl << empty.sum() << endl;



	////This example taken from http://en.cppreference.com/w/cpp/numeric/valarray/apply to check apply() works correctly
	////valarray<int> v = { 1,2,3,4,5,6,7,8,9,10 };
	////v = v.apply([](int n)->int {
	////	return std::round(std::tgamma(n + 1));
	////});
	////for (auto n : v) {
	////	std::cout << n << ' ';
	////}
	////std::cout << '\n';

	cout << "hello world!"<<endl;
	cout << (a + a).apply(std::negate<int>{}) << endl;
	cout << ((a + a).sqrt()) << endl;
	cout << a.sqrt() << endl;
	getchar();

}