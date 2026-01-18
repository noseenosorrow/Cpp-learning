#include "zuThreadPool.h"

zuThreadPool::zuThreadPool(size_t size) :stop(false)
{
	for (size_t i = 0; i < size; i++) {
		workers.emplace_back([this]() {
			while (true) {
				std::unique_lock<std::mutex> lock(this->mtx);

				// release mtx and wait task
				cv.wait(lock, [this]() { return this->stop || !this->tasks.empty(); });

				// terminate thread
				if(stop && this->tasks.empty())
					return;

				std::function<void()> task(std::move(tasks.front()));
				tasks.pop();
				lock.unlock();
				task();
			}
		});
	}
}

zuThreadPool::~zuThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(mtx);
		stop = true;
	}

	cv.notify_all();

	for (auto& thd : workers) {
		thd.join();
	}
}
