#include "lab_m1/tema2/tema2.h"

#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stack>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{   

    srand(time(NULL));
    renderCameraTarget = false;
    projectionType = true;
    angleRotation = 0;
    right = 10.f;
    left = .01f;
    bottom = .01f;
    top = 10.f;
    fov = 40.f;
    camera = new implemented::Camera();
    //camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }



    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        lightPosition = glm::vec3(0, 1, 1);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;
    }
    //generare-labirint
    labirint = Maze(5, 5, 2);
    labirint.generate();
    /*for (int i = 0; i < labirint.maze.size(); i++) {
        cout << endl;
        for (int j = 0; j < labirint.maze[i].size(); j++) {
            cout << labirint.maze[i][j] << " ";
        }
    }*/
    //creeare vector cu pozitiile libire din labirint
    for (int i = 0; i < labirint.maze.size(); i++) {
        for (int j = 0; j < labirint.maze[i].size(); j++) {
            if (labirint.maze[i][j] == 0) {
                zeros.push_back({ i,j });
           }
        }
    }
    //posStart- player
    int index = rand() % zeros.size();
    posStartX = zeros[index].first;
    posStartZ = zeros[index].second;
    zeros.erase(zeros.begin() + index);
    //human
    playerX = posStartX;
    playerY = 0.6f;
    playerZ = posStartZ;
    //inamici, setare pozitiilor random de spawnare a inamicilor
    count = 4;
    angle = 3.14f;
    while (count != 0) {
        int enemyIndex = rand() % zeros.size();
        inamici.push_back(Inamici(zeros[enemyIndex].first, 0, zeros[enemyIndex].second, 1));
        zeros.erase(zeros.begin() + enemyIndex);
        count--;
    }
    //setare camera sa urmareasca jucatorul
    camera->Set(glm::vec3(playerX - 0.5f, playerY + 0.4f, playerZ + camera->distanceToTarget), glm::vec3(playerX, playerY, playerZ), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    

    // Render ground
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.01f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(1,1,1));
    }
    //generare harta,labirint,inamici si jucator
    DrawLabirint();
    DrawEnemy(deltaTimeSeconds);
    DrawHuman();
}

void Tema2::DrawEnemy(float deltaTimeSeconds) {
    //miscarea inamicilor
    angle -= deltaTimeSeconds / 2;
    if (angle < -3.14f) {
        angle = 3.14f;
    }
    inamicX = cos(angle);
    inamicZ = sin(2 * angle) / 2;
    //generare inamici
    for (int i = 0; i < inamici.size(); i++) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(inamici[i].x + inamicX, 0.3f, inamici[i].z + inamicZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.6f, 0.3f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.7f, 0.14f, 0.14f));
    }
}

void Tema2::DrawHuman() {
    playerX = camera->GetTargetPosition().x;
    playerY = camera->GetTargetPosition().y;
    playerZ = camera->GetTargetPosition().z;

        if (!renderCameraTarget) {
            {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.5f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.f, 1.f, 0.53f));
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX + 0.125f, playerY - 0.41f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.3f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.1f, 0.5f, 0.7f));
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX - 0.125f, playerY - 0.41f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.3f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.1f, 0.5f, 0.7f));
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX + 0.29f, playerY + 0.15f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.17f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.f, 1.f, 0.53f));
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX - 0.29f, playerY + 0.15f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.17f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.f, 1.f, 0.53f));
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX + 0.29f, playerY - 0.07f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.23f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.9f, 0.93f, 0.584f));
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX - 0.29f, playerY - 0.07f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.23f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.9f, 0.93f, 0.584f));
        }

        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY + 0.36f, playerZ));
            modelMatrix = glm::rotate(modelMatrix, angleRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.9f, 0.93f, 0.584f));
        }
    }
}

void Tema2::DrawLabirint() {
    //scena
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(5, 0.01f, 5));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(20, 0.25f, 20));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.36f, 0.56f, 0.36f));
    }

    for (int i = 0; i < labirint.maze.size(); i++) {
        for(int j = 0; j < labirint.maze[i].size(); j++) {
            if (labirint.maze[i][j] == 1) {
                glm::mat4 wallMatrix = glm::mat4(1);
                wallMatrix = glm::translate(wallMatrix, glm::vec3(0 + i, 1.5f, 0 + j));
                wallMatrix = glm::scale(wallMatrix, glm::vec3(1, 3, 1));
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], wallMatrix, glm::vec3(0.55f, 0.56f, 0.55f));
            }
        }
    }
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);


    auto objectColorLocation = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(objectColorLocation, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    //// Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 2;

        // Control light position using on W, A, S, D, E, Q
        if (window->KeyHold(GLFW_KEY_W)) {
            playerZ += deltaTime * speed;
            camera->MoveForward(deltaTime * speed);
        }
        if (window->KeyHold(GLFW_KEY_A))
        {
            playerX -= deltaTime * speed;
            camera->TranslateRight(-deltaTime * speed);
        }
        if (window->KeyHold(GLFW_KEY_S)) {
            playerZ -= deltaTime * speed;
            camera->MoveForward(-deltaTime * speed);
        }
        if (window->KeyHold(GLFW_KEY_D)) {
            playerX += deltaTime * speed;
            camera->TranslateRight(deltaTime * speed);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(-deltaTime * speed);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(deltaTime * speed);
        }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T) {
        renderCameraTarget = !renderCameraTarget;
        if (!renderCameraTarget) {
            camera->MoveForward(-1.5f);
        }
        else {
            camera->MoveForward(1.5f);
        }
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;
        angleRotation += -sensivityOY * deltaX;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            
            camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            camera->RotateThirdPerson_OX(sensivityOX * -deltaY);
            camera->RotateThirdPerson_OY(sensivityOY * -deltaX);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
