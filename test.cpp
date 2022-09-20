#include "ThreadPool.hpp"
#include <iostream>
using namespace std;
void f(int a) {
	cout << a << endl;
	this_thread::sleep_for(chrono::seconds(6));
}


int main() {
	int a = 10;
	ThreadPool<int> pool(3,5);
	for (size_t i = 0; i < 10; i++)
	{
		pool.addTask(Task<int>(f,i+100));
	}
	this_thread::sleep_for(chrono::seconds(15));
	for (size_t i = 0; i < 10; i++)
	{
		pool.addTask(Task<int>(f, i + 1000));
	}
	this_thread::sleep_for(chrono::seconds(25));
	for (size_t i = 0; i < 10; i++)
	{
		pool.addTask(Task<int>(f, i + 10000));
	}
	this_thread::sleep_for(chrono::seconds(25));
	pool.shut();
	return 0;
}