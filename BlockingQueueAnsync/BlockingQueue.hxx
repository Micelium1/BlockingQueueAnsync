#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include "BlockingQueue.h"
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
	queueIsFull.wait(lock, [this] {return true; });
	m_back = (m_back + 1) % m_capacity;
	++m_size;
	m_DataArray[m_back] = data;
}

template<typename T>
T BlockingQueue<T>::atomicDeque()
{
	const std::lock_guard<std::mutex> lock(m_mutex);
	T ReturnValue = m_DataArray[m_front];
	m_front = (m_front + 1) / m_capacity;
	--m_size;
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