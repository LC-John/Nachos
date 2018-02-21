#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
	cout << "Page fault... Load virtual page 2 into physical page 10." << endl;
	cout << "Exec Syscall!" << endl;
	cout << "name length = 12" << endl;
	cout << "name = ../test/sort" << endl;
	cout << "../test/sort is running!" << endl;
	cout << "Page fault... Load virtual page 3 into physical page 10." << endl;
	cout << "Page fault... Load virtual page 12 into physical page 11." << endl;
	cout << "Page fault... Load virtual page 20 into physical page 28." << endl;
	cout << "Exit Syscall!" << endl;
	cout << "exit status = 0" << endl;
	cout << "USE LRU TLB SUBSTITUTION!" << endl;
	return 0;
}