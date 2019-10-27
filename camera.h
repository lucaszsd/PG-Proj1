#ifndef CAMERAH
#define CAMERAH

#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include "object.h"

//#include <math.h>

#include <SDL.h>

std::vector<Obj> objects;

const int WIDTH = 600;
const int HEIGHT = 400;

const static int INSIDE = 0;
const static int LEFT = 1;
const static int RIGHT = 2;
const static int BOTTOM = 4;
const static int TOP = 8;

//Revisar
constexpr float kEpsilon = 1e-8;
const int x_max = 600;
const int y_max = 400;
const int x_min = 0;
const int y_min = 0;

float Zbuffer[600 * 400];

#define M_PI 3.141592653589793

// End Revisar


class camera
{
public:
	int imgWidth, imgHeight;
	float fov, _near, _far, aspectRatio;
	float bottom, left, top, right;
	matrix44 camToWorld;
	matrix44 worldToCamera;
	matrix44 matrixMap;

	vec3 _from, _at, _up;
	vec3 axisX, axisY, axisZ;

public:
	camera();
	camera(const vec3& from, const vec3& at, const vec3& up,
		const float& f, const float& n,
		const int& iwidth, const int& iheight) :
		fov(f), _near(n), imgWidth(iwidth), imgHeight(iheight),
		_from(from), _at(at), _up(up)
	{
		top = std::tan((fov / 2) * 3.14 / 180);
		aspectRatio = imgWidth / (float)imgHeight;
		right = top * aspectRatio;
		left = -right;
		bottom = -top;
		_far = 10000;

		look_at(from, at, up);
	}

	

	
	

	//Revisar

	int GetOutcode(vec2& ponto, int x_min, int x_max, int y_min, int y_max) {

		int return_val = INSIDE;

		if (ponto.y() > y_max) {
			return_val |= TOP;
		}

		if (ponto.y() < y_min) {
			return_val |= BOTTOM;
		}

		if (ponto.x() > x_max) {
			return_val |= RIGHT;
		}

		if (ponto.x() < x_min) {
			return_val |= LEFT;
		}

		return return_val;
	}

	bool ClipLine(vec2& p0, vec2& p1) {

		int outcode0 = GetOutcode(p0, 0, imgWidth, 0, imgHeight);
		int outcode1 = GetOutcode(p1, 0, imgWidth, 0, imgHeight);

		float slope = (p1.y() - p0.y()) / (p1.x() - p0.x());
		float x;
		float y;
		/**
		float x = p0.x() + (1.0 / slope) * (0 - p0.y());
		float y = p0.y() + slope*()
		*/

		bool acept = false;

		while (true) {

			if (outcode0 == 0 && outcode1 == 0) {
				acept = true;
				return true;
			}
			else if (outcode0 & outcode1) {
				return false;
			}

			else {
				int outcodeOutside = outcode1 != 0 ? outcode1 : outcode0;

				if (outcodeOutside & TOP) {
					y = p0.y() + slope * (imgHeight - p0.y());
					//x = p0.x() + (1.0 / slope) * (imgWidth - p0.y());
				}

				else if (outcodeOutside & BOTTOM) {
					y = p0.y() + slope * (0 - p0.y());
					//x = p0.x() + (1.0 / slope) * (imgWidth - p0.y());
				}

				else if (outcodeOutside & RIGHT) {
					x = p0.x() + (1.0 / slope) * (imgWidth - p0.y());
				}

				else if (outcodeOutside & LEFT) {
					x = p0.x() + (1.0 / slope) * (0 - p0.y());
				}

				if (outcodeOutside == outcode0) {
					p0[0] = x;
					p0[1] = y;
					outcode0 = GetOutcode(p0, 0, imgWidth, 0, imgHeight);
				}
				else {
					p1[0] = x;
					p1[1] = y;
					outcode1 = GetOutcode(p1, 0, imgWidth, 0, imgHeight);
				}
			}

		}
		return acept;
	}

	void DrawLine(SDL_Renderer* renderer, vec2& p0, vec2& p1) {

		vec2 vetor;
		vetor.e[0] = p1[0] - p0[0];
		vetor.e[1] = p1[1] - p0[1];
		int tamanho = (int)vetor.length();
		vetor.make_unit_vector();

		vec2 point(p0[0], p0[1]);
		for (int i = 0; i < tamanho; i++)
		{
			SDL_RenderDrawPoint(renderer, point.x(), point.y());
			point.e[0] += vetor.x();
			point.e[1] += vetor.y();
		}

	}

	int value_at(int x, int y)
	{
		return y * WIDTH + x;
	}

	void reload() {
		std::fill(std::begin(Zbuffer), std::begin(Zbuffer) + 240000, 5000);
	}

	float sign(vec2 p1, vec2 p2, vec2 p3)
	{
		vec2 p = p2 - p1;
		vec2 r = p3 - p1;
		return (p.x() * r.y()) - (r.x() * p.y());
	}

	vec3 phong(vec3 normal, vec3 Kd, vec3 Ks, float n, Obj obj, vec2 textura) {


		vec3 raioDeLuz(0.0f, 0.0f, -1.0f);
		vec3 componenteAmbiente(40, 40, 40);
		float cos0 = std::max(0.0f, -dot(unit_vector(normal), raioDeLuz));
		vec3 componenteDifusa = cos0 * Kd;
		vec3 R = raioDeLuz - 2 * (dot(normal, raioDeLuz)) * raioDeLuz;
		vec3 componenteEspecular = Ks * (pow(std::max(dot(axisZ, R), 0.0f), n));
		float aux = textura.y();
		aux = 1 - aux;
		int y = obj.texture_height * aux;
		int x = obj.texture_width * textura.x();
		vec3 cor = obj.texture_buffer[y * obj.texture_width + x];
		//std::cout << cor << '\n';

		return componenteDifusa + componenteAmbiente + componenteEspecular;
		// return obj.texture_buffer[y*obj.texture_width+x];

	}

	void fill_triangle(SDL_Renderer* renderer, const vec2& v0, const vec2& v1, const vec2& v2, const Triangle& tr, const vec3& light_dir) {

		vec2 praster1 = v0;
		vec2 praster2 = v1;
		vec2 praster3 = v2;

		//Para calcular o phong shading
		vec3 A;
		vec3 B;
		vec3 C;
		vec3 normal;
		vec3 raio = light_dir;

		vec3 V = -raio + 5;
		V.make_unit_vector();
		vec3 R;
		vec3 especular;



		//DIFUSA
		vec3 AB = B - A;

		vec3 AC = C - A;

		normal = cross(AB, AC);
		normal.make_unit_vector();
		raio.make_unit_vector();

		//float cosO = std::max (0.0f,dot(normal, light_dir));
		vec3 cosO = dot(normal, raio);

		//ESPECULAR
		R = raio - 2 * (dot(normal, raio)) * normal;
		R.make_unit_vector();

		float ks = 0.08;
		vec3 produto = dot(V, R);
		//vec3 exp = pow(produto, 8);

		especular = ks * (dot(V, R));

		vec3 I = 0.5 * 0.3 + cosO + especular;

		//=======================

		vec2 vproj[] = { v0, v1, v2 };

		int maxX = 0;
		int minX = imgHeight;
		int maxY = 0;
		int minY = imgWidth;

		vec2 ponto;





		vec2 bbmin = vec2(imgHeight, imgWidth);
		vec2 bbmax = vec2(0, 0);

		//v1 - v0 p- v0

		vec2 v0v1 = v1 - v0;
		vec2 v1v2 = v2 - v1;
		vec2 v2v0 = v0 - v2;

		vec2 v0p = ponto - v0;
		vec2 v1p = ponto - v1;
		vec2 v2p = ponto - v2;


		//float wa1 = v0.x() * ponto.y() - v0.y() * ponto.x();
		//float wa2 = v1.x() * ponto.y() - v1.y() * ponto.x();
		//float wa3 = v2.x() * ponto.y() - v2.y() * ponto.x();



		float wa1;
		float wa2;
		float wa3;

		for (auto& object : objects) {
			for (auto& tr : object.mesh.tris) {

				A = tr.vertex[0].pos;
				B = tr.vertex[1].pos;
				C = tr.vertex[2].pos;



				wa1 = v0v1.x() * v0p.y() - v0v1.y() * v0p.x();
				wa2 = v1v2.x() * v1p.y() - v1v2.y() * v1p.x();
				wa3 = v2v0.x() * v2p.y() - v2v0.y() * v2p.x();
			}
		}


		for (int i = 0; i < 3; i++) {

			//vproj[i] = projectAndConvertToNDC(triangle[i].v[i]);
			//vproj[i].x() = imgWidth;
			//vproj[i].y() = imgHeight;

			if (vproj[i].x() < bbmin[0]) {

				bbmin[0] = vproj[i].x();
			}

			if (vproj[i].y() < bbmin.y()) {

				bbmin[1] = vproj[i].y();
			}

			if (vproj[i].x() > bbmax.x()) {

				bbmax[0] = vproj[i].x();
			}

			if (vproj[i].y() > bbmax.y()) {

				bbmax[1] = vproj[i].y();
			}

			//ponto = (vproj[i].x(), vproj[i].y());



		}


		for (int y = bbmin[1]; y <= bbmin[1]; ++y) {
			for (int x = bbmin[0]; x <= bbmax[0]; ++x) {

				ponto = vec2(bbmin[x], bbmin[y]);

				if (wa1 >= 0 && wa2 >= 0 && wa3 >= 0) {



					vec3 color(210, 510, 300);
					//vec3 color = I;
					SDL_SetRenderDrawColor(renderer, color.r(), color.g(), color.b(), 255);
					//SDL_RenderDrawPoint(renderer, vproj[x].x(), vproj[y].y());
					//SDL_RenderDrawPoint(renderer, vproj[x].x(), vproj[y].y());
					SDL_RenderDrawPoint(renderer, ponto.x(), ponto.y());
				}
			}
		}

	}

	void look_at(const vec3& from, const vec3& at, const vec3& up)
	{
		axisZ = from - at;
		axisZ.make_unit_vector();

		axisY = up - ((dot(up, axisZ) / dot(axisZ, axisZ)) * axisZ);
		axisY.make_unit_vector();

		axisX = cross(axisY, axisZ);
		axisX.make_unit_vector();

		camToWorld = matrix44({ axisX.x(), axisX.y(), axisX.z(), 0,
		  axisY.x(), axisY.y(), axisY.z(), 0,
		  axisZ.x(), axisZ.y(), axisZ.z(), 0,
		  from.x(), from.y(), from.z(), 1 });
		worldToCamera = camToWorld.inverse();

	}

	bool compute_pixel_coordinates(const vec3& pWorld, vec2& pRaster)
	{
		vec3 pScreen, pProj, pNDC;
		vec2 pRas;
		vec3 raio = pWorld - _from;
		raio.make_unit_vector();

		if (dot(raio, axisZ) > 0) {
			return false;
		}

		worldToCamera.mult_point_matrix(pWorld, pScreen);

		pProj = vec3(pScreen.x() * _near / pScreen.z(), pScreen.y() * _near / pScreen.z(), _near);

		matrixMap = matrix44({ 2 * (_near / (right - left)), 0, 0, 0,
							0, 2 * (_near / (bottom - top)), 0, 0,
							-(right + left) / (right - left), -(bottom + top) / (bottom - top), (_far + _near) / (_far - _near), 1,
							0, 0, -(2 * _near) / (_far - _near), 0 });

		matrixMap.mult_point_matrix(pProj, pNDC);


		pRaster[0] = (1 + pNDC.x()) / 2 * imgWidth;
		pRaster[1] = (1 - pNDC.y()) / 2 * imgHeight;

		//pRaster = vec2((1 + pNDC.x()) / 2 * imgWidth, (1 - pNDC.y()) / 2 * imgHeight);

		//pScreen[0]

		return true; // Retornar verdadeiro caso o ponto esteja no mesmo plano da imagem
	}

	void render_scene(std::vector<Obj> objs, SDL_Renderer* renderer) {

		vec3 light(0.0f, 0.0f, -1.0f);
		light.make_unit_vector();

		for (auto obj : objs) {
			for (int i = 0; i < obj.mesh.tris.size(); i++)
			{
				vec2 praster1;
				vec2 praster2;
				vec2 praster3;

				vec3 col(255, 255, 255);
				SDL_SetRenderDrawColor(renderer, 355, 250, 255, SDL_ALPHA_OPAQUE);

				fill_triangle(renderer, praster1, praster2, praster3, obj.mesh.tris[i], light);

				bool v1, v2, v3;
				v1 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[0].pos, praster1);
				v2 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[1].pos, praster2);
				v3 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[2].pos, praster3);

				if (v1 && v2)
					SDL_RenderDrawLine(renderer, praster1.x(), praster1.y(), praster2.x(), praster2.y());
				if (v1 && v3)
					SDL_RenderDrawLine(renderer, praster1.x(), praster1.y(), praster3.x(), praster3.y());
				if (v2 && v3)
					DrawLine(renderer, praster2, praster3);
			}
		}
	}
	
	bool rayTriangleIntersect(const vec3& orig, const vec3& dir, const vec3& A, const vec3& B, const vec3& C, vec3* ponto, float* T)
	{

		float t, u, v;

		//equação do plano
		vec3 AB = B - A;
		vec3 AC = C - A;
		vec3 NormalDoPlano = cross(AB, AC);

		//encontrar o ponto de interseção

		//plano e triangulo sao paralelos
		float NormalDoPlanodotDir = dot(NormalDoPlano, dir);
		if (fabs(NormalDoPlanodotDir) < kEpsilon)
			return false;

		//distancia 
		float d = dot(NormalDoPlano, A);

		//t
		t = -(dot(NormalDoPlano, orig) - d) / NormalDoPlanodotDir;


		if (t < 0) return false; //o triangulo esta atras do raio

		//ponto de interseção P
		vec3 P = orig + t * dir;

		*ponto = P;
		*T = t;

		//inside-outside test
		// aresta 0
		vec3 aresta0 = B - A;
		vec3 vp0 = P - A;
		vec3 C0 = cross(aresta0, vp0);
		if (dot(NormalDoPlano, C0) < 0) return false;

		// aresta 1
		vec3 aresta1 = C - B;
		vec3 vp1 = P - B;
		vec3 C1 = cross(aresta0, vp0);
		if ((u = dot(NormalDoPlano, C1)) < 0)  return false;

		// aresta 2
		vec3 aresta2 = A - C;
		vec3 vp2 = P - C;
		vec3 C2 = cross(aresta0, vp0);
		if ((v = dot(NormalDoPlano, C2)) < 0) return false;



		return true; // o raio acerta o triangulo
	}

	//End Revisar

	

	


	
	
};


#endif