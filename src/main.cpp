#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Window.h"
#include "SceneHandler.h"
#include "Camera.h"
#include "OrbitalPhysics.h"

namespace
{
    void handleTimeChange(GLfloat yScrollOffset, GLfloat* timeChange)
    {
        if (yScrollOffset == 0.0f) return;

        const GLfloat amountChange = yScrollOffset * 0.1f;
        *timeChange += (*timeChange + amountChange > 3.0f || *timeChange + amountChange < -0.1f) ? 0 : amountChange;
    }

    void setupScene()
    {
        window::width = 1920;
        window::height = 1200;

        // Print out the controls
        std::cout << "**********\n";
        std::cout << "\033[92m" << "Controls" << "\033[0m\n";
        std::cout << "**********\n";
        std::cout << "\nMouse: Look around\n\n";
        std::cout << "W: Move forward\n";
        std::cout << "S: Move backwards\n";
        std::cout << "A: Move left\n";
        std::cout << "D: Move right\n\n";
        std::cout << "Q: Roll left\n";
        std::cout << "E: Roll right\n\n";
        std::cout << "Space: Move up\n";
        std::cout << "Shift: Move down\n\n";
        std::cout << "F: Toggle flashlight\n";
        std::cout << "L: Toggle shadows\n\n\n";

        // Promp user to select a numerical integration scheme
        std::cout << "\033[92m" << "Choose a numerical integration method" << "\033[0m\n";
        std::cout << "0: Euler method\n1: Verlet method \n>";
        std::cin >> OrbitalPhysics::verlet;

        // Prompt user to select scene
        int selectedScene {};
        std::cout << "\033[92m" << "\nChoose a scene" << "\033[0m\n";
        std::cout << "1: 1 planet 1 sun\n2: Lots of objects\n3: Figure eight\n4: Final release scene\n> ";
        std::cin >> selectedScene;
        
        // If this isn't right here, we will get a segmentation fault
        window::initialize();
        
        // Build scene based on user input
        switch (selectedScene)
        {
            case 1:
                SceneHandler::createObjects1Sun1Planet();
                break;
            case 2:
                SceneHandler::createObjectsDefault();
                break;
            case 3:
                SceneHandler::createObjectsFigureEight();
                break;
            case 4:
                SceneHandler::createObjectsFancy();
                break;
            default:
                break;
        }

        // Projection defines how the 3D world is projected onto a 2D screen. We're using a perspective matrix
        const glm::mat4 projection {glm::perspective(glm::radians(60.0f), static_cast<GLfloat>(window::bufferWidth) / static_cast<GLfloat>(window::bufferHeight), 1.0f, 400.0f)};
        SceneHandler::setupSkybox(projection);
        renderer::setup(projection);
    }
}

int main()
{
    setupScene();

    // Loop until window is closed
    GLfloat now {};
    std::string frameRateStr {};
    unsigned int counter {0};
    double lastFPSUpdateTime {glfwGetTime()};
    GLfloat lastFrame {0.0f};
    GLfloat deltaTime {0.0f};
    GLfloat timeChange {1.0f};
    GLfloat timeStep {0.0f};
    GLfloat timeSinceLastVerlet {0.0f}; // Not relevant if we're using Euler for physics
    while(!glfwWindowShouldClose(window::mainWindow))
    {
        now         = glfwGetTime();
        deltaTime   = now - lastFrame;
        lastFrame   = now;
        timeStep    = deltaTime * timeChange;
        ++counter;

        // Update FPS counter
        if (counter == 30)
        {
            frameRateStr = "Solar System - " + std::to_string(30.0 / (now - lastFPSUpdateTime)) + " FPS";
            glfwSetWindowTitle(window::mainWindow, frameRateStr.c_str());
            lastFPSUpdateTime = now;
            counter = 0;
        }

        OrbitalPhysics::updateCelestialBodyAngles(timeStep);

        // Update our object's positions based on our chosen numerical scheme
        if (OrbitalPhysics::verlet)
            OrbitalPhysics::updatePositionsVerlet(&timeSinceLastVerlet);
        else
            OrbitalPhysics::updatePositionsEuler(timeStep);

        // Get + handle user input
        glfwPollEvents();
        bool* keys {window::keys};
        camera::keyControl(keys, deltaTime);
        camera::mouseControl(window::getXChange(), window::getYChange());
        handleTimeChange(window::getYScrollOffset(), &timeChange);

        // Check for flashlight toggle
        if (keys[GLFW_KEY_L])
        {
            // Setting this to false means it won't trigger multiple times when we press it once
            keys[GLFW_KEY_L] = false;

            renderer::toggleShadows();
        }

        renderer::omniShadowMapPasses();
        renderer::renderPass();

        glUseProgram(0);
        glfwSwapBuffers(window::mainWindow);
    }

    return 0;
}
