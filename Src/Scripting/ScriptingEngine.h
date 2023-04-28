#pragma once

namespace AriaCore
{
	class ScriptingEngine
	{
	public:
		ScriptingEngine() = default;
		~ScriptingEngine() = default;

		static void init();
		static void initMono();
		
		static void shutdown();

		static void monoTest();
	private:

	};
}