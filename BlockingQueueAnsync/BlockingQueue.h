#pragma once
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
template<typename T>
class BlockingQueue
{
public:
	BlockingQueue(size_t capacity);
	void atomicEnque(T data);
	T atomicDeque();
	T atomicPeek();
	size_t size();
	size_t capacity();
	bool isEmpty();
	bool isFull();
private:
	T* m_DataArray;
	size_t m_size;
	size_t m_capacity;
	int m_front;
	int m_back;
	mutable std::mutex m_mutex;
	std::condition_variable queueIsFull;
	std::condition_variable queueIsEmpty;
};

template<typename T>
BlockingQueue<T>::BlockingQueue(size_t capacity)
	:m_capacity(capacity)
	, m_size(0)
	, m_front(0)
	, m_back(-1)
{
	m_DataArray = new T[capacity];
}

template<typename T>
void BlockingQueue<T>::atomicEnque(T data)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	queueIsFull.wait(lock, [this] {return m_size != m_capacity; });
	m_back = (m_back + 1) % m_capacity;
	++m_size;
	m_DataArray[m_back] = data;
	std::cout << data << '\n';
	queueIsEmpty.notify_one();
}

template<typename T>
T BlockingQueue<T>::atomicDeque()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	queueIsEmpty.wait(lock, [this] {return m_size > 0; });
	T ReturnValue = m_DataArray[m_front];
	m_front = (m_front + 1) % m_capacity;
	--m_size;
	std::cout << "dequed" << ReturnValue << '\n';
	queueIsFull.notify_one();
	return ReturnValue;
}

template<typename T>
T BlockingQueue<T>::atomicPeek()
{
	const std::lock_guard<std::mutex> lock(m_mutex);
	T ReturnValue = m_DataArray[m_front];
	return ReturnValue;
}

template<typename T>
size_t BlockingQueue<T>::size()
{
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_size;
}

template<typename T>
size_t BlockingQueue<T>::capacity()
{
	return m_capacity;
}

template<typename T>
bool BlockingQueue<T>::isEmpty()
{
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_size == 0;
}

template<typename T>
bool BlockingQueue<T>::isFull()
{
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_size == m_capacity;
}

