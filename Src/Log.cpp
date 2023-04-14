#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::coreLogger;
std::shared_ptr<spdlog::logger> Log::clientLogger;

void Log::init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	coreLogger = spdlog::stdout_color_mt("Aria");
	coreLogger->set_level(spdlog::level::trace);

	coreLogger->trace("trace");
	coreLogger->info("info");
	coreLogger->warn("warn");
	coreLogger->error("error");
	coreLogger->critical("critical");

	clientLogger = spdlog::stdout_color_mt("App");
	clientLogger->set_level(spdlog::level::trace);
}
