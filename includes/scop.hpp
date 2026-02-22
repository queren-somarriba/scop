#pragma once

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>

#include <GL/freeglut.h>

struct Vector3f
{
	float x;
	float y;
	float z;
};

struct Matrix4f
{
	float m[4][4];
};//convertir en class et implementer op