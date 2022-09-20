#pragma once
#include "TaskQueue.hpp"
#include <iostream>
#include <vector>
template <typename T>
class ThreadPool
{
public:
	ThreadPool(int min,int max);
	~ThreadPool();
	void addTask(Task<T> newTsk);
	void shut();
private:
	static void manager_thread(ThreadPool<T>* mt);
	static void worker_thread(ThreadPool<T>* mt);
private:
	bool shutdown;
	int maxNum;
	int minNum;
	int busyNum;
	int liveNum;
	int sleepNum;
	const static int NUMBER = 2;
	std::thread* m_manager;
	std::mutex p_tex;
	std::condition_variable cv;
	std::condition_variable cv_mgr;
	std::vector<std::thread*> m_worker;
	TaskQueue<T> tskQ;
};
template <typename T>
ThreadPool<T>::ThreadPool(int min,int max):shutdown(false),maxNum(max),minNum(min),busyNum(0),sleepNum(0)
{
	m_manager = new std::thread(manager_thread, this);
	for (size_t i = 0; i < min; i++)
	{
		m_worker.push_back(new std::thread(worker_thread, this));
	}
	liveNum = min;
	busyNum = min;
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
	m_manager->join();
	delete m_manager;//释放内存
	for (size_t i = 0; i < liveNum; i++)
	{
		m_worker[i]->join();
		delete m_worker[i];//释放内存
	}

}

template<typename T>
void ThreadPool<T>::addTask(Task<T> newTsk)
{
	tskQ.addTask(newTsk);
	cv_mgr.notify_one();
}

template<typename T>
inline void ThreadPool<T>::shut()
{
	shutdown = true;
	cv_mgr.notify_one();//通知管理者
}

template<typename T>
void ThreadPool<T>::manager_thread(ThreadPool<T>* mt)
{
	while (!mt->shutdown)
	{
		//std::this_thread::sleep_for(std::chrono::seconds(3));
		if ( mt->tskQ.getTskNum() == 0||mt->busyNum==mt->maxNum)//无任务等待
		{
			std::unique_lock<std::mutex> lck(mt->p_tex);
			mt->cv_mgr.wait(lck);
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		if (mt->liveNum < mt->maxNum&&mt->tskQ.getTskNum()!=0) {//查看工作线程是否不足
			for (size_t i = 0; i < NUMBER&&mt->busyNum<mt->maxNum; i++)
			{
				mt->m_worker.push_back(new std::thread(worker_thread, mt));//添加新的工作线程
				std::lock_guard<std::mutex> lk(mt->p_tex);
				++mt->busyNum;
				++mt->liveNum;//存活线程
				std::cout << "push new Task" << std::endl;
			}
		}
		if (mt->sleepNum!=0&&mt->tskQ.getTskNum() != 0)//有任务，唤醒线程
		{
			std::lock_guard<std::mutex> lk(mt->p_tex);
			--mt->sleepNum;
			++mt->busyNum;
			mt->cv.notify_one();//唤醒一个等待线程
		}

	}
	std::cout << "shutdown" << std::endl;
	mt->cv.notify_all();//关机，唤醒等待线程；
}

template<typename T>
void ThreadPool<T>::worker_thread(ThreadPool<T>* mt)
{
	while (!mt->shutdown)
	{
		if (!mt->shutdown&&mt->tskQ.getTskNum() == 0) {//无任务，线程等待
			std::unique_lock<std::mutex> lck(mt->p_tex);
			std::cout << "thread waiting" << std::endl;
			++mt->sleepNum;//等待线程+
			--mt->busyNum;//工作线程-
			mt->cv.wait(lck);//阻塞等待
		}
		if (mt->tskQ.getTskNum() != 0)//有任务，执行任务
		{
			std::unique_lock<std::mutex> lck(mt->p_tex);
			if (mt->tskQ.getTskNum() != 0)//有任务，执行任务
			{
				Task<T> tsk = mt->tskQ.getTask();//获取任务
				lck.unlock();
				tsk.run();//执行任务
			}
		}
	}
}
