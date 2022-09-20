#pragma once
#include <queue>
#include <mutex>
using callback = void(*)(int);//void(*)(int)类型函数指针
template <typename T>
class Task//任务类
{
public:
	Task(callback fun,T ar) {
		fuc= fun;
		arg = ar;
	}
	~Task() {

	}
	void run() {//执行任务
		fuc(arg);
	}

private:
		callback fuc;
		T arg;
};
template <typename T>//任务队列
class TaskQueue
{
public:
	TaskQueue();
	~TaskQueue();
	void addTask(Task<T> tk);//添加任务
	inline int getTskNum();//获取剩余任务数量
	Task<T> getTask();//获取任务
private:
	std::queue<Task<T>> tsk_queue;//任务队列
	std::mutex tex;//互斥锁
	
};

template <typename T>
TaskQueue<T>::TaskQueue()
{
}

template <typename T>
TaskQueue<T>::~TaskQueue()
{
}

template<typename T>
int  TaskQueue<T>::getTskNum() {
	std::lock_guard<std::mutex> lck(tex);
	return tsk_queue.size();
}

template<typename T>
void TaskQueue<T>::addTask(Task<T> tk)
{
	std::lock_guard<std::mutex> lck(tex);
	tsk_queue.push(tk);
}

template<typename T>
Task<T> TaskQueue<T>::getTask()
{
	std::lock_guard<std::mutex> lck(tex);
	if (!tsk_queue.empty())
	{
		Task<T> f_tk = tsk_queue.front();
		tsk_queue.pop();
		return f_tk;
	}
	
}

