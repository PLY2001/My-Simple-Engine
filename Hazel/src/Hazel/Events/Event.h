#pragma once

#include "Hazel/Core.h"
#include "hzpch.h"

namespace Hazel {

	// Events in Hazel are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.

	enum class EventType//�����¼�����
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory//�¼����࣬ʹ��λ������һ���¼����ڶ�����ࡣ
	{
		None = 0,//0000 0000
		EventCategoryApplication = BIT(0),//0000 0001
		EventCategoryInput = BIT(1),//0000 0010
		EventCategoryKeyboard = BIT(2),//0000 0100
		EventCategoryMouse = BIT(3),//0000 1000
		EventCategoryMouseButton = BIT(4)//0001 0000
	};

//�궨�壬���Լ����ظ���д��
//ָ���¼�����									//GetStaticType()��ȡ��ǰ���¼����ͱ��
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
//ָ���¼�����
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class HAZEL_API Event//�¼�����
	{
	public:
		bool Handled = false;
		//�����麯�������Ծ���ʵ�ַ���Ҫ��������
		virtual EventType GetEventType() const = 0;//��ȡ�¼����ͱ��
		virtual const char* GetName() const = 0;//��ȡ�¼�������
		virtual int GetCategoryFlags() const = 0;//��ȡ�¼�������
		virtual std::string ToString() const { return GetName(); }//��ȡ�¼����������Լ����ԣ������ڵ�����Ϣ

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;//���������ж��Ƿ����¼�������
		}
	
	};

	class EventDispatcher//�¼�������
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;//���ͱ�������EventFn����std::function<bool(T&)>
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)//���ĳ���뺯�����е��ȣ��������Ϊĳ����������
		{
			if (m_Event.GetEventType() == T::GetStaticType())//�ж��Ƿ�
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)//������أ��ܹ����Event����ʵ�����¼����������Լ������ԣ�
	{
		return os << e.ToString();
	}
}