#pragma once

/*A thread-safe STL*/

#include<thread>
#include<mutex>
#include<algorithm>
#include<memory>
#include<exception>
#include<stack>
#include<deque>
#include<vector>
#include<map>
#include<array>

#define TSL_API __declspec(dllexport)

class empty_stack :public std::exception
{
public:
	empty_stack(void) {}
	empty_stack(const char* const_pStr) :exception{ const_pStr } {}
};


namespace tsl
{

	/* Warning: 
	 *			1.The push() function dose not support a temporary copy of an argument that being
	 *			transfered in. So the copy constructor of the value type should be implemented.
	 *
	 */	
	template<typename T>
	class CTSStack
	{
	private:
		std::stack<T> m_Container;
		mutable std::mutex m_Mutex;
	public:
		CTSStack(void) :m_Container{}, m_Mutex{} {}
		CTSStack(const CTSStack& lrOther)
		{
			std::lock_guard<std::mutex> guard{ lrOther.m_Mutex };
			m_Container = lrOther.m_Container;
		}
		CTSStack& operator=(const CTSStack&) = delete;

		void push(T& Value)
		{
			std::lock_guard<std::mutex> guard{ m_Mutex };
			m_Container.push(Value);
		}
		void push(T&& Value)
		{
			std::lock_guard<std::mutex> guard{ m_Mutex };
			m_Container.push(Value);
		}
		std::shared_ptr<T> pop(void)
		{
			std::lock_guard<std::mutex> guard{ m_Mutex };
			if (m_Container.empty())
			{
				throw empty_stack{ "Stack is empty." };
			}
			std::shared_ptr<T> const Ret{ std::make_shared<T>(m_Container.top()) };
			m_Container.pop();
			return Ret;
		}
		void pop(T& Value)
		{
			std::lock_guard<std::mutex> guard{ m_Mutex };
			if (m_Container.empty())
			{
				throw empty_stack{ "Stack is empty." };
			}
			Value = m_Container.top();
			m_Container.pop();
		}
		bool empty(void)
		{
			std::lock_guard<std::mutex> guard{ m_Mutex };
			return m_Container.empty();
		}
		
		template<typename T>
		friend void swap(CTSStack<T>& lrOne, CTSStack<T>& lrOther);
	};
	template<typename T>
	void swap(CTSStack<T>& lrOne, CTSStack<T>& lrOther)
	{
		if (&lrOne == &lrOther)
		{
			return;
		}
		std::lock(lrOne.m_Mutex, lrOther.m_Mutex);
		std::lock_guard<std::mutex> guard_a{ lrOne.m_Mutex,std::adopt_lock };
		std::lock_guard<std::mutex> guard_b{ lrOther.m_Mutex,std::adopt_lock };
		std::swap(lrOne.m_Container, lrOther.m_Container);
	}
}