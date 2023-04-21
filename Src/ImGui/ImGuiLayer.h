#pragma once

class ImGuiLayer
{
public:
	void initImGui(struct GLFWwindow* window);

	void updateImGui();
	void renderImGui();

	void imGuiShutdown();
};