#include <iostream>

using namespace std;

int foo() {
	int x;
	*(&x+1) = 1; // BOOM
	*(&x+2) = 1;
	*(&x+3) = 1;
	return 0;
}

int main() {
	foo();
}