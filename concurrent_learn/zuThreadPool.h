#pragma once
#include<queue>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<functional>
#include<future>

// std::function
// std::invoke_result

class zuThreadPool
{
public:
	zuThreadPool(size_t);
	~zuThreadPool();

	template<class F, class... Args>
	void enqueue(F&& f, Args&&... args);

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex mtx;
	std::condition_variable cv;
	bool stop;
};

template<class F, class ...Args>
void zuThreadPool::enqueue(F&& f, Args && ...args)
{
	std::function<void()> task(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
	{
		std::unique_lock<std::mutex> lock(mtx);
		tasks.push(std::move(task));
	}
}
