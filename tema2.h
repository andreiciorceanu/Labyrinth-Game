#pragma once

#include "lab_m1/tema2/camera.h"
#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/tema2/maze.h"
#include "lab_m1/tema2/inamici.h"


#define Z_FAR		(200.f)
#define Z_NEAR		(.01f)

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));
        void DrawHuman();
        void DrawEnemy(float deltaTimeSeconds);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void DrawLabirint();
        //shader
        glm::vec3 lightPosition;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;
        //camera
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        bool projectionType;

        GLfloat right;
        GLfloat left;
        GLfloat bottom;
        GLfloat top;
        GLfloat fov;
        //player
        float playerX, playerY, playerZ;
        float posStartX, posStartZ;
        //labirint
        Maze labirint;
       std::vector<std::pair<int, int>> zeros;
       float wallMatrix;
       //enemy
       std::vector<Inamici> inamici;
       int count;
       float angle;
       float inamicX, inamicZ;
       //orientare
       float angleRotation;
       float speed = 1;
    };
}   // namespace m1
   