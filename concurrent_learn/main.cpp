#include<iostream>
#include<thread>
#include<queue>
#include<condition_variable>
#include<mutex>


std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;

void producer(int i) {
	for (;;) {
		std::unique_lock<std::mutex> lock(mtx);
		if (q.size() < 4) {
			q.push(i);
			std::cout << "Produced: " << i << "\tremain:" << q.size() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			cv.notify_one();
		}
	}
}

void consumer() {
	for (;;) {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [] { return !q.empty(); });
		int item = q.front();
		q.pop();
		std::cout << "Consumed: " << item << "\tremain:" << q.size() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(600));
	}
}


#include"zuThreadPool.h"

int main() {
	zuThreadPool pool(4);
	for (int i = 0; i < 8; i++) {
		pool.enqueue([i]() {
			std::cout << "Task " << i << " is running in thread " << std::this_thread::get_id() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			std::cout << "Task " << i << " is finished in thread " << std::this_thread::get_id() << std::endl;
			});
	}
	return 0;
}