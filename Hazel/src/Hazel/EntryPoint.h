#pragma once
#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();//不知道有啥用，删掉也无影响

int main(int argc, char** argv)
{
	Hazel::Log::Init();
	//HZ_CORE_ERROR("Error!");
	HZ_CLIENT_WARN("Value = {0}",5);

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif // HZ_PLATFORM_WINDOWS

