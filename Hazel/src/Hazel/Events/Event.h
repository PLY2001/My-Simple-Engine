#pragma once

#include "Hazel/Core.h"
#include "hzpch.h"

namespace Hazel {

	// Events in Hazel are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.

	enum class EventType//单个事件类型
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory//事件种类，使用位移能让一个事件属于多个种类。
	{
		None = 0,//0000 0000
		EventCategoryApplication = BIT(0),//0000 0001
		EventCategoryInput = BIT(1),//0000 0010
		EventCategoryKeyboard = BIT(2),//0000 0100
		EventCategoryMouse = BIT(3),//0000 1000
		EventCategoryMouseButton = BIT(4)//0001 0000
	};

//宏定义，可以减少重复编写量
//指定事件类型									//GetStaticType()获取当前的事件类型编号
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
//指定事件种类
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class HAZEL_API Event//事件基类
	{
	public:
		bool Handled = false;
		//都是虚函数，所以具体实现方法要看派生类
		virtual EventType GetEventType() const = 0;//获取事件类型编号
		virtual const char* GetName() const = 0;//获取事件类型名
		virtual int GetCategoryFlags() const = 0;//获取事件种类名
		virtual std::string ToString() const { return GetName(); }//获取事件类型名（以及属性），用于调试信息

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;//与运算来判断是否在事件种类中
		}
	
	};

	class EventDispatcher//事件调度器
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;//类型别名，用EventFn代替std::function<bool(T&)>
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)//针对某输入函数进行调度，输入参量为某函数的引用
		{
			if (m_Event.GetEventType() == T::GetStaticType())//判断是否
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)//输出重载，能够输出Event对象实例的事件类型名（以及其属性）
	{
		return os << e.ToString();
	}
}