#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Simulation.h"
#include "Globals.h"

#include "Input.h"

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	/** Create the window */
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Multi-agent Pathfinding", sf::Style::Default, settings);
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	if (!Globals::InitGlobals())
		return false;

	bool scrolling = false;
	bool firstTick = false;
	sf::Vector2i lastMousePos;

	Simulation simulation;

	float cameraSpeed = 100.f;

	/** Initial view settings */
	sf::View view = window.getView();
	view.setCenter(sf::Vector2f(0, 0));
	window.setView(view);

	Input input(&simulation, &window, &view);

	sf::Clock deltaClock;
	while (window.isOpen())
	{
		sf::Time deltaTime = deltaClock.restart();
		float dt = deltaTime.asSeconds();

		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			input.ProcessInput(event);

			switch (event.type)
			{
			case sf::Event::Closed:
			{
				window.close();
				break;
			}
			case sf::Event::MouseWheelMoved:
			{
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Space)
					simulation.Step();
				if (event.key.code == sf::Keyboard::Escape)
					window.close();

				break;
			}
			case sf::Event::MouseMoved:
			{
				if (scrolling)
				{
					if (firstTick)
					{
						lastMousePos.x = event.mouseMove.x;
						lastMousePos.y = event.mouseMove.y;
						firstTick = false;
					}
					else
					{
						sf::Vector2i currentMousePos(event.mouseMove.x, event.mouseMove.y);
						sf::Vector2i deltaPos = lastMousePos - currentMousePos;
						view.move(sf::Vector2f(deltaPos.x, deltaPos.y) * input.zoom);
						lastMousePos = currentMousePos;
					}
				}
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (!scrolling)
					{
						firstTick = true;
						scrolling = true;
					}
				}
				break;
			}
			case sf::Event::MouseButtonReleased:
			{
				if (event.mouseButton.button == sf::Mouse::Left)
					scrolling = false;
				break;
			}
			}
		}

		/** Update the simulation */
		simulation.Update(dt);

		/** Move view by left clicking and dragging */

		window.setView(view);
		ImGui::SFML::Update(window, deltaTime);

		bool open = true;


		///** Full screen window (invisible content, just for the menu bar) */
		//ImGui::SetNextWindowSize(Graphics::GetDisplaySize(), ImGuiSetCond_Always);
		//ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
		//ImGui::Begin("MenuBar", &open, ImVec2(0, 0), 0.f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
		//simulation.BuildMenuBar();
		//ImGui::End();

		/** Options window (on the left) */
		//ImGui::SetNextWindowSize(ImVec2((maxSize - minSize) / 2.f, static_cast<float>(display_h)), ImGuiSetCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiSetCond_Once);
		ImGui::Begin("Options", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
		simulation.BuildOptions();
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImGui::End();

		/** Info window (on the right) */
		
		ImVec2 infoPadding(25.f, 75.f);
		ImGui::SetNextWindowPos(ImVec2(window.getSize().x - infoPadding.x, infoPadding.y), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Example: Fixed Overlay", &open, ImVec2(0, 0), 0.2f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
		simulation.LogInfo();
		ImGui::End();

		window.clear();
		simulation.Render(window);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}


//// https://gamedev.stackexchange.com/questions/140693/how-can-i-render-an-opegl-scene-into-an-imgui-window
//
//// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>
//
//// Include GLEW
//#include <GL/glew.h>
//
//#include <iostream>
//
//// Include GLFW
//#include <GLFW/glfw3.h>
////GLFWwindow* window;
//
//// Include GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//using namespace glm;
//
//#include <common/shader.hpp>
//
//#include <imgui.h>
//#include <imgui_impl_glfw_gl3.h>
//#include "imgui-SFML.h"
//#include "Mesh.h"
//
//#include "Simulation.h"
//Simulation* simulation;
//
//#include "Input.h"
//Input* input;
//
//#include "Graphics.h"
//#include <chrono>
//#include <ctime>
//
//#include <SFML/Graphics.hpp>
//
//static double g_TimeAcc;
//
//
//int main2()
//{
//	// create the window
//	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
//
//	sf::Clock clock;
//
//	sf::View view = window.getDefaultView();
//	//view.zoom(0.5f);
//
//	// Create a new render-texture
//	sf::RenderTexture texture;
//
//	if (!texture.create(window.getSize().x, window.getSize().y))
//		return -1;
//
//	for (int x = 0; x < 500; x++)
//	{
//		for (int y = 0; y < 500; y++)
//		{
//			sf::RectangleShape rectangle(sf::Vector2f(1, 1));
//			rectangle.setPosition(sf::Vector2f(x * 1.5, y * 1.5));
//			texture.draw(rectangle);
//
//			//std::cout << x << "," << y << std::endl;
//		}
//	}
//
//	sf::Sprite sprite(texture.getTexture());
//
//	// run the program as long as the window is open
//	while (window.isOpen())
//	{
//
//		sf::Time deltaTime = clock.restart();
//
//		std::cout << deltaTime.asMilliseconds() << std::endl;
//
//		float dt = deltaTime.asMilliseconds();
//
//		view.move(sf::Vector2f(100.f, 5.f) * dt);
//
//		// check all the window's events that were triggered since the last iteration of the loop
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			// "close requested" event: we close the window
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//
//		// clear the window with black color
//		window.clear(sf::Color::Black);
//
//		// draw everything here...
//		window.draw(sprite);
//
//		// end the current frame
//		window.display();
//	}
//
//	return 0;
//}
//
//int main()
//{
//	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
//
//	Graphics graphics;
//	if (!graphics.initGraphics())
//		return -1;
//
//	ImGui_ImplGlfwGL3_Init(graphics.window, true);
//
//	// create the Simulation
//	simulation = new Simulation();
//	input = new Input();
//
//	std::chrono::time_point<std::chrono::system_clock> prevTime = std::chrono::system_clock::now();
//
//	do
//	{
//		int displayWidth, displayHeight;
//		glfwGetFramebufferSize(graphics.window, &displayWidth, &displayHeight);
//		Graphics::UpdateDisplaySize(displayWidth, displayHeight);
//
//		// Rendering
//		glfwPollEvents();
//		ImGui_ImplGlfwGL3_NewFrame();
//
//		// update the simulation and camera according to user input
//		input->Update(0.16f, simulation, graphics.camera);
//		graphics.camera->update(0.16f);
//
//		//if (g_TimeAcc > .033)
//		{
//			//g_TimeAcc -= .033;
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//			glViewport(0, 0, Graphics::GetDisplaySize().x, Graphics::GetDisplaySize().y);
//
//			simulation->Render(&graphics);
//		}
//
//		bool open = true;
//
//		// Full screen window (invisible content, just for the menu bar)
//		ImGui::SetNextWindowSize(Graphics::GetDisplaySize(), ImGuiSetCond_Always);
//		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
//		ImGui::Begin("MenuBar", &open, ImVec2(0, 0), 0.f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
//		simulation->BuildMenuBar();
//		ImGui::End();
//
//		// Options window (on the left)
//		//ImGui::SetNextWindowSize(ImVec2((maxSize - minSize) / 2.f, static_cast<float>(display_h)), ImGuiSetCond_Always);
//		ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiSetCond_Once);
//		ImGui::Begin("Options", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
//		simulation->BuildOptions();
//		ImVec2 pos = ImGui::GetWindowPos();
//		ImVec2 size = ImGui::GetWindowSize();
//		ImGui::End();
//
//		// Info window (on the right)
//		ImVec2 infoPadding(25.f, 75.f);
//		ImGui::SetNextWindowPos(ImVec2(Graphics::GetDisplaySize().x - 300 / 2.f - infoPadding.x, infoPadding.y), ImGuiSetCond_FirstUseEver);
//		ImGui::Begin("Example: Fixed Overlay", &open, ImVec2(0, 0), 0.2f, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
//		simulation->LogInfo();
//		ImGui::End();
//
//		/*ImGui::SetNextWindowPos(ImVec2(10, 10));
//		if (!ImGui::Begin("Example: Fixed Overlay", &open, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
//		{
//			ImGui::End();
//		}
//		ImGui::Text("Simple overlay\non the top-left side of the screen.");
//		ImGui::Separator();
//		ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
//		ImGui::End();*/
//
//		glViewport(0, 0, Graphics::GetDisplaySize().x, Graphics::GetDisplaySize().y);
//		ImGui::Render();
//		glfwSwapBuffers(graphics.window);
//
//		// delta time
//		std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
//		std::chrono::duration<double> timeElapsed = currentTime - prevTime;
//		g_TimeAcc += timeElapsed.count();
//		prevTime = currentTime;
//
//		// check all the window's events that were triggered since the last iteration of the loop
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			// "close requested" event: we close the window
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//
//		// clear the window with black color
//		window.clear(sf::Color::Black);
//
//		// draw everything here...
//		//window.draw(sprite);
//
//		// end the current frame
//		window.display();
//
//	} // Check if the ESC key was pressed or the window was closed
//	while (glfwGetKey(graphics.window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(graphics.window) == 0);
//
//	graphics.CleanUp();
//
//	// Close OpenGL window and terminate GLFW
//	glfwTerminate();
//
//	return 0;
//}