// ImguiTest.cpp : Defines the entry point for the application.
//

#include "ImguiTest.h"

using namespace SYE;

// Shader Program ID
GLuint shaderProgram;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO;

float baseAngle = 0.0f;
float lowerArmAngle = 0.0f;
float upperArmAngle = 0.0f;

static float perspective_angle = 100.0f;
static float cameralocation_x = 0.0f;
static float cameralocation_y = 0.0f;
static float cameralocation_z = 5.0f;

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
}

void DrawClient::draw() {
    // 셰이더 프로그램 사용
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // 기본 행렬 설정
    glm::mat4 projection = glm::perspective(glm::radians(perspective_angle) // 시야 각
        , (float)1280 / (float)720, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(cameralocation_x, cameralocation_y, cameralocation_z) // 카메라 위치
        , glm::vec3(0.0f, 0.0f, 0.0f) // 바라보는 지점
        , glm::vec3(0.0f, 1.0f, 0.0f)); // 상단 방향
    glm::mat4 model = glm::mat4(1.0f);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    // 기본 관절의 위치 설정 및 그리기
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(baseAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    glUniform4f(objectColorLoc, 1.0f, 0.5f, 0.2f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);  // 세그먼트 그리기

    // 입방체로 관절 부분 그리기
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f)); // 관절의 크기 조절
    glUniform4f(objectColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 관절 그리기

    // 하위 팔 그리기
    model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f)); // 크기 복원
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(upperArmAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform4f(objectColorLoc, 1.0f, 0.5f, 0.2f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 입방체로 관절 부분 그리기
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f)); // 관절의 크기 조절
    glUniform4f(objectColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 관절 그리기

    // 하위 팔 그리기
    model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f)); // 크기 복원
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

void changeViewPoint(float f1, float a1, float a2, float a3) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, f1);
    glRotatef(a1, 1, 0, 0);
    glRotatef(a2, 0, 1, 0);
    glRotatef(a3, 0, 0, 1);
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
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    //// Tell GLFW what version of OpenGL we are using 
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we are Using Core-Profile 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui + OpenGL Testing Tool Prototype", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    // Make the window's context current - current thread
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Load GLAD 
    gladLoadGL();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    // x=0, y=0 to x=1280, y=720
    glViewport(0, 0, display_w, display_h);

    //Client for draw
    SYE::DrawClient draw0(0.5, Pose(0.0f, 0.0f, 0.0f));
    //SYE::DrawClient* pDraw1 = new SYE::DrawClient(0.3);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput(window);

        //Rendering
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        //display();
        draw0.draw();
        //pDraw1->draw();



        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::SetWindowSize(ImVec2(400, 250));
            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("Perspective", &perspective_angle, 0.0f, 180.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("rotateX", &cameralocation_x, -180.0f, 180.0f);
            ImGui::SliderFloat("rotateY", &cameralocation_y, -180.0f, 180.0f);
            ImGui::SliderFloat("rotateZ", &cameralocation_z, -180.0f, 180.0f);
            ImGui::SliderFloat("baseAngle", &baseAngle, -180.0f, 180.0f);
            ImGui::SliderFloat("upperArmAngle", &upperArmAngle, -180.0f, 180.0f);
            ImGui::SliderFloat("lowerArmAngle", &lowerArmAngle, -180.0f, 180.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
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
