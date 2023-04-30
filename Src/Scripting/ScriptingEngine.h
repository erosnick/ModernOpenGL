#pragma once

#include <filesystem>

namespace AriaCore
{
	class ScriptingEngine
	{
	public:
		ScriptingEngine() = default;
		~ScriptingEngine() = default;

		static void init();
		static void initMono();

		static void loadAssembly(const std::filesystem::path& path);
		
		static void shutdown();

		static void monoTest();
	private:

	};
}