#include "Angel.h"

#include<iostream>
#include<fstream>
#include <string>
#include<crtdbg.h>
using namespace std;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

point4* vertices;
color4* colors;

point4* vertices_sorted;
color4* colors_sorted;

// Viewing transformation parameters

GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;

GLuint model_view;  // model-view matrix uniform shader variable location
GLuint scaling;
GLuint rotateY;
float degree_y = 0.0;
GLuint rotateX;
float degree_x = 0.0;

// Projection transformation parameters ????
GLfloat  fovy = 90.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 50.0;
GLuint  projection; // projection matrix uniform shader variable location
GLdouble viewer[3] = { 1.5, 0.0, 0.0 };


float max_x = -9999;
float max_y = -9999;
float max_z = -9999;
float min_x = 9999;
float min_y = 9999;
float min_z = 9999;
float max_distance = -9999;

float x_min = 9999;
float x_max = -9999;
float y_min = 9999;
float y_max = -9999;
float z_min = 9999;
float z_max = -9999;

GLuint translate1;
GLuint translate2;
float translate_x = 0;
float translate_y = 0;
float translate_z = 0;

float scaling_rate = 1.0;

int drawingMode = 0;
/*
*  0: GL_TRIANGLE
*  1: GL_LINE
*  2: GL_VERTEX
*/

int NumOfFiles;
char* files[7];
string currFileName;
int file_num = 0;

int NumVertices = 0; //(M faces)(1 triangles/face)(3 vertices/triangle)
int index;
float size_of_vertices_sorted;
float size_of_colors_sorted;
int N = 0;  //vertices
int M = 0;  //faces


void commWShader() {
    // Create a vertex array object
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size_of_vertices_sorted + size_of_colors_sorted,
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size_of_vertices_sorted, vertices_sorted);
    glBufferSubData(GL_ARRAY_BUFFER, size_of_vertices_sorted, size_of_colors_sorted, colors_sorted);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("hw2vs.glsl", "fshader42.glsl");
    glUseProgram(program);

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(point4) * M * 3));

    model_view = glGetUniformLocation(program, "model_view");
    projection = glGetUniformLocation(program, "projection");
    scaling = glGetUniformLocation(program, "scaling");
    rotateY = glGetUniformLocation(program, "rotateY");
    rotateX = glGetUniformLocation(program, "rotateX");
    translate1 = glGetUniformLocation(program, "translate1");
    translate2 = glGetUniformLocation(program, "translate2");

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void make_triangle(int loc_p1, int loc_p2, int loc_p3) {
    colors_sorted[index] = colors[loc_p1];
    vertices_sorted[index] = vertices[loc_p1];
    index++;
    colors_sorted[index] = colors[loc_p2];
    vertices_sorted[index] = vertices[loc_p2];
    index++;
    colors_sorted[index] = colors[loc_p3];
    vertices_sorted[index] = vertices[loc_p3];
    index++;
}

void fillArrays() {

    x_min = 9999;
    x_max = -9999;
    y_min = 9999;
    y_max = -9999;
    z_min = 9999;
    z_max = -9999;

    translate_x = 0;
    translate_y = 0;
    translate_z = 0;

    scaling_rate = 1.0;

    degree_x = 0.0;
    degree_y = 0.0;
    index = 0;
    fstream file;
    string word;
    file.open(currFileName);
    int w = 0;  //count words
    int top = 0;
    int in_line_ct = 0;
    int line_ct = 0;
    //vertice coordinates
    float x;
    float y;
    float z;
    //M yuzey
    int first, second, third, fourth;
    //int index = 0;
    while (file >> word) {
        if (w == 0 || w == 3)
            w++;
        else if (w == 1) {
            N = stoi(word);
            w++;
        }
        else if (w == 2) {
            M = stoi(word);
            vertices = new point4[N];
            colors = new color4[N];
            vertices_sorted = new point4[M * 3];
            size_of_vertices_sorted = M * 3 * sizeof(point4);
            colors_sorted = new color4[M * 3];
            size_of_colors_sorted = M * 3 * sizeof(color4);
            NumVertices = 3 * M;
            w++;
        }
        else if (w == 4) {
            if (line_ct < N) {
                if (in_line_ct == 0) {
                    x = stof(word);
                    if (x > x_max)
                        x_max = x;
                    else if (x < x_min)
                        x_min = x;
                    y = 0.0;
                    z = 0.0;
                    in_line_ct++;
                }
                else if (in_line_ct == 1) {
                    y = stof(word);
                    if (y > y_max)
                        y_max = y;
                    else if (y < y_min)
                        y_min = y;
                    in_line_ct++;
                }
                else if (in_line_ct == 2) {
                    z = stof(word);
                    if (z > z_max)
                        z_max = z;
                    else if (z < z_min)
                        z_min = z;

                    //assign vertex and vertex's color
                    point4* p = new point4(x, y, z, 1.0);
                    vertices[line_ct] = *p;
                    float blue = (double)rand() / (double)RAND_MAX;
                    /*float red = (double)rand() / (double)RAND_MAX;
                    float green = (double)rand() / (double)RAND_MAX;
                    color4* c = new color4(red, green, blue, 1.0);*/
                    color4* c = new color4(0.0, 0.0, blue, 1.0);
                    colors[line_ct] = *c;
                    in_line_ct = 0;
                    line_ct++;
                }
                else
                    cout << "ERROR";
            }
            //vertice sonu
            //face basi
            else {
                if (in_line_ct == 0) {
                    int b = line_ct - N + 1;
                    first = stoi(word);
                    in_line_ct++;
                    if (first != 3)
                        cout << "ERROR OCCURRED" << endl;
                }
                else if (in_line_ct == 1) {
                    second = stoi(word);
                    in_line_ct++;
                }
                else if (in_line_ct == 2) {
                    third = stoi(word);
                    in_line_ct++;
                }
                else if (in_line_ct == 3) {
                    fourth = stoi(word);
                    in_line_ct = 0;
                    line_ct++;
                    make_triangle(second, third, fourth);
                }
            }
            //face sonu
        }
    }
    file.close();

    float x_mid = (x_max - x_min) / 2;
    float y_mid = (y_max - y_min) / 2;
    float z_mid = (z_max - z_min) / 2;
    translate_x = x_max - x_mid;
    translate_y = y_max - y_mid;
    translate_z = z_max - z_mid;

    float m_x1 = abs(x_max);
    float m_x2 = abs(x_min);
    float m_y1 = abs(y_max);
    float m_y2 = abs(y_min);
    float m_z1 = abs(z_max);
    float m_z2 = abs(z_min);
    float arr[6] = { m_x1, m_x2, m_y1, m_y2, m_z1, m_z2 };

    float maxx = 1;
    float max_xx = -9999;
    for (int nn = 0; nn < 6; nn++) {
        if (arr[nn] > max_xx) {
            max_xx = arr[nn];
            maxx = arr[nn];
        }
    }
    scaling_rate = 1 / maxx;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    commWShader();
}

void init()
{
    string name = string(files[file_num]);
    currFileName = name;
    fillArrays();
    glutPostRedisplay();

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    point4  eye(viewer[0], viewer[1], viewer[2], 1.0);
    point4  at(0.0, 0.0, 0.0, 1.0);
    vec4    up(0.0, 1.0, 0.0, 0.0);

    mat4  mv = LookAt(eye, at, up);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p);

    mat4  scale = Scale(scaling_rate, scaling_rate, scaling_rate);
    glUniformMatrix4fv(scaling, 1, GL_TRUE, scale);

    mat4  rt = RotateY(degree_y);
    glUniformMatrix4fv(rotateY, 1, GL_TRUE, rt);

    mat4  rt2 = RotateX(degree_x);
    glUniformMatrix4fv(rotateX, 1, GL_TRUE, rt2);

    mat4  tr1 = Translate(translate_x, translate_y, translate_z);
    glUniformMatrix4fv(translate1, 1, GL_TRUE, tr1);

    mat4  tr2 = Translate(-1 * translate_x, -1 * translate_y, -1 * translate_z);
    glUniformMatrix4fv(translate2, 1, GL_TRUE, tr2);


    if (drawingMode == 0)
        glDrawArrays(GL_TRIANGLES, 0, M * 3);
    else if (drawingMode == 1)
        glDrawArrays(GL_LINE_STRIP, 0, M * 3);
    else
        glDrawArrays(GL_POINTS, 0, M * 3);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{

    if (key == 'x') {
        viewer[0] -= 0.10;
    }
    else if (key == 'X') {
        viewer[0] += 0.1;
    }
    else if (key == 'y') {
        viewer[1] -= 0.1;
    }
    else if (key == 'Y') {
        viewer[1] += 0.1;
    }
    else if (key == 'z') {
        viewer[2] -= 0.1;
    }
    else if (key == 'Z') {
        viewer[2] += 0.1;
    }
    else if (key == 'N' || key == 'n') {
        file_num = (file_num + 1) % NumOfFiles;
        currFileName = string(files[file_num]);
        fillArrays();
    }
    else if (key == 'p' || key == 'P') {
        if (file_num == 0) {
            file_num = NumOfFiles - 1;
        }
        else
            file_num--;
        currFileName = string(files[file_num]);
        fillArrays();
    }
    else if (key == 'v' || key == 'V') {
        drawingMode = 2;
    }
    else if (key == 'e' || key == 'E') {
        drawingMode = 1;
    }
    else if (key == 'f' || key == 'F') {
        drawingMode = 0;
    }
    else if (key == 's') {
        scaling_rate *= 0.5;
    }
    else if (key == 'S') {
        scaling_rate *= 2.0;
    }
    else if (key == 'q' || key == 'Q') {
        //free(files);
        free(vertices);
        free(vertices_sorted);
        free(colors_sorted);
        free(colors);
        /*for (int i = 0; i < 7; i++) {
            free(files[i]);
        }*/
        _CrtDumpMemoryLeaks();
        exit(EXIT_SUCCESS);
    }

    glutPostRedisplay();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = GLfloat(width) / height;
    glutPostRedisplay();
}

void SpecialInput(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT) {
        degree_y += 2.0;
    }
    else if (key == GLUT_KEY_LEFT) {
        degree_y -= 2.0;
    }
    else if (key == GLUT_KEY_UP) {
        degree_x += 2.0;
    }
    else if (key == GLUT_KEY_DOWN) {
        degree_x -= 2.0;
    }

    glutPostRedisplay();

}

int main(int argc, char* argv[])
{
    NumOfFiles = argc - 1;
    for (int i = 1; i < argc; ++i) {
        cout << argv[i] << endl;
        files[i - 1] = argv[i];
    }
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(512, 512);
    glutCreateWindow("HW2");
    glewExperimental = GL_TRUE;
    glewInit();

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(SpecialInput);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
