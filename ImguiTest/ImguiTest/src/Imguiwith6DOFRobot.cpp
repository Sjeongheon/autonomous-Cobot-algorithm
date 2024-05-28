// ImguiTest.cpp : Defines the entry point for the application.
//

#include "ImguiTest.h"
#include <cmath>
#include <gl/GLU.h>
#include <iostream>

using namespace SYE;

DrawClient::DrawClient(double size, Pose pose): mSize(size), mPose(pose) {
	//mSize = size;
}

void DrawClient::draw() {
}

/////////////////////////////////////////////////////////////////////////////


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#define PI 3.14159265358979323846
#define RES 100

struct Vec3 {
    float x, y, z;
};

struct Transform {
    Vec3 t;
    Vec3 Rz;
};

struct Robot {
    int n_links;
    int n_dofs;
};

struct Kinematics {
    Robot robot;
    Transform* h_tf;
    Transform* h_tf_links;
    Vec3* link_com;
    Vec3* link_vel;
    Vec3* com_vel;
    Vec3* link_acc;
    Vec3* com_acc;
};

struct Options {
    int draw_wireframe;
    int use_alpha;
    int draw_joint_frame[RES];
    int draw_motion_vectors;
};

void angle_axis_from_vec_dir(Vec3 vec, Vec3 dir, float* ang, Vec3* ax) {
    *ang = acos(vec.z);
    ax->x = -dir.y;
    ax->y = dir.x;
    ax->z = 0.0f;
}

float linkangle1 = 50.0f;
float linkangle2 = 0.0f;
float linkangle4 = 0.0f;
float linkangle5 = 0.0f;
float linkangle6 = 0.0f;
float linkangle7 = 90.0f;

// Render function for the robot graphics
void render_robot(Kinematics* kinematics, Options* options) {
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);

    if (options->draw_wireframe) {
        gluQuadricDrawStyle(quadric, GLU_LINE);
    } else {
        gluQuadricDrawStyle(quadric, GLU_FILL);
    }

    if (options->use_alpha) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }

    float linkHeight = 0.05f;
    float modelRadius = 0.3f;
    // Base
    float baseHeight = 0.2f;
    float baseRadius = 0.4f;
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    gluCylinder(quadric, baseRadius, modelRadius, baseHeight, 32, 32);
    
    // Model 1
    float modelHeight1 = 0.6f;
    glTranslatef(0.0f, 0.0f, baseHeight);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight1, 32, 32);
    
    // link 1
    glColor3f(0.5f, 0.0f, 0.0f);
    glRotatef(linkangle1, 0.0f, 0.0f, 1.0f);
    gluCylinder(quadric, modelRadius, modelRadius, linkHeight, 32, 32);

    // Model 2
    glColor3f(0.5f, 0.5f, 0.5f);
    float modelHeight2 = 0.5f;
    glTranslatef(modelRadius, 0.0f, modelHeight1/2);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight2, 32, 32);

    // link 2
    glColor3f(0.5f, 0.0f, 0.0f);
    glRotatef(linkangle2, 0.0f, 0.0f, 1.0f);
    gluCylinder(quadric, modelRadius, modelRadius, linkHeight, 32, 32);

    // Model 3
    glColor3f(0.5f, 0.5f, 0.5f);
    float modelHeight3 = 2.0f;
    glTranslatef(-modelRadius, 0.0f, modelHeight2/2);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight3, 32, 32);

    // Model 4
    glColor3f(0.5f, 0.5f, 0.5f);
    float modelHeight4 = 0.5f;
    glTranslatef(-modelRadius, 0.0f, modelHeight3 - modelRadius);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight4, 32, 32);
    
    // Link 4
    glColor3f(0.5f, 0.0f, 0.0f);
    glRotatef(linkangle4, 0.0f, 0.0f, 1.0f);
    gluCylinder(quadric, modelRadius, modelRadius, linkHeight, 32, 32);

    // Model 5
    glColor3f(0.5f, 0.5f, 0.5f);
    float modelHeight5 = 2.0f;
    glTranslatef(-modelRadius, 0.0f, modelHeight4/2);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight5, 32, 32);

    // Link 5
    glColor3f(0.5f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, modelHeight5 - modelRadius*2);
    glRotatef(linkangle5, 0.0f, 0.0f, 1.0f);
    gluCylinder(quadric, modelRadius, modelRadius, linkHeight, 32, 32);
    
    // Model 6
    glColor3f(0.5f, 0.5f, 0.5f);
    float modelHeight6 = 0.5f;
    glTranslatef(modelRadius, 0.0f, modelRadius);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight6, 32, 32);
    
    // Link 6
    glColor3f(0.5f, 0.0f, 0.0f);
    glRotatef(linkangle6, 0.0f, 0.0f, 1.0f);
    gluCylinder(quadric, modelRadius, modelRadius, linkHeight, 32, 32);

    // Model 7
    glColor3f(0.5f, 0.5f, 0.5f);
    float modelHeight7 = 0.5f;
    glTranslatef(modelRadius, 0.0f, modelHeight6/2);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight7, 32, 32);
    
    // Link 7
    glColor3f(0.5f, 0.0f, 0.0f);
    glRotatef(linkangle7, 0.0f, 0.0f, 1.0f);
    gluCylinder(quadric, modelRadius, modelRadius, linkHeight, 32, 32);

    // Model 8
    glColor3f(0.5f, 0.5f, 0.5f);
    float modelHeight8 = 0.5f;
    glTranslatef(0.0f, modelRadius, modelRadius);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadric, modelRadius, modelRadius, modelHeight8, 32, 32);
    
    glPopMatrix();
    // // 
    // for (int i = 1; i <= kinematics->robot.n_links; ++i) {
    //     float link_len = sqrt(pow(kinematics->h_tf[i].t.x - kinematics->h_tf[i-1].t.x, 2) +
    //                           pow(kinematics->h_tf[i].t.y - kinematics->h_tf[i-1].t.y, 2) +
    //                           pow(kinematics->h_tf[i].t.z - kinematics->h_tf[i-1].t.z, 2));
    //     float ang;
    //     Vec3 ax;
    //     angle_axis_from_vec_dir({0.0f, 0.0f, 1.0f}, 
    //                             {kinematics->h_tf[i].t.x - kinematics->h_tf[i-1].t.x,
    //                              kinematics->h_tf[i].t.y - kinematics->h_tf[i-1].t.y,
    //                              kinematics->h_tf[i].t.z - kinematics->h_tf[i-1].t.z},
    //                             &ang, &ax);

    //     glPushMatrix();
    //     glColor4f(0.8f, 0.3f, 0.3f, 0.7f);
    //     glTranslatef(kinematics->h_tf[i-1].t.x, kinematics->h_tf[i-1].t.y, kinematics->h_tf[i-1].t.z);
    //     glRotatef(ang * (180.0 / PI), ax.x, ax.y, ax.z);
    //     gluCylinder(quadric, 2.0f, 2.0f, 2.0f, 32, 32);
    //     glPopMatrix();
    // }

    // for (int i = 1; i <= kinematics->robot.n_dofs; ++i) {
    //     float ang;
    //     Vec3 ax;
    //     angle_axis_from_vec_dir({0.0f, 0.0f, 1.0f}, kinematics->h_tf[i-1].Rz, &ang, &ax);
    //     glPushMatrix();
    //     glColor4f(0.3f, 0.8f, 0.3f, 0.7f);
    //     glTranslatef(kinematics->h_tf[i-1].t.x, kinematics->h_tf[i-1].t.y, kinematics->h_tf[i-1].t.z);
    //     glRotatef(ang * (180.0 / PI), ax.x, ax.y, ax.z);
    //     glTranslatef(0.0f, 0.0f, -0.5f * 0.1f);
    //     gluCylinder(quadric, 2.0f, 2.0f, 5.0f, 32, 32);
        
    //     glColor4f(0.3f, 0.8f, 0.3f, 0.7f);
    //     glTranslatef(kinematics->h_tf[i-1].t.x, kinematics->h_tf[i-1].t.y, kinematics->h_tf[i-1].t.z);
    //     glRotatef(ang * (180.0 / PI), ax.x, ax.y, ax.z);
    //     glTranslatef(0.0f, 0.0f, 0.5f * 0.1f);
    //     gluDisk(quadric, 0.0f, 2.0f, 32, 1);
        
    //     glColor4f(0.3f, 0.8f, 0.3f, 0.7f);
    //     glTranslatef(kinematics->h_tf[i-1].t.x, kinematics->h_tf[i-1].t.y, kinematics->h_tf[i-1].t.z);
    //     glRotatef(ang * (180.0 / PI), ax.x, ax.y, ax.z);
    //     glTranslatef(0.0f, 0.0f, -0.5f * 0.1f);
    //     gluDisk(quadric, 0.0f, 2.0f, 32, 1);
    //     glPopMatrix();
    // }

    // glPushMatrix();
    // glColor4f(0.3f, 0.3f, 0.8f, 0.7f);
    // glTranslatef(kinematics->h_tf[kinematics->robot.n_links].t.x,
    //              kinematics->h_tf[kinematics->robot.n_links].t.y,
    //              kinematics->h_tf[kinematics->robot.n_links].t.z);
    // gluSphere(quadric, 2.0f, 32, 32);
    // glPopMatrix();

    // for (int i = 0; i <= kinematics->robot.n_links; ++i) {
    //     glPushMatrix();
    //     glColor4f(0.8f, 0.8f, 0.3f, 0.7f);
    //     glTranslatef(kinematics->link_com[i].x, kinematics->link_com[i].y, kinematics->link_com[i].z);
    //     gluSphere(quadric, 2.0f, 32, 32);
    //     glPopMatrix();
    // }

    gluDeleteQuadric(quadric);
}

// 더미 데이터 생성
void create_dummy_data(Kinematics* kinematics) {
    kinematics->robot.n_links = 4;
    kinematics->robot.n_dofs = 4;

    kinematics->h_tf = (Transform*)malloc((4 + 1) * sizeof(Transform));
    kinematics->h_tf_links = (Transform*)malloc((4 + 1) * sizeof(Transform));
    kinematics->link_com = (Vec3*)malloc((4 + 1) * sizeof(Vec3));
    kinematics->link_vel = (Vec3*)malloc((4 + 1) * sizeof(Vec3));
    kinematics->com_vel = (Vec3*)malloc((4 + 1) * sizeof(Vec3));
    kinematics->link_acc = (Vec3*)malloc((4 + 1) * sizeof(Vec3));
    kinematics->com_acc = (Vec3*)malloc((4 + 1) * sizeof(Vec3));

    for (int i = 0; i <= 4; ++i) {
        kinematics->h_tf[i].t = {i * 0.1f, 0.0f, 0.0f};
        kinematics->h_tf[i].Rz = {0.0f, 0.0f, 1.0f};

        kinematics->link_com[i] = {i * 0.1f, 0.0f, 0.0f};
        kinematics->link_vel[i] = {0.0f, 1.0f, 0.0f};
        kinematics->com_vel[i] = {0.0f, 1.0f, 0.0f};
        kinematics->link_acc[i] = {0.0f, 1.0f, 0.0f};
        kinematics->com_acc[i] = {0.0f, 1.0f, 0.0f};
    }
}

// Clears the window and draws the tetrahedron.
void display() {
    glBegin(GL_LINES);
    glColor4f(0.3f, 0.3f, 0.8f, 0.7f);
    float len = 100.0f;
    float width = 100.0f;
    for (float i = -len/2; i <= len/2; i += len/RES) {
        for (float j = -width/2; j <= width/2; j += width/RES) {
            // 그리드 라인 그리기
            glVertex3f(i, -width/2, 0);
            glVertex3f(i, width/2, 0);
            glVertex3f(-len/2, j, 0);
            glVertex3f(len/2, j, 0);

            // 대각선 그리기
            if (i < len/2 && j < width/2) {
                glVertex3f(i, j, 0);
                glVertex3f(i + len/RES, j + width/RES, 0);
                glVertex3f(i + len/RES, j, 0);
                glVertex3f(i, j + width/RES, 0);
            }
        }
    }
    glEnd();

    // // Draw the tetrahedron.
    // glBegin(GL_TRIANGLE_STRIP);
    // glColor3f(1, 1, 1); glVertex3f(0, 2, 0);
    // glColor3f(1, 0, 0); glVertex3f(-1, 0, 1);
    // glColor3f(0, 1, 0); glVertex3f(1, 0, 1);
    // glColor3f(0, 0, 1); glVertex3f(0, 0, -1);
    // glColor3f(1, 1, 1); glVertex3f(0, 2, 0);
    // glColor3f(1, 0, 0); glVertex3f(-1, 0, 1);
    // glEnd();

    Options options = {1, 1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 1};
    Kinematics kinematics;
    create_dummy_data(&kinematics);
    render_robot(&kinematics, &options);
}

// Sets up global attributes and matrices.
void init() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1, 0.39, 0.88, 1.0);
    glColor3f(1.0, 1.0, 1.0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2, 2, -1.5, 1.5, 1, 40);
}

void changeViewPoint(float f1, float a1, float a2, float a3) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, f1);
    glRotatef(a1, 1, 0, 0);
    glRotatef(a2, 0, 1, 0);
    glRotatef(a3, 0, 0, 1);
}


// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    // Make the window's context current
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

    // Set up global attributes and matrices
    init();

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

		//Client for draw
		SYE::DrawClient draw0(0.5);
		SYE::DrawClient *pDraw1 = new SYE::DrawClient(0.3);

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
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        static float f = -3.0f;
        static float ax = 0.0f;
        static float ay = 0.0f;
        static float az = 0.0f;
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        display();
				draw0.draw();
				pDraw1->draw();

        changeViewPoint(f, ax, ay, az);

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

            ImGui::SliderFloat("move", &f, -5.0f, 5.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("rotateX", &ax, -180.0f, 180.0f);
            ImGui::SliderFloat("rotateY", &ay, -180.0f, 180.0f);
            ImGui::SliderFloat("rotateZ", &az, -180.0f, 180.0f);

            ImGui::SliderFloat("linkangle1", &linkangle1, -180.0f, 180.0f);
            ImGui::SliderFloat("linkangle2", &linkangle2, -180.0f, 180.0f);
            ImGui::SliderFloat("linkangle4", &linkangle4, -180.0f, 180.0f);
            ImGui::SliderFloat("linkangle5", &linkangle5, -180.0f, 180.0f);
            ImGui::SliderFloat("linkangle6", &linkangle6, -180.0f, 180.0f);
            ImGui::SliderFloat("linkangle7", &linkangle7, -180.0f, 180.0f);
            
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
