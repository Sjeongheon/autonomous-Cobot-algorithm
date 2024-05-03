#include "ImguiTest.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace SYE;

// Shader Program ID
GLuint shaderProgram;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO, floorVAO, floorVBO;

float baseAngle = 0.0f;
float lowerArmAngle = 0.0f;
float upperArmAngle = 0.0f;

static float perspective_angle = 100.0f;
static float cameralocation_x = 0.0f;
static float cameralocation_y = 0.0f;
static float cameralocation_z = 5.0f;

static float targetlocation_x = 0.0f;
static float targetlocation_y = 0.0f;
static float targetlocation_z = 5.0f;

GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
};

GLuint indices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    0, 3, 7, 7, 4, 0,
    1, 2, 6, 6, 5, 1,
    0, 1, 5, 5, 4, 0,
    2, 3, 7, 7, 6, 2
};

static float viewRange = 10.0f;

std::vector<float> generateGridVertices(float viewRange) {
    std::vector<float> vertices;
    for (float i = -viewRange; i <= viewRange; i += 1.0f) {
        // xz 평면에서 z축 방향으로 줄
        vertices.push_back(i);
        vertices.push_back(0.0f);
        vertices.push_back(-viewRange);
        vertices.push_back(i);
        vertices.push_back(0.0f);
        vertices.push_back(viewRange);

        // xz 평면에서 x축 방향으로 줄
        vertices.push_back(-viewRange);
        vertices.push_back(0.0f);
        vertices.push_back(i);
        vertices.push_back(viewRange);
        vertices.push_back(0.0f);
        vertices.push_back(i);
    }
    return vertices;
}

const char* vertSource =
"#version 330 core \n\
layout (location = 0) in vec3 aPos; \n\
uniform mat4 model; \n\
uniform mat4 view; \n\
uniform mat4 projection; \n\
void main(void) { \n\
    gl_Position = projection * view * model * vec4(aPos, 1.0); \n\
}";

const char* fragSource =
"#version 330 core \n\
out vec4 FragColor; \n\
uniform vec4 objectColor; \n\
void main(void) { \n\
    FragColor = objectColor; \n\
}";

DrawClient::DrawClient(double size, Pose pose) : mSize(size), mPose(pose) {
    mSize = size;
    //Create Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertSource, NULL);
    glCompileShader(vertexShader);

    //Create Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    // Attach the Vertex and Fragment Shaders to the Shader Program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // Link to get .EXE

    // Delete the now useless Vertex and Fragment Shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //VBO : Vertex Buffer Object, EBO : Element Buffer Object, VAO : Vertex Array Object
    unsigned int VBO, EBO;
    //Create VAO - Multiple VBO configuration + attribute
    glGenVertexArrays(1, &VAO);
    // Create VBO - store vertex data to GPU mem
    glGenBuffers(1, &VBO);
    // Create EBO - store indices data(how to connect vertex)
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 바닥 정점 생성 및 버퍼 설정
    std::vector<float> floorVertices = generateGridVertices(viewRange);
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, floorVertices.size() * sizeof(float), &floorVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DrawClient::draw() {
    // 셰이더 프로그램 사용
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // 기본 행렬 설정
    glm::mat4 projection = glm::perspective(glm::radians(perspective_angle), (float)1280 / (float)720, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(cameralocation_x, cameralocation_y, cameralocation_z),
        glm::vec3(targetlocation_x, targetlocation_y, targetlocation_z),
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    // 바닥 그리기
    glBindVertexArray(floorVAO);
    glUniform4f(objectColorLoc, 0.7f, 0.7f, 0.7f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawArrays(GL_LINES, 0, (viewRange * 2 + 1) * 4 * 2);
    glBindVertexArray(0);

    // 기본 관절의 위치 설정 및 그리기
    glBindVertexArray(VAO);
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(baseAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    glUniform4f(objectColorLoc, 1.0f, 0.5f, 0.2f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 입방체로 관절 부분 그리기
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
    glUniform4f(objectColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 하위 팔 그리기
    model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(upperArmAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform4f(objectColorLoc, 1.0f, 0.5f, 0.2f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 입방체로 관절 부분 그리기
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
    glUniform4f(objectColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 하위 팔 그리기
    model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(lowerArmAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform4f(objectColorLoc, 0.5f, 0.0f, 0.5f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

/////////////////////////////////////////////////////////////////////////////

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        baseAngle += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        baseAngle -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        lowerArmAngle += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lowerArmAngle -= 1.0f;
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    // Initialize GLFW
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui + OpenGL Testing Tool Prototype", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load GLAD 
    gladLoadGL();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    //Client for draw
    SYE::DrawClient draw0(0.5, Pose(0.0f, 0.0f, 0.0f));

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput(window);

        //Rendering
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw0.draw();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("3D visualization tool");
        ImGui::SetWindowSize(ImVec2(400, 400));
        ImGui::Text("Work Optimization for Cobot");
        ImGui::Text("Perspective Setting");
        ImGui::SliderFloat("Perspective", &perspective_angle, 0.0f, 180.0f);
        ImGui::SliderFloat("rotateX", &cameralocation_x, -180.0f, 180.0f);
        ImGui::SliderFloat("rotateY", &cameralocation_y, -180.0f, 180.0f);
        ImGui::SliderFloat("rotateZ", &cameralocation_z, -180.0f, 180.0f);
        ImGui::Text("\nMove to Location - Inverse Kinematics");
        ImGui::SliderFloat("FloatX", &targetlocation_x, -20.0f, 20.0f);
        ImGui::SliderFloat("FloatY", &targetlocation_y, -20.0f, 20.0f);
        ImGui::SliderFloat("FloatZ", &targetlocation_z, -20.0f, 20.0f);
        ImGui::Text("\nMoving Arm - Forward Kinematics");
        ImGui::SliderFloat("baseAngle", &baseAngle, -180.0f, 180.0f);
        ImGui::SliderFloat("upperArmAngle", &upperArmAngle, -180.0f, 180.0f);
        ImGui::SliderFloat("lowerArmAngle", &lowerArmAngle, -180.0f, 180.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
