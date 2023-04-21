#pragma once

#include <memory>

#include <spdlog/spdlog.h>

class Log
{
public:
	static void init();
	static const std::shared_ptr<spdlog::logger>& getCoreLogger() { return coreLogger; }
	static const std::shared_ptr<spdlog::logger>& getClientLogger() { return clientLogger; }

private:
	static std::shared_ptr<spdlog::logger> coreLogger;
	static std::shared_ptr<spdlog::logger> clientLogger;
};

#define ARIA_CORE_TRACE(...) Log::getCoreLogger()->trace(__VA_ARGS__);
#define ARIA_CORE_INFO(...) Log::getCoreLogger()->info(__VA_ARGS__);
#define ARIA_CORE_WARN(...) Log::getCoreLogger()->warn(__VA_ARGS__);
#define ARIA_CORE_ERROR(...) Log::getCoreLogger()->error(__VA_ARGS__);
#define ARIA_CORE_CRITICAL(...) Log::getCoreLogger()->critical(__VA_ARGS__);

#define ARIA_CLIENT_TRACE(...) Log::getClientLogger()->trace(__VA_ARGS__);
#define ARIA_CLIENT_INFO(...) Log::getClientLogger()->info(__VA_ARGS__);
#define ARIA_CLIENT_WARN(...) Log::getClientLogger()->warn(__VA_ARGS__);
#define ARIA_CLIENT_ERROR(...) Log::getClientLogger()->error(__VA_ARGS__);
#define ARIA_CLIENT_CRITICAL(...) Log::getClientLogger()->critical(__VA_ARGS__);