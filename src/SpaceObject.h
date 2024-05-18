#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.h"

class SpaceObject
{
    public:
        SpaceObject();
        explicit SpaceObject(GLfloat mass);

        GLfloat getMass();
        glm::vec3 getOldPosition() const;
        void setOldPosition(glm::vec3 oldPos);
        glm::vec3 getPosition() const;
        void setPosition(glm::vec3 position);
        glm::vec3 getVelocity() const;
        void setVelocity(glm::vec3 velocity);
        glm::vec3 getRotation() const;
        void setRotation(glm::vec3 rotation);
        GLfloat getAngle();
        void setAngle(GLfloat angle);
        GLfloat getRotationSpeed();
        void setRotationSpeed(GLfloat speed);
        void setMaterialPointer(Material* material);
        Material* getMaterialPointer();
        virtual void render() = 0;
        virtual void setWorldProperties(glm::mat4* model) = 0;
        virtual void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) = 0;
        virtual GLfloat getCollisionDistance() = 0;

        ~SpaceObject();

    protected:
        const GLfloat mass {};
        glm::vec3 oldPosition {};
        glm::vec3 position {};
        glm::vec3 velocity {};
        glm::vec3 rotation {};
        GLfloat angle {};
        GLfloat rotationSpeed {};
        Material *material {};
};
