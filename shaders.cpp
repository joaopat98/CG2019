/* #include "shaders.h"

GLchar chr[10000];
GLchar *buf = chr;
struct VertexPC
{
    float x, y, z, r, g, b, a;
    VertexPC(float ix, float iy, float iz, float ir, float ig, float ib, float ia) : x(ix), y(iy), z(iz), r(ir), g(ig), b(ib), a(ia)
    {
    }
};
static VertexPC *verts;
static int *indices;

// These 2 are "handles" that connect the POSITION attribute in
// the GLSL shader with __AN ACTUAL SET OF VERTICES__ from the C++ source code.
// In this program each VERTEX has a position and a color (only).
GLuint positionAttribute, colorAttribute;

// This is the connector between the uniform variable modelViewMatrix,
// used in the glsl shader code.  We need this variable to LOAD
// a specific modelview matrix INTO the GLSL shader before rendering the verts.
GLuint uniformModelViewMatrix;

// The vertex array object identifier.  This single number
// identifies EVERYTHING about the vertex buffer we are
// about to create and render, from its attribute set (positions, colors)
// to where to fetch the data from when it's time to draw.
GLuint vao;

void loadShaderSource(const char *filename)
{
    FILE *fd = fopen(filename, "r");
    fread(buf, sizeof(char), 10000, fd);
}

void loadShaders()
{
    glewInit();

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    loadShaderSource("vertex_shader.fs");
    glShaderSource(vertexShader, 1, &buf, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    loadShaderSource("fragment_shader.fs");
    glShaderSource(fragmentShader, 1, &buf, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    positionAttribute = glGetAttribLocation(shaderProgram, "position");
    colorAttribute = glGetAttribLocation(shaderProgram, "color");

    uniformModelViewMatrix = glGetUniformLocation(shaderProgram, "modelViewMatrix");

    // Make a "vertex buffer"
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    // LOAD THE DATA INTO IT:
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Make an "index buffer"
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(
        positionAttribute, // index. NOTE THIS HAS BEEN CONNECTED UP EARLIER
        // WITH THE VERTEX SHADER THAT WILL BE USED TO RENDER THIS VERTEX ARRAY
        // IN THE LINE:
        //   > positionAttribute = glGetAttribLocation( glslProgId, "position" ) ; // C++ code line
        // and "position" is used in the GLSL vertex shader as:
        //   > attribute vec3 position; // GLSL shader line
        // SO. Don't be confused, the `positionAttribute` variable is simply connected up
        // to the vertex shader's INCOMING "position" attribute.  Naked girls.
        3,                // size. 3 floats/vertex
        GL_FLOAT,         // type. floats!
        0,                // normalized? No.
        sizeof(VertexPC), // stride
        0                 // pointer.  0 says to use the CURRENTLY BOUND
                          // VERTEX BUFFER (WHATEVER THAT IS AT THE TIME OF THE DRAW CALL).
                          // If you want to draw from CLIENT MEMORY you would pass AN ACTUAL ADDRESS
                          // here (BUT YOU MUST MAKE SURE TO HAVE NO VERTEX BUFFER BOUND BECAUSE THEN
                          // THE MEMORY ADDRESS YOU PASS HERE WOULD BE INTERPRETTED AS AN __OFFSET__
                          // INTO THAT VERTEX BUFFER, TRIGGERING A SEGFAULT/OUT OF BOUNDS EXCEPTION TYPE THING.)
    );

    glEnableVertexAttribArray(colorAttribute);

    int offset = 3 * sizeof(float);
    glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, 0, sizeof(VertexPC),

                          (void *)offset // RE: THE LAST PARAMETER: I know it looks ridiculous!! to pass a NUMBER as cast to VOID* here,
                                         // but that's C api's for you.  "Wait! Shouldn't it be &offset?"
                                         // NO. Your eyes kid you not.  You take the INTEGER NUMBER (which amounts to 12 here)
                                         // AND JUST PASS THAT AS IF IT WERE A "VOID*" POINTER. Even though you and I know it is not.
    );
} */