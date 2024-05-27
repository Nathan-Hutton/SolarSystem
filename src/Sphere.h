#pragma once

#include <vector>

#include "SpaceObject.h"
#include "Mesh.h"
#include "Texture.h"

class Sphere : public SpaceObject
{
    public:
        Sphere();
        Sphere(GLfloat radius, GLfloat mass, int stacks, int slices, bool usingNormals=true);

        void setTexturePointer(Texture* texture);
        virtual void render() const override = 0;
        void setWorldProperties(glm::mat4& model) override;
        virtual void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override = 0;
        GLfloat getCollisionDistance() const override;

        ~Sphere();

    protected:
        virtual void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int stacks, int slices, bool usingNormals=true);
        const GLfloat radius {};
        Mesh *sphereMesh {};
        Texture *texture {};
};
