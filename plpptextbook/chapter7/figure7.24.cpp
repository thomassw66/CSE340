#include <iostream>

using namespace std;

typedef struct { int i; double d; } IntDouble;

bool operator < (IntDouble x, IntDouble y) {
	return x.i < y.i && x.d < y.d;
}

IntDouble operator + (IntDouble x, IntDouble y) {
	IntDouble z;
	z.i = x.i + y.i;
	z.d = x.d + y.d;
	return z;
}

int main () {
	IntDouble x = {1, 2.1}, y = {5, 3.4};
	if (x < y) x = x + y;
	else y = x + y;
	cout << x.i << " " << x.d << endl;
	return 0;
}