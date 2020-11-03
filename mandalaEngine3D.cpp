#include <iostream>
#include "olcConsoleGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>

struct vec3d
{
	float x , y, z; 
};

struct triangle
{
	vec3d p[3];
	wchar_t sym;
	short color; 
};

struct mesh
{
	std :: vector<triangle> tris; 

	bool loadFromObjectFile(std :: string sFilename)
	{
		std :: ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// load verts
		std::vector<vec3d> verts; 

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 218);
			
			std :: strstream s; 
			s << line;

			char tmp; 

			if (line[0] == 'v')
			{
				vec3d v; 
				s >> tmp >> v.x >> v.y >> v.z; 
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> tmp >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}


		return true; 
	}
};

struct mat4x4
{
	float m[4][4] = { 0 };
};



class mandalaEngine3D : public olcConsoleGameEngine
{

// Public Methods
public: 
	
	// Name of the app
	mandalaEngine3D()
	{
		m_sAppName = L"Mandala 3D";
	}






// Private Methods
private:

	// Global variables
		mesh meshCube; // Mesh for testing purposes 
		mat4x4 matProj; // Projection matrix variable
		vec3d vCamera; // The position of the camera

		float fTheta; 


		void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
		{
			o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
			o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
			o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
			float  w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
			
			// Divide by the fourth dimension variable
			if (w != 0.0f)
			{
				o.x /= w; 
				o.y /= w; 
				o.z /= w; 
			}
			
		}

		// Normalizes a vector to a length of 1 (unit vector)
		void Normalize(vec3d &v)
		{
			float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
			v.x /= len; v.y /= len; v.z /= len;
		}

		// Returns the dot product of two vectors
		float DotProduct(vec3d &v1, vec3d &v2)
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		}

		// Convert a greyscale to supported color
		CHAR_INFO GetColor(float lum)
		{
			short bg_col, fg_col;
			wchar_t sym;
			int pixel_bw = (int)(13.0f*lum);
			switch (pixel_bw)
			{
			case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

			case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
			case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
			case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
			case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

			case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
			case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
			case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
			case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

			case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
			case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
			case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
			case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
			default:
				bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
			}

			CHAR_INFO c;
			c.Attributes = bg_col | fg_col;
			c.Char.UnicodeChar = sym;
			return c;
		}

		void renderRaster(std::vector<triangle> &rasterBuffer)
		{
			for (auto &triProjected : rasterBuffer)
			{
				// Draw Faces
				FillTriangle(
					triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					triProjected.sym, triProjected.color);
				// Draw Wireframe over the faces
				DrawTriangle(
					triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					PIXEL_SOLID, FG_BLACK);
			}
		}

		void sortTriangles(std::vector<triangle> &rasterBuffer)
		{
			std::sort(rasterBuffer.begin(), rasterBuffer.end(), [](triangle &t1, triangle &t2)
			{
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 < z2;
			});

		}







public:







	// Application initializer
	bool OnUserCreate() override
	{
		//meshCube.tris =
		//{
		//	// South Face +Z
		//	{0.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f,},
		//	{0.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,},
		//	// East Face +X
		//	{1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f,},
		//	{1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 1.0f,},
		//	// North Face -Z
		//	{1.0f, 0.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 1.0f,},
		//	{1.0f, 0.0f, 1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,},
		//	// West Face -X
		//	{0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,},
		//	{0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f,},
		//	// Top Face +Y
		//	{0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,},
		//	{0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 0.0f,},
		//	// Bottom Face -Y
		//	{1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,},
		//	{1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,},
		//};

		meshCube.loadFromObjectFile("./primitives/cube.obj");


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
 








	// Per frame update 
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear the screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		
		// Rotation matricies
		mat4x4 matRotZ, matRotX; 
		fTheta += 1.0f * fElapsedTime; 

		// Filling rotation matrix Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;
	
		// Rotation matrix X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta *0.5f);
		matRotX.m[1][2] = sinf(fTheta *0.5f);
		matRotX.m[2][1] = -sinf(fTheta *0.5f);
		matRotX.m[2][2] = cosf(fTheta *0.5f);
		matRotX.m[3][3] = 1;

		std::vector<triangle> rasterBuffer; // A data structure that will hold triangles to later be rendered in the Raster phase 

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
			// Calculate normals
			vec3d normal, line1, line2; 
			// Use cross product here
			line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
			line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
			line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

			line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
			line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
			line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y; 
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;
			
			// Normalize the normal
			/*float len = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			normal.x /= len; normal.y /= len; normal.z /= len;*/
			Normalize(normal);


			// Calculate Dot product of face and camera
			// It it is less than 0, draw the face
			if (normal.x * (triTranslated.p[0].x - vCamera.x)+ 
				normal.y * (triTranslated.p[0].y - vCamera.y)+ 
				normal.z * (triTranslated.p[0].z - vCamera.z) < 0)
			{
				// Illumination
				vec3d light_direction = { 0.0f, 0.0f, -1.0f };
				Normalize(light_direction); 
				float dp = DotProduct(normal, light_direction); 
				
				// Converting to the Console Game Engine color pallet 
				CHAR_INFO c = GetColor(dp);
				triTranslated.color = c.Attributes;
				triTranslated.sym = c.Char.UnicodeChar; 


				// Project triangles from 2D to 3D via the projection matrix
				MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
				MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
				MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);
				triProjected.color = triTranslated.color;
				triProjected.sym = triTranslated.sym;


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
				
				// Add each triangle to the raster buffer
				rasterBuffer.push_back(triProjected);				
			}

		}

		sortTriangles(rasterBuffer); 
		renderRaster(rasterBuffer);


		return true;
	}
};



int main()
{
	mandalaEngine3D demo; 
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start(); 
	return 0; 
}

