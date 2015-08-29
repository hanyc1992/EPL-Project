#include <iostream>
#include <vector>
#include "Valarray.h"


using std::cout;
using std::endl;

int main(){
	valarray<double> aa{ 1,1,1,1,1 };
	valarray<double> aaa{ 3.2, 2, 6.2, 9, 12 };
	valarray<double> a{ 2,2,2,2,2 };

	valarray<double> aaaa = aa + aaa +a;

	for (auto ptr = aaaa.begin(); ptr != aaaa.end(); ++ptr)
		cout << *ptr << " ";
	cout << endl;


	cout << "hello world!";
	getchar();

}