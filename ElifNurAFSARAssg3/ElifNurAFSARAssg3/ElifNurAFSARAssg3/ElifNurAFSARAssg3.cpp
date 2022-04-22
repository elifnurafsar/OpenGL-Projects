
#include "Angel.h"

#include<iostream>
#include<fstream>
#include <string>
#include<crtdbg.h>
#include ".\Angel.h"

using namespace std;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//Distinct colors and vertices for terrain
point4** Vertices;
color4** Colors;

//points array that will be send to vbo
point4* vertices_sorted;
//color array that will be send to vbo
color4* colors_sorted;
//normals array that will send to vbo
vec3* normals_sortedFlat;
vec3* normals_sortedGauraud;

//colors and vertices for airplane
point4* vertices_sortedAP;
color4* colors_sortedAP;
int size_of_vertices_sortedAP;
int size_of_colors_sortedAP;
int NumVerticesAP = 12;

//counter for new created vertices
int counter;
int row = 0;
float magnitude = 0.5;
float h = 0.5;

int NumOfSubdivisions = 1;
int NumVertices = 0; //(M faces)(1 triangles/face)(3 vertices/triangle)
int index;
int size_of_vertices_sorted;
int size_of_colors_sorted;
int size_of_normals_sorted;

// Viewing transformation parameters
GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;

GLuint model_view;  // model-view matrix uniform shader variable location
GLuint scaling;
GLuint rotateY;
float degree_y = 0.0;
GLuint rotateZ;
float degree_z = 0.0;

// Projection transformation parameters ????
GLfloat  fovy = 90.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 50.0;
GLuint  projection; // projection matrix uniform shader variable location
GLdouble viewer[3] = { 0.0, 0.0, 1.0 };
GLdouble look_at[3] = { 0.0, 0.0, 0.0 };
GLdouble upp[3] = { 0.0, 1.0, 0.0 };

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

int shadingMode = 0;
/*
*  0: FLAT
*  1: GOURAUD
*/
GLuint shading_mode;

int viewingMode = 0;
/*
*  0: 0, 0, 1
*  1: plane_front
*  2: plane_back
*/

float material_shininess = 500.0;
point4 light_position;
color4 light_ambient;
color4 light_diffuse;
color4 light_specular;
color4* material_ambient_sorted;
color4* material_diffuse_sorted;
color4* material_specular_sorted;
int size_of_material_ambient_sorted;
int size_of_material_diffuse_sorted;
int size_of_material_specular_sorted;

GLdouble plane[3] = { 0.0, -3.0, 0.5 };
GLdouble look_at_plane[3] = { 0.0, 0.0, 0.0 };
float degree_z_plane = 0.0;
float degree_y_plane = 0.0;


//This function produces random numbers
GLfloat myRandom(double min, double max)
{
    GLfloat val = (float)rand() / RAND_MAX;
    double v = max - min;
    if (min < 0 && max > 0)
        v = max + min;    val = min + val * (max - min);
    return val;
}

void commWShader() {
    // Create a vertex array object
    GLuint vao;

    glGenVertexArrays(1, &vao);
    //activate
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int buff_size = size_of_vertices_sorted + size_of_normals_sorted + size_of_normals_sorted + size_of_material_diffuse_sorted + size_of_material_specular_sorted + size_of_material_ambient_sorted;
    glBufferData(GL_ARRAY_BUFFER, buff_size,
        NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size_of_vertices_sorted, vertices_sorted);

    int sze = size_of_vertices_sorted;
    glBufferSubData(GL_ARRAY_BUFFER, sze, size_of_normals_sorted, normals_sortedFlat);

    sze = sze + size_of_normals_sorted;
    glBufferSubData(GL_ARRAY_BUFFER, sze, size_of_normals_sorted, normals_sortedGauraud);

    sze = sze + size_of_normals_sorted;
    glBufferSubData(GL_ARRAY_BUFFER, sze, size_of_material_ambient_sorted, material_ambient_sorted);

    sze = sze + size_of_material_ambient_sorted;
    glBufferSubData(GL_ARRAY_BUFFER, sze, size_of_material_diffuse_sorted, material_diffuse_sorted);

    sze = sze + size_of_material_diffuse_sorted;
    glBufferSubData(GL_ARRAY_BUFFER, sze, size_of_material_specular_sorted, material_specular_sorted);


    // Load shaders and use the resulting shader program
    GLuint program = InitShader("hw3vs.glsl", "fshader36.glsl");
    glUseProgram(program);

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(0));

    int sze_x = size_of_vertices_sorted;
    GLuint vNormalF = glGetAttribLocation(program, "vNormalF");
    glEnableVertexAttribArray(vNormalF);
    glVertexAttribPointer(vNormalF, 3, GL_FLOAT, GL_TRUE, 0,
        BUFFER_OFFSET(sze_x));

    sze_x = sze_x + sizeof(vec3) * NumVertices;
    GLuint vNormalG = glGetAttribLocation(program, "vNormalG");
    glEnableVertexAttribArray(vNormalG);
    glVertexAttribPointer(vNormalG, 3, GL_FLOAT, GL_TRUE, 0,
        BUFFER_OFFSET(sze_x));

    sze_x = sze_x + sizeof(vec3) * NumVertices;
    GLuint vColorA = glGetAttribLocation(program, "vColorA");
    glEnableVertexAttribArray(vColorA);
    glVertexAttribPointer(vColorA, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sze_x));

    sze_x = sze_x + sizeof(color4) * NumVertices;
    GLuint vColorD = glGetAttribLocation(program, "vColorD");
    glEnableVertexAttribArray(vColorD);
    glVertexAttribPointer(vColorD, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sze_x));

    sze_x = sze_x + sizeof(color4) * NumVertices;
    GLuint vColorS = glGetAttribLocation(program, "vColorS");
    glEnableVertexAttribArray(vColorS);
    glVertexAttribPointer(vColorS, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sze_x));

    model_view = glGetUniformLocation(program, "model_view");
    projection = glGetUniformLocation(program, "projection");
    scaling = glGetUniformLocation(program, "scaling");
    rotateY = glGetUniformLocation(program, "rotateY");
    rotateZ = glGetUniformLocation(program, "rotateZ");
    translate1 = glGetUniformLocation(program, "translate1");
    translate2 = glGetUniformLocation(program, "translate2");
    shading_mode = glGetUniformLocation(program, "shadingMode");

    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
    glUniform4fv(glGetUniformLocation(program, "LightAmbient"), 1, light_ambient);
    glUniform4fv(glGetUniformLocation(program, "LightSpecular"), 1, light_specular);
    glUniform4fv(glGetUniformLocation(program, "LightDiffuse"), 1, light_diffuse);
    glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529, 0.808, 0.980, 1.0);
    /*// Create a vertex array object
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
    GLuint program = InitShader("vshader36.glsl", "fshader36.glsl");
    glUseProgram(program);

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(point4) * NumVertices));

    model_view = glGetUniformLocation(program, "model_view");
    projection = glGetUniformLocation(program, "projection");
    scaling = glGetUniformLocation(program, "scaling");
    rotateY = glGetUniformLocation(program, "rotateY");
    rotateZ = glGetUniformLocation(program, "rotateZ");
    translate1 = glGetUniformLocation(program, "translate1");
    translate2 = glGetUniformLocation(program, "translate2");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529, 0.808, 0.980, 1.0);*/
}


color4 defineColor(point4 p) {
    //start with blue then green then yellow then brown and white
    if (p[2] <= -0.6) {
        //navy
        return color4(0.000, 0.000, 0.502, 1.0);
    }
    else if (p[2] > -0.6 && p[2] <= -0.5) {
        return color4(0.000, 0.000, 0.545, 1.0);
    }
    else if (p[2] > -0.5 && p[2] <= -0.4) {
        return color4(0.000, 0.000, 1.000, 1.0);
    }
    else if (p[2] > -0.4 && p[2] < -0.3) {
        return color4(0.118, 0.565, 1.00, 1.0);
    }
    else if (p[2] > -0.3 && p[2] < -0.2) {
        return color4(0.118, 0.565, 1.000, 1.0);
    }
    else if (p[2] > -0.2 && p[2] < 0.0) {
        return color4(0.000, 0.749, 1.000, 1.0);
    }
    else if (p[2] >= 0.0 && p[2] < 0.05) {
        //mocassin
        return color4(0.961, 0.871, 0.702, 1.0);
    }
    else if (p[2] >= 0.05 && p[2] < 0.1) {
        //green yellow
        return color4(0.678, 1.000, 0.184, 1.0);
    }
    else if (p[2] >= 0.1 && p[2] < 0.2) {
        //lime
        return color4(0.196, 0.804, 0.196, 1.0);
    }
    else if (p[2] >= 0.2 && p[2] < 0.3) {
        //medium sea
        return color4(0.180, 0.545, 0.341, 1.0);
    }
    else if (p[2] >= 0.3 && p[2] < 0.4) {
        //forest
        return color4(0.133, 0.545, 0.133, 1.0);
    }
    else if (p[2] >= 0.4 && p[2] < 0.5) {
        //dark green
        return color4(0.000, 0.392, 0.000, 1.0);
    }
    else if (p[2] >= 0.5 && p[2] < 0.6) {
        return color4(0.545, 0.271, 0.075, 1.0);
    }
    else if (p[2] >= 0.6 && p[2] < 0.7) {
        return color4(0.502, 0.000, 0.000, 1.0);
    }
    else if (p[2] >= 0.7 && p[2] < 0.8) {
        return color4(0.545, 0.271, 0.075, 1.0);
    }
    else if (p[2] >= 0.8 && p[2] < 1.0) {
        return color4(1.000, 0.980, 0.980, 1.0);
    }
    else {
        return color4(0.600, 0.196, 0.800, 1.0);
    }
}


color4 defineAmbient(point4 p) {
    //start with blue then green then yellow then brown and white
    if (p[2] <= -0.6) {
        //navy-midnight
        return color4(0.098, 0.098, 0.439);
    }
    else if (p[2] > -0.6 && p[2] <= -0.5) {
        //dark-navy
        return color4(0.000, 0.000, 0.502, 1.0);
    }
    else if (p[2] > -0.5 && p[2] <= -0.4) {
        //blue-medium
        return color4(0.000, 0.000, 0.804, 1.0);
    }
    else if (p[2] > -0.4 && p[2] < -0.3) {
        //dodger-steel
        return color4(0.118, 0.565, 1.000, 1.0);
    }
    else if (p[2] > -0.3 && p[2] < -0.2) {
        //dodger-steel
        return color4(0.118, 0.565, 1.000, 1.0);
    }
    else if (p[2] > -0.2 && p[2] < 0.0) {
        //deep-dodger
        return color4(0.118, 0.565, 1.000, 1.0);
    }
    else if (p[2] >= 0.0 && p[2] < 0.05) {
        //wheat-tan
        return color4(0.824, 0.706, 0.549, 1.0);
    }
    else if (p[2] >= 0.05 && p[2] < 0.1) {
        //green yellow - lawn
        return color4(0.486, 0.988, 0.000, 1.0);
    }
    else if (p[2] >= 0.1 && p[2] < 0.2) {
        //lime-sea
        return color4(0.180, 0.545, 0.341, 1.0);
    }
    else if (p[2] >= 0.2 && p[2] < 0.3) {
        //sea-teal
        return color4(0.000, 0.502, 0.502, 1.0);
    }
    else if (p[2] >= 0.3 && p[2] < 0.4) {
        //forest-green
        return color4(0.000, 0.502, 0.000, 1.0);
    }
    else if (p[2] >= 0.4 && p[2] < 0.5) {
        //dark green
        return color4(0.000, 0.392, 0.000, 1.0);
    }
    else if (p[2] >= 0.5 && p[2] < 0.6) {
        return color4(0.545, 0.271, 0.075, 1.0);
    }
    else if (p[2] >= 0.6 && p[2] < 0.7) {
        return color4(0.502, 0.000, 0.000, 1.0);
    }
    else if (p[2] >= 0.7 && p[2] < 0.8) {
        return color4(0.545, 0.271, 0.075, 1.0);
    }
    else if (p[2] >= 0.8 && p[2] < 1.0) {
        return color4(1.000, 0.980, 0.980, 1.0);
    }
    else {
        return color4(0.600, 0.196, 0.800, 1.0);
    }
}


void triangle(point4 p1, point4 p2, point4 p3) {

    vec3  normal1 = (p1[0], p1[1], p1[2]);
    vec3  normal2 = (p2[0], p2[1], p2[2]);
    vec3  normal3 = (p3[0], p3[1], p3[2]);

    vec3  normal1F = normalize(cross(p3 - p2, p1 - p2));
    vec3  normal2F = normalize(cross(p3 - p2, p1 - p2));
    vec3  normal3F = normalize(cross(p3 - p2, p1 - p2));

    colors_sorted[index] = defineColor(p1);
    color4 ambient = defineAmbient(p1) * light_ambient;
    if (p1[2] < 0) {
        p1[2] = 0;
    }
    vertices_sorted[index] = p1;
    normals_sortedGauraud[index] = normal1;
    normals_sortedFlat[index] = normal1F;
    material_ambient_sorted[index] = ambient;
    color4 diffuse = colors_sorted[index] * light_diffuse;
    material_diffuse_sorted[index] = diffuse;
    color4 specular = colors_sorted[index] * light_specular;
    material_specular_sorted[index] = specular;
    index++;

    colors_sorted[index] = defineColor(p2);
    ambient = defineAmbient(p2);
    if (p2[2] < 0) {
        p2[2] = 0;
    }
    vertices_sorted[index] = p2;
    normals_sortedGauraud[index] = normal2;
    normals_sortedFlat[index] = normal2F;
    material_ambient_sorted[index] = ambient * light_ambient;
    diffuse = colors_sorted[index] * light_diffuse;
    material_diffuse_sorted[index] = diffuse;
    specular = colors_sorted[index] * light_specular;
    material_specular_sorted[index] = specular;
    index++;

    colors_sorted[index] = defineColor(p3);
    ambient = defineAmbient(p3) * light_ambient;
    if (p3[2] < 0) {
        p3[2] = 0;
    }
    vertices_sorted[index] = p3;
    normals_sortedGauraud[index] = normal3;
    normals_sortedFlat[index] = normal3F;
    material_ambient_sorted[index] = ambient;
    diffuse = colors_sorted[index] * light_diffuse;
    material_diffuse_sorted[index] = diffuse;
    specular = colors_sorted[index] * light_specular;
    material_specular_sorted[index] = specular;
    index++;
}

void CreateTriangles() {

    for (int j = row - 1; j > 0; j--) {
        for (int i = 0; i < row - 1; i++) {
            point4 p1 = Vertices[i][j];
            point4 p2 = Vertices[i][j - 1];
            point4 p3 = Vertices[i + 1][j - 1];
            point4 p4 = Vertices[i + 1][j];
            triangle(p1, p2, p3);
            triangle(p1, p3, p4);
        }
    }

}

void diamondSquare(int startX, int endX, int startY, int endY)
{
    int size = endX - startX;
    if (size < 2)
        return;
    int halfX = startX + (size / 2);
    int halfY = startY + (size / 2);

    //square
    //if (Vertices[halfX][halfY][2] == 0) {
    float avg = Vertices[startX][startY][2] + Vertices[startX][endY][2] + Vertices[endX][startY][2] + Vertices[endX][endY][2];
    float noise = avg / 5;
    avg = (avg / 4) + magnitude * myRandom(-noise, noise);
    if (avg > 1)
        avg = myRandom(0.9, 1.0);
    else if (avg < -1)
        avg += -1 * myRandom(0.9, 1.0);
    Vertices[halfX][halfY][2] = avg;
    //}


    //diamond1
    if (Vertices[startX][halfY][2] == 0) {
        float avg1 = Vertices[startX][startY][2] + Vertices[startX][endY][2] + avg;
        float noise1 = avg1 / 5;
        avg1 = (avg1 / 3) + magnitude * myRandom(-noise1, noise1);
        if (avg1 > 1)
            avg1 = myRandom(0.9, 1.0);
        else if (avg1 < -1)
            avg1 = -1 * myRandom(0.9, 1.0);
        Vertices[startX][halfY][2] = avg1;
    }

    //diamond2
    if (Vertices[halfX][startY][2] == 0) {
        float avg2 = Vertices[startX][startY][2] + Vertices[endX][startY][2] + avg;
        float noise2 = avg2 / 5;
        avg2 = (avg2 / 3) + magnitude * myRandom(-noise2, noise2);
        if (avg2 > 1)
            avg2 = myRandom(0.9, 1.0);
        else if (avg2 < -1)
            avg2 = -1 * myRandom(0.9, 1.0);
        Vertices[halfX][startY][2] = avg2;
    }

    //diamond3
    if (Vertices[endX][startY][2] == 0) {
        float avg3 = Vertices[endX][startY][2] + Vertices[endX][endY][2] + avg;
        float noise3 = avg3 / 5;
        avg3 = (avg3 / 3) + magnitude * myRandom(-noise3, noise3);
        if (avg3 > 1)
            avg3 = myRandom(0.9, 1.0);
        else if (avg3 < -1)
            avg3 = -1 * myRandom(0.9, 1.0);
        Vertices[endX][halfY][2] = avg3;
    }

    //diamond4
    if (Vertices[halfX][endY][2] == 0) {
        float avg4 = Vertices[startX][endY][2] + Vertices[endX][endY][2] + avg;
        float noise4 = avg4 / 5;
        avg4 = (avg4 / 3) + magnitude * myRandom(-noise4, noise4);
        if (avg4 > 1)
            avg4 = myRandom(0.9, 1.0);
        else if (avg4 < -1)
            avg4 = -1 * myRandom(0.9, 1.0);
        Vertices[halfX][endY][2] = avg4;
    }
    magnitude = magnitude * pow(2, (-1 * h));
    diamondSquare(startX, halfX, startY, halfY);
    diamondSquare(startX, halfX, halfY, endY);
    diamondSquare(halfX, endX, startY, halfY);
    diamondSquare(halfX, endX, halfY, endY);
}

void fillArrays() {

    //Clculate the number of points then initialize arrays (vertice-sorted and colors-sorted)
    NumOfSubdivisions = 4;
    NumOfSubdivisions += 2; //because we started with 25 points
    NumVertices = 2;
    for (int a = 0; a < NumOfSubdivisions; a++) {
        NumVertices *= 2;
    }
    NumVertices /= 2;
    NumVertices++;
    row = NumVertices;
    NumVertices *= NumVertices;

    Vertices = new point4 * [NumVertices];
    Colors = new color4 * [NumVertices];
    for (int a = 0; a < row; a++) {
        Vertices[a] = new point4[row];
        Colors[a] = new color4[row];
    }
    //Until this line NumVertices expesses the number of different points to create a terrain.
    cout << NumVertices << endl;

    NumVertices = 2;
    for (int a = 0; a < NumOfSubdivisions; a++) {
        NumVertices *= 2;
    }
    NumVertices /= 2;
    int triangle_each_row = NumVertices * 2;
    NumVertices = triangle_each_row * NumVertices;
    //Now NumVertices expesses the number of faces that our terrain has.
    NumVertices *= 3;
    //Now NumVertices expesses the total number of points that we store in arrays to draw triangles.
    cout << "Needed size: " << endl;
    cout << NumVertices << endl;

    //For terrain
    vertices_sorted = new point4[NumVertices];
    size_of_vertices_sorted = NumVertices * sizeof(point4);
    colors_sorted = new color4[NumVertices];
    size_of_colors_sorted = NumVertices * sizeof(color4);
    normals_sortedFlat = new vec3[NumVertices];
    normals_sortedGauraud = new vec3[NumVertices];
    size_of_normals_sorted = NumVertices * sizeof(vec3);

    material_ambient_sorted = new color4[NumVertices];
    size_of_material_ambient_sorted = NumVertices * sizeof(color4);
    material_diffuse_sorted = new color4[NumVertices];
    size_of_material_diffuse_sorted = NumVertices * sizeof(color4);
    material_specular_sorted = new color4[NumVertices];
    size_of_material_specular_sorted = NumVertices * sizeof(color4);

    //initialize variables for terrain 
    index = 0;
    counter = 0;

    int mid = (row - 1) / 2;
    float y = 2.0;
    float n = 2 * pow((row - 1), -1);

    for (int i = 0; i < row; i++) {
        float x = -2.0;
        for (int j = 0; j < row; j++) {
            Vertices[i][j] = point4(x, y, 0.0, 1.0);
            x = x + (2 * n);
        }
        y = y - (2 * n);
    }

    //peaks
    GLfloat h1 = myRandom(0.0, 0.5);
    GLfloat h2 = myRandom(0.5, 0.7);
    GLfloat h3 = myRandom(0.7, 0.95);
    //deep
    GLfloat h4 = myRandom(-0.0, -0.2);
    GLfloat h5 = myRandom(-0.2, -0.3);
    GLfloat h6 = myRandom(-0.3, -0.4);
    GLfloat h7 = myRandom(-0.4, -0.6);
    GLfloat h8 = myRandom(-0.6, -0.7);
    GLfloat h9 = myRandom(-0.7, -0.8);

    /*mid = (int)((row - 1) / 2);
    Vertices[0][0][2] = h6;
    Vertices[0][mid][2] = h5;
    Vertices[0][row - 1][2] = h6;

    Vertices[mid][0][2] = h4;
    Vertices[mid][mid][2] = h2;
    Vertices[mid][row - 1][2] = h3;

    Vertices[row - 1][0][2] = h4;
    Vertices[row - 1][mid][2] = h5;
    Vertices[row - 1][row - 1][2] = h6;

    magnitude = 0.5;
    h = 0.5;
    diamondSquare(0, mid, 0, mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid, row - 1, mid, row - 1);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(0, mid, mid, row - 1);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid, row - 1, 0, mid);*/

    mid = (int)((row - 1) / 2);
    int m_mid = (int)(mid / 2);
    Vertices[0][0][2] = h9;
    Vertices[0][m_mid][2] = h8;
    Vertices[0][mid][2] = h7;
    Vertices[0][mid + m_mid][2] = h8;
    Vertices[0][row - 1][2] = h9;

    Vertices[m_mid][0][2] = h9;
    Vertices[m_mid][m_mid][2] = h3;
    Vertices[m_mid][mid][2] = h1;
    Vertices[m_mid][mid + m_mid][2] = h8;
    Vertices[m_mid][row - 1][2] = h9;

    Vertices[mid][0][2] = h8;
    Vertices[mid][m_mid][2] = h9;
    Vertices[mid][mid][2] = h6;
    Vertices[mid][mid + m_mid][2] = h2;
    Vertices[mid][row - 1][2] = h8;

    Vertices[mid + m_mid][0][2] = h8;
    Vertices[mid + m_mid][m_mid][2] = h1;
    Vertices[mid + m_mid][mid][2] = h2;
    Vertices[mid + m_mid][mid + m_mid][2] = h8;
    Vertices[mid + m_mid][row - 1][2] = h9;

    Vertices[row - 1][0][2] = h9;
    Vertices[row - 1][m_mid][2] = h8;
    Vertices[row - 1][mid][2] = h7;
    Vertices[row - 1][mid + m_mid][2] = h7;
    Vertices[row - 1][row - 1][2] = h8;


    diamondSquare(0, m_mid, 0, m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(0, m_mid, m_mid, mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(0, m_mid, mid, mid + m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(0, m_mid, mid + m_mid, row - 1);

    magnitude = 0.5;
    h = 0.5;
    diamondSquare(m_mid, mid, 0, m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(m_mid, mid, m_mid, mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(m_mid, mid, mid, mid + m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(m_mid, mid, mid + m_mid, row - 1);

    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid, mid + m_mid, 0, m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid, mid + m_mid, m_mid, mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid, mid + m_mid, mid, mid + m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid, mid + m_mid, mid + m_mid, row - 1);

    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid + m_mid, row - 1, 0, m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid + m_mid, row - 1, m_mid, mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid + m_mid, row - 1, mid, mid + m_mid);
    magnitude = 0.5;
    h = 0.5;
    diamondSquare(mid + m_mid, row - 1, mid + m_mid, row - 1);

    CreateTriangles();

    /// <summary>
    /// this code is necessary for display our terrarin in front of us
    /// </summary>
    x_max = 1.0;
    y_max = 1.0;
    z_max = 2.0;
    x_min = -1.0;
    y_min = -1.0;
    z_min = 0.0;

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
    //scaling_rate = 1 / maxx;
    scaling_rate = 1 / 1;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    commWShader();
}

void init()
{
    light_position = (0.0, 0.0, 5.0, 1.0);
    light_ambient = (0.2, 0.2, 0.2, 1.0);
    light_diffuse = (0.0, 1.0, 0.0, 1.0);
    light_specular = (1.0, 1.0, 1.0, 1.0);
    fillArrays();
    glutPostRedisplay();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    point4  eye(viewer[0], viewer[1], viewer[2], 1.0);
    point4  at(look_at[0], look_at[1], look_at[2], 1.0);
    vec4    up(upp[0], upp[1], upp[2], 0.0);

    mat4  mv = LookAt(eye, at, up);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p);

    mat4  scale = Scale(scaling_rate, scaling_rate, scaling_rate);
    glUniformMatrix4fv(scaling, 1, GL_TRUE, scale);

    mat4  rt = RotateY(degree_y);
    glUniformMatrix4fv(rotateY, 1, GL_TRUE, rt);

    mat4  rt2 = RotateZ(degree_z);
    glUniformMatrix4fv(rotateZ, 1, GL_TRUE, rt2);

    mat4  tr1 = Translate(translate_x, translate_y, translate_z);
    glUniformMatrix4fv(translate1, 1, GL_TRUE, tr1);

    mat4  tr2 = Translate(-1 * translate_x, -1 * translate_y, -1 * translate_z);
    glUniformMatrix4fv(translate2, 1, GL_TRUE, tr2);

    glUniform1i(shading_mode, shadingMode);

    if (drawingMode == 0)
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    else if (drawingMode == 1)
        glDrawArrays(GL_LINES, 0, NumVertices);
    else
        glDrawArrays(GL_POINTS, 0, NumVertices);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{

    if (key == 'x') {
        viewer[0] -= 0.05;
    }
    else if (key == 'X') {
        viewer[0] += 0.05;
    }
    else if (key == 'y') {
        viewer[1] -= 0.05;
    }
    else if (key == 'Y') {
        viewer[1] += 0.05;
    }
    else if (key == 'z') {
        viewer[2] -= 0.01;
    }
    else if (key == 'Z') {
        viewer[2] += 0.01;
    }
    else if (key == 'v' || key == 'V') {
        drawingMode = 2;
    }
    else if (key == 'f' || key == 'F') {
        if (shadingMode == 0) {
            shadingMode = 1;
        }
        else if (shadingMode == 2 || shadingMode == 1) {
            shadingMode = 0;
        }
    }
    else if (key == 'B' || key == 'b') {
        shadingMode = 2;
    }
    else if (key == 's') {
        scaling_rate *= 0.5;
    }
    else if (key == 'S') {
        scaling_rate *= 2.0;
    }
    else if (key == 'q' || key == 'Q') {
        free(vertices_sorted);
        free(colors_sorted);
        free(normals_sortedFlat);
        free(normals_sortedGauraud);

        free(material_ambient_sorted);
        free(material_specular_sorted);
        free(material_diffuse_sorted);
        for (int i = 0; i < row; i++) {
            free(Vertices[i]);
            free(Colors[i]);
        }
        free(viewer);
        free(look_at);
        free(upp);
        free(look_at_plane);
        free(plane);
        _CrtDumpMemoryLeaks();
        exit(EXIT_SUCCESS);
    }
    else if (key == 'R' || key == 'r') {
        viewer[0] -= 0.1;
        look_at[0] += 0.1;
    }
    else if (key == 'L' || key == 'l') {
        viewer[0] += 0.1;
        look_at[0] += 0.1;
    }
    else if (key == 'w' || key == 'W') {
        viewer[0] = 0.0;
        viewer[1] = 0.0;
        viewer[2] = 1.0;
        degree_y = 0;
        degree_z = 0;
        look_at[0] = 0.0;
        look_at[1] = 0.0;
        viewingMode = 0;
    }
    else if (key == 't' || key == 'T') {
        viewer[0] = plane[0];
        viewer[1] = plane[1];
        viewer[2] = plane[2];
        degree_y = 0;
        degree_z = 0;
        look_at[0] = look_at_plane[0];
        look_at[1] = look_at_plane[1];
        degree_z = degree_z_plane;
        viewingMode = 1;
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
        if (viewingMode == 0) {
            viewer[0] += 0.1;
            look_at[0] += 0.1;
        }
        else
            degree_z_plane -= 2;

    }
    else if (key == GLUT_KEY_LEFT) {
        if (viewingMode == 0) {
            viewer[0] -= 0.1;
            look_at[0] -= 0.1;
        }
        else
            degree_z_plane += 2;
    }
    else if (key == GLUT_KEY_UP) {
        viewer[1] += 0.1;
        look_at[1] += 0.1;
        if (viewingMode > 0) {
            plane[1] = viewer[1];
            look_at_plane[1] = look_at[1];
        }
    }
    else if (key == GLUT_KEY_DOWN) {
        viewer[1] -= 0.1;
        look_at[1] -= 0.1;
        if (viewingMode > 0) {
            plane[1] = viewer[1];
            look_at_plane[1] = look_at[1];
        }
    }
    if (viewingMode > 0)
        degree_z = degree_z_plane;
    glutPostRedisplay();

}
int main(int argc, char* argv[])
{
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(512, 512);
    glutCreateWindow("HW3");
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

