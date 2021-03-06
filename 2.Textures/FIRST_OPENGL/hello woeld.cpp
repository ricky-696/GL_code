#include <iostream>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <SOIL/SOIL.h>


// key down
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const GLuint WIDTH = 800, HEIGHT = 600;

int T_width, T_height;
unsigned char* image = SOIL_load_image("wall.jpg", &T_width, &T_height, 0, SOIL_LOAD_RGB);

int main()
{
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "First_OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
//----------------- Define the viewport dimensions------------------------
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

// -----------Build and compile our shader program---------------------
    Shader ourShader("vectorShader.txt", "fragmentShader.txt");
//--------------------create vertex-----------------------------------------
    GLfloat vertices[] = { //頂點座標,顏色,紋理座標
     //x,y,z             //RGB
     0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,// 右上角
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// 右下角
     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,// 左下角
     -0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f
    };
    GLuint indices[] = { //頂點連線座標
        0, 1, 3, // first tri
        1, 2, 3  // two
    };

//--------紋理(綁定紋理對象-->設置環繞方式-->加載並生成紋理)----------------------------------
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    //          (dim(v),多級漸遠紋理等級,紋理儲存格式,width,height,always0,讀取格式,儲存格式,image)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, T_width, T_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    //release
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

//----------創建緩衝對象 --> 綁定緩衝類型 --> set data------------
    GLuint VBO,VAO,EBO;    
    glGenBuffers(1, &VBO); //Vertex Buffer Objects
    glGenVertexArrays(1, &VAO);//Vertex Array Objects(存放頂點)
    glGenBuffers(1, &EBO);//索引緩衝對象-Element Buffer Object(頂點編號indices[])

    glBindVertexArray(VAO);//綁定vao

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //GL_ARRAY_BUFFER頂點緩衝類型
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//sent data in buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //解析數據{座標屬性}      (Location,vec3,float,normalize,stride,GLvoid)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    //解析數據{顏色屬性}      (Location,vec3,float,normalize,stride,GLvoid)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //解析數據{紋理屬性}
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    
    //----------------------- Game loop----------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Check events (key pressed, mouse moved etc.) 
        glfwPollEvents();

//--------------------------<Render>---------------------------------
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //激活shader

        //draw
        ourShader.Use();
        glBindTexture(GL_TEXTURE_2D, texture); 
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //   draw     (繪製型態,頂點個數,索引類型,偏移量);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
}