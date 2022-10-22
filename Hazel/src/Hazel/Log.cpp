#include "hzpch.h"
#include "Log.h"

namespace Hazel
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;//static ����������Ҫ��������
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;//static ����������Ҫ��������

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");//����log��ʽ��[ʱ��] ����: ����

		s_CoreLogger = spdlog::stdout_color_mt("HAZEL");//��������
		s_CoreLogger->set_level(spdlog::level::trace);//���õȼ�

		s_ClientLogger = spdlog::stdout_color_mt("APP");//��������
		s_ClientLogger->set_level(spdlog::level::trace);//���õȼ�
	}
}

