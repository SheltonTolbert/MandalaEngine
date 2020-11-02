#include <iostream>
#include "olcConsoleGameEngine.h"

struct vec3d
{
	float x , y, z; 
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	std :: vector<triangle> tris; 
};

struct mat4x4
{
	float m[4][4] = { 0 };
};



class pluckyEngine3D : public olcConsoleGameEngine
{
public: 
	pluckyEngine3D()
	{
		m_sAppName = L"3D Demo";
	}

private:
		mesh meshCube;
		mat4x4 matProj;
		float fTheta; 

		void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
		{
			o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
			o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
			o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
			float  w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
			
			// Divide by the fourth dimension variable
			if (w != 0)
			{
				o.x /= w; 
				o.y /= w; 
				o.z /= w; 
			}
			
		}

public: 
	bool OnUserCreate() override
	{
		meshCube.tris =
		{
			// South Face +Z
			{0.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f,},
			{0.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,},
			// East Face +X
			{1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f,},
			{1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 1.0f,},
			// North Face -Z
			{1.0f, 0.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 1.0f,},
			{1.0f, 0.0f, 1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,},
			// West Face -X
			{0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,},
			{0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f,},
			// Top Face +Y
			{0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f,},
			{0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 0.0f,},
			// Bottom Face -Y
			{1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,},
			{1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,},
		};

		// Populate projection matrix
		float fNear = 0.1f; 
		float fFar = 1000.0f; 
		float fFov = 90.0f; 
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);
		
		matProj.m[0][0] = fAspectRatio * fFovRad; 
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear); 
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f; 
		matProj.m[3][3] = 0.0f;


		return true;
	}
 

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear the screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		
		// Rotation matricies
		mat4x4 matRotZ, matRotX; 
		fTheta += 1.0f *fElapsedTime; 

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta *0.5f);
		matRotX.m[1][2] = sinf(fTheta *0.5f);
		matRotX.m[2][1] = -sinf(fTheta *0.5f);
		matRotX.m[2][2] = cosf(fTheta *0.5f);
		matRotX.m[3][3] = 1;





		// Draw triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX; 
			// Rotate on the Z axis
			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);
			// Rotate on the X axis
			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);



			// Translation should be the last operation
			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;


			MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
			MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
			MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

			// Apply Scalar 
			// Screen Origin 0,0 == top left of the screen
			// So we need to find the center point of the screen where
			// Screenwidth() / 2 == midpoint.x 
			// and ScreenHeight() / 2 == midpoint.y
			
			// Here we take normalized Vector (0 - 1) and add one to it (1 - 2
			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

			triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

			DrawTriangle(
				triProjected.p[0].x, triProjected.p[0].y, 
				triProjected.p[1].x, triProjected.p[1].y, 
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_SOLID, FG_WHITE);

		}
		return true;
	}
};



int main()
{
	pluckyEngine3D demo; 
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start(); 
	return 0; 
}

