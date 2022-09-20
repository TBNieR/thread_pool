#pragma once
#include <queue>
#include <mutex>
using callback = void(*)(int);//void(*)(int)���ͺ���ָ��
template <typename T>
class Task//������
{
public:
	Task(callback fun,T ar) {
		fuc= fun;
		arg = ar;
	}
	~Task() {

	}
	void run() {//ִ������
		fuc(arg);
	}

private:
		callback fuc;
		T arg;
};
template <typename T>//�������
class TaskQueue
{
public:
	TaskQueue();
	~TaskQueue();
	void addTask(Task<T> tk);//�������
	inline int getTskNum();//��ȡʣ����������
	Task<T> getTask();//��ȡ����
private:
	std::queue<Task<T>> tsk_queue;//�������
	std::mutex tex;//������
	
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

