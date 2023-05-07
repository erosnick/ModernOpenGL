#pragma once

#include <filesystem>

namespace AriaCore
{
	class ScriptEngine
	{
	public:
		ScriptEngine() = default;
		~ScriptEngine() = default;

		static void init();
		static void initMono();

		static void loadAssembly(const std::filesystem::path& path);
		
		static void shutdown();

		static void monoTest();
	private:

	};
}