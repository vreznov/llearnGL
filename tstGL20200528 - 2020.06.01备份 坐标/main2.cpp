#include "glad/glad.h"
#include "GLFW/glfw3.h"  //最后包含
#include <math.h>
#include <fstream>
#include "kshader.h"
#include <iostream>


// GLM测试
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;
using namespace glm;

int main2(int argc, char *argv[])
{
    vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    mat4 trans(1.0f);
    trans = translate(trans, vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;

    cout << vec.x << " " << vec.y << " " << vec.z << endl;
    return  0;
}
