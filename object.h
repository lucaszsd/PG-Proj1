#include <vector>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include "lodepng.h"

#define min_x 0
#define max_x 1
#define min_y 2
#define max_y 3
#define min_z 4
#define max_z 5

#define M_PI 3.141592653589793

struct Vertex{
	vec3 pos;
	vec3 normal;
	vec2 coordText;
};

struct Triangle {

	Vertex vertex[3];
	

	Triangle( const std::vector<vec3> v, std::vector<vec3> n, const std::vector<vec2> t)
	{
		vertex[0].pos = v[0];
		vertex[1].pos = v[1];
		vertex[2].pos = v[2];

		//vec3 normal = cross(vertex[1].pos = v[1] - vertex[0].pos = v[0], vertex[2].pos = v[2] - vertex[0].pos = v[0]);
	
		vertex[0].normal = n[0];
		vertex[1].normal = n[1];
		vertex[2].normal = n[2];

		vertex[0].coordText = t[0];
		vertex[1].coordText = t[1];
		vertex[2].coordText = t[2];

		
	}

	

	~Triangle(){}
};

class Mesh 
{
public:
	std::vector<Triangle> tris;

	Mesh() {}
	~Mesh() {}

	

	

	bool load_mesh_from_file(const char* path) 
	{
		tris.clear();
		std::vector< unsigned int > vertexIndices;

		//Revisar

		std::vector< unsigned int > normalIndices;
		std::vector< unsigned int > textureIndices;
		std::vector< vec3 > temp_vertices;
		std::vector< vec3 > temp_normal;
		std::vector< vec2 > temp_texture;
		//End-Revisar


		std::ifstream f(path);
		if (!f.is_open())
		{
			std::cout << "File cannot be oppened or does not exist\n";
			return false;
		}

		std::cout << "file was  oppened!\n";

		
		while (!f.eof())
		{
			char line[256];
			f.getline(line, 256);

			std::stringstream s;
			s << line;

			char junk;

			if ( line[0] == 'v')
			{
				if (line[1] == 't') 

				{
					vec2 vertex;
					char junk2;
					s >> junk >> junk2 >> vertex[0] >> vertex[1];
					temp_texture.push_back(vertex);
				}
				else if (line[1] == 'n') 
				{
					vec3 vertex;
					char junk2;
					s >> junk >> junk2 >> vertex[0] >> vertex[1] >> vertex[2];
					temp_normal.push_back(vertex);
				}
				else {
					vec3 vertex;
					s >> junk >> vertex[0] >> vertex[1] >> vertex[2];
					temp_vertices.push_back(vertex);
				}
			}

			else if ( line[0] == 'f')
			{
				/*
				
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3];
				unsigned int normalIndex[3];
				unsigned int textureIndex[3];

				s >> junk >> vertex1 >> vertex2 >> vertex3;
				
				//Revisar

				int fstslash = vertex1.find("/");
				std::string fst = vertex1.substr(0, fstslash);
				vertexIndex[0] = atoi( fst.c_str() );

				fstslash = vertex2.find("/");
				fst = vertex2.substr(0, fstslash);
				vertexIndex[1] = atoi(fst.c_str());

				fstslash = vertex3.find("/");
				fst = vertex3.substr(0, fstslash);
				vertexIndex[2] = atoi(fst.c_str());
				//End Revisar

					
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

				textureIndices.push_back(textureIndex[0]);
				textureIndices.push_back(textureIndex[1]);
				textureIndices.push_back(textureIndex[2]);
				*/

				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3];
				unsigned int normalIndex[3];
				unsigned int textureIndex[3];

				s >> junk >> vertex1 >> vertex2 >> vertex3;

				std::vector<int> aux1;
				std::stringstream ss1(vertex1);
				for (int i; ss1 >> i;) {
					aux1.push_back(i);
					aux1.push_back(i);
					if (ss1.peek() == '/')
						ss1.ignore();
				}

				std::vector<int> aux2;
				std::stringstream ss2(vertex2);
				for (int i; ss2 >> i;) {
					aux2.push_back(i);
					aux2.push_back(i);
					if (ss2.peek() == '/')
						ss2.ignore();
				}

				std::vector<int> aux3;
				std::stringstream ss3(vertex3);
				for (int i; ss3 >> i;) {
					aux3.push_back(i);
					aux3.push_back(i);
					if (ss3.peek() == '/')
						ss3.ignore();
				}

				vertexIndex[0] = aux1[0];
				textureIndex[0] = aux1[1];
				normalIndex[0] = aux1[2];

				vertexIndex[1] = aux2[0];
				textureIndex[1] = aux2[1];
				normalIndex[1] = aux2[2];

				vertexIndex[2] = aux3[0];
				textureIndex[2] = aux3[1];
				normalIndex[2] = aux3[2];


				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

				textureIndices.push_back(textureIndex[0]);
				textureIndices.push_back(textureIndex[1]);
				textureIndices.push_back(textureIndex[2]);

			}
		}

		for (unsigned int i = 0; i < vertexIndices.size(); i+=3)
		{
			unsigned int v1 = vertexIndices[i];
			unsigned int v2 = vertexIndices[i+1];
			unsigned int v3 = vertexIndices[i+2];

			std::vector<vec3> vertices;
			vertices.push_back(temp_vertices[v1 - 1]);
			vertices.push_back(temp_vertices[v2 - 1]);
			vertices.push_back(temp_vertices[v3 - 1]);
			
			v1 = normalIndices[i];
			v2 = normalIndices[i + 1];
			v3 = normalIndices[i + 2];

			std::vector<vec3> normais;
			normais.push_back(temp_normal[v1 - 1]);
			normais.push_back(temp_normal[v2 - 1]);
			normais.push_back(temp_normal[v3 - 1]);
			
			v1 = textureIndices[i];
			v2 = textureIndices[i + 1];
			v3 = textureIndices[i + 2];
			
			std::vector<vec2> textures;
			textures.push_back(temp_texture[v1 - 1]);
			textures.push_back(temp_texture[v2 - 1]);
			textures.push_back(temp_texture[v3 - 1]);
			

			Triangle t(vertices, normais, textures);
			tris.push_back(t);
		}

		std::cout << "vertSize = " << vertexIndices.size() << "\n";
		return true;
	}
};

class Obj 
{
public:
	Mesh mesh;

	Obj(){}
	
	Obj(const char* file_path, const char* texture_path) {
		mesh.load_mesh_from_file(file_path);
		decodeOneStep(texture_path);
	}
	
	Obj( const char* file_path ){
		mesh.load_mesh_from_file(file_path); 
	}
	
	~Obj(){}

	std::vector<vec3> texture_buffer;
	int texture_width, texture_height;

	void decodeOneStep(const char* file_path) {
		std::vector<unsigned char> png;
		std::vector<unsigned char> image; //the raw pixels
		unsigned width, height;
		lodepng::State state; //optionally customize this one

		unsigned error = lodepng::load_file(png, file_path); //load the image file with given file_path
		if (!error) error = lodepng::decode(image, width, height, state, png);

		//if there's an error, display it
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
		texture_buffer.reserve((int)width * (int)height);
		for (int i = 0; i < image.size(); i += 4)
			texture_buffer.push_back(vec3(float(image[i]), float(image[i + 1]), float(image[i + 2])));

		texture_width = (int)width;
		texture_height = (int)height;
	}

};