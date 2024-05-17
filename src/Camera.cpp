#include "Camera.h"

namespace camera
{
    glm::vec3 position {glm::vec3{0.0f}};
    glm::vec3 worldUp {glm::vec3{0.0f, 1.0f, 0.0f}};
    GLfloat yaw {-90.0f};
    GLfloat oldYaw {-90.0f};
    GLfloat pitch {0.0f};
    GLfloat oldPitch {0.0f};
    GLfloat roll {0.0f};
    GLfloat oldRoll {0.0f};
    
    glm::vec3 front {glm::normalize(glm::vec3{0.0f, 0.0f, -1.0f})};
    glm::vec3 right {glm::normalize(glm::cross(front, worldUp))};
    glm::vec3 up {glm::normalize(glm::cross(right, front))};

    GLfloat moveSpeed {5.0f};
    GLfloat turnSpeed {1.0f};

    SpotLight *spotLight {};

    void keyControl(bool* keys, GLfloat deltaTime)
    {
        const GLfloat velocity {moveSpeed * deltaTime};

        if (keys[GLFW_KEY_W])
            position += front * velocity;
        if (keys[GLFW_KEY_A])
            position -= right * velocity;
        if (keys[GLFW_KEY_S])
            position -= front * velocity;
        if (keys[GLFW_KEY_D])
            position += right * velocity;

        // Up and down
        if (keys[GLFW_KEY_LEFT_SHIFT])
            position -= up * velocity;
        if (keys[GLFW_KEY_SPACE])
            position += up * velocity;

        // Roll
        if (keys[GLFW_KEY_E])
            roll += turnSpeed * 3;
        if (keys[GLFW_KEY_Q])
            roll -= turnSpeed * 3;

        handleFlashlightKey(keys);
    }

    void handleFlashlightKey(bool* keys)
    {
        // Check for flashlight toggle
        if (!keys[GLFW_KEY_F])
            return;

        // Setting this to false means we won't trigger it multiple times when we press it once
        keys[GLFW_KEY_F] = false;

        // If flashlight is disabled, don't put it in the shader (it's the last spotLight in our array)
        spotLight->toggle();
    }

    void mouseControl(GLfloat xChange, GLfloat yChange)
    {
        xChange *= turnSpeed;
        yChange *= turnSpeed;

        yaw += xChange;
        pitch += yChange;

        update();
    }

    void setSpotLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat xDir, GLfloat yDir, GLfloat zDir,
                    GLfloat exponential, GLfloat linear, GLfloat constant,
                    GLfloat edge)
    {
        spotLight = new SpotLight{shadowWidth, shadowHeight,
                            near, far,
                            red, green, blue,
                            ambientIntensity, diffuseIntensity,
                            xPos, yPos, zPos,
                            xDir, yDir, zDir,
                            exponential, linear, constant,
                            edge};
    }

    glm::mat4 calculateViewMatrix()
    {
        // Args are where the camera is, what the camera is looking at, and what its up is
        return glm::lookAt(position, position + front, up);
    }

    void update()
    {
        const glm::mat4 yawMatrix {glm::rotate(glm::mat4{1.0f}, glm::radians(oldYaw - yaw), up)};
        front       = glm::normalize(glm::vec3{yawMatrix * glm::vec4{front, 0.0f}});
        oldYaw      = yaw;
        right       = glm::normalize(glm::cross(front, up));

        glm::mat4 pitchMatrix {glm::rotate(glm::mat4{1.0f}, glm::radians(pitch - oldPitch), right)};
        front       = glm::normalize(glm::vec3{pitchMatrix * glm::vec4{front, 0.0f}});
        oldPitch    = pitch;
        up          = glm::normalize(glm::cross(right, front));

        glm::mat4 rollMatrix {glm::rotate(glm::mat4{1.0f}, glm::radians(roll - oldRoll), front)};
        up          = glm::normalize(glm::vec3{rollMatrix * glm::vec4{up, 0.0f}});
        oldRoll     = roll;
        right       = glm::normalize(glm::cross(front, up));

        // Update the flashlight position and direction
        spotLight->setFlash(position, front);
    }
}
