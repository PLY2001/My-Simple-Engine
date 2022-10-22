#include "hzpch.h"
#include "Log.h"

namespace Hazel
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;//static 变量在这里要重新声明
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;//static 变量在这里要重新声明

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");//设置log形式：[时间] 名字: 内容

		s_CoreLogger = spdlog::stdout_color_mt("HAZEL");//设置名字
		s_CoreLogger->set_level(spdlog::level::trace);//设置等级

		s_ClientLogger = spdlog::stdout_color_mt("APP");//设置名字
		s_ClientLogger->set_level(spdlog::level::trace);//设置等级
	}
}

