#include <string>
#include <math.h>
#include <algorithm>
#include "camera.h" 
#include "ImGUI/imgui_sdl.h"
#include "ImGUI/imgui.h"


//Revisar
	
float zoomFrom = 50;
float upAndDownRotation = 0;

// End Revisar

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		SDL_Window* window = SDL_CreateWindow("Projeto PG - Grupo X", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
		SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		if (window && renderer) {
			SDL_bool done = SDL_FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);

			std::vector<Obj> objects;
			objects.push_back(Obj("./object/monkey_smooth.obj"));
			// objects.push_back(Obj("./textura/elf.obj", "./Textura/Elf_Albedo.png"));
			// objects.push_back( Obj("./textura/scrat.obj") );


			ImGui::CreateContext();
			ImGuiSDL::Initialize(renderer, WIDTH, HEIGHT);

			camera cam(vec3(0, 0, 5), vec3(0, 0, -1), vec3(0, 1, 0), 90.0f, 1.f, WIDTH, HEIGHT);

			float my_color[4];
			bool my_tool_active;

			while (!done) {
				SDL_Event event;
				ImGuiIO& io = ImGui::GetIO();
				std::clock_t then = std::clock();

				int mouseX, mouseY;
				const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

				io.DeltaTime = 1.0f / 60.0f;
				io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
				io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

				ImGui::NewFrame();

				// Create a window called "My First Tool", with a menu bar.
				ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
						if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
						if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				// Edit a color (stored as ~4 floats)
				ImGui::ColorEdit4("Color", my_color);
				// Plot some values
				const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
				ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

				// Display contents in a scrolling region
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
				ImGui::BeginChild("Scrolling");
				ImGui::Text("Random Message\n");
				ImGui::EndChild();
				ImGui::End();

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
				SDL_RenderClear(renderer); // clear previous frame generated image

				cam.render_scene(objects, renderer); // feed triangle data onto renderer

				ImGui::Render();
				ImGuiSDL::Render(ImGui::GetDrawData());
				SDL_RenderPresent(renderer); // present the generated triangle data onto screen

				while (SDL_PollEvent(&event)) {

					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym == SDLK_a)
							printf("Just pressed keyboard key A!\n");



						//Revisar Depois
					
							//pra frente
							if (event.key.keysym.sym == SDLK_w) {
								zoomFrom = zoomFrom - 0.1f;
								cam._from[2] = zoomFrom;
								// cam._at[2] = -1;
								cam.camToWorld[2][2] = zoomFrom;
								// cam.camToWorld[2][3] = -1;
								cam.look_at(vec3(0, upAndDownRotation, zoomFrom), vec3(0, 0, -1), vec3(0, 1, 0));

							}
							//pra tras
							if (event.key.keysym.sym == SDLK_s) {
								zoomFrom = zoomFrom + 0.1f;
								cam._from[2] = zoomFrom;
								// cam._at[2] = -1;
								cam.camToWorld[2][2] = zoomFrom;
								// cam.camToWorld[2][3] = -1;
								cam.look_at(vec3(0, upAndDownRotation, zoomFrom), vec3(0, 0, -1), vec3(0, 1, 0));

							}

							if (event.key.keysym.sym == SDLK_DOWN) {
								upAndDownRotation = upAndDownRotation + 0.1f;
								upAndDownRotation = std::min(2.0f, upAndDownRotation);
								// zoomFrom= zoomFrom - 0.1f;
								// cam._from[2] = zoomFrom;
								// // cam._at[2] = -1;
								// cam.camToWorld[2][2] = zoomFrom;
								// // cam.camToWorld[2][3] = -1;
								cam.look_at(vec3(0, upAndDownRotation, zoomFrom), vec3(0, 0, -1), vec3(0, 1, 0));

							}
							//pra tras
							if (event.key.keysym.sym == SDLK_UP) {
								upAndDownRotation = upAndDownRotation - 0.1f;
								upAndDownRotation = std::max(-2.0f, upAndDownRotation);
								// zoomFrom = zoomFrom + 0.1f;
								// cam._from[2] = zoomFrom;
								// // cam._at[2] = -1;
								// cam.camToWorld[2][2] = zoomFrom;
								// // cam.camToWorld[2][3] = -1;
								cam.look_at(vec3(0, upAndDownRotation, zoomFrom), vec3(0, 0, -1), vec3(0, 1, 0));

							}

						//End Revisar


					}

					// mouse pressed event
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						//If the left mouse button was pressed
						if (event.button.button == SDL_BUTTON_RIGHT)
						{
						}
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							//Relative mouse mode moves mouse to center of window
							//every frame. Useful if you want to use mouse and not worry
							//about it going off screen
							//Revisar
							/*
							
							int x;
							int y;

							float t;

							vec3 vecDst;
							Mesh malha;

							vec3 A;
							vec3 B;
							vec3 C;
							vec3 AB;
							vec3 AC;
							vec3 normal;
							vec3 C1;
							vec3 C2;
							vec3 C3;
							float D;

							vec3 P;

							SDL_GetMouseState(&x, &y);

							float Px = -(2 * (x / cam.imgWidth) - 1) * cam.right * cam._near / 2;
							float Py = (1 - 2 * (y / cam.imgHeight)) * cam.left * cam._near / 2;

							vec3 RayDirection = (Px, Py, -1);

							cam.camToWorld.mult_vec_matrix(RayDirection, vecDst);

							RayDirection.make_unit_vector();

							for (auto& object : objects) {
								for (auto& tr : object.mesh.tris) {

									A = tr.vertex[0].pos;
									B = tr.vertex[1].pos;
									C = tr.vertex[2].pos;

									AB = tr.vertex[1].pos - tr.vertex[0].pos;

									AC = tr.vertex[2].pos - tr.vertex[0].pos;

									normal = cross(AB, AC);

									D = dot(normal, tr.vertex[0].pos);

									//t = (normal.x() * cam._from.x() + normal.y() * cam._from.y() + normal.z() * cam._from.z() + D) / (normal.x() * cam._from.x() + normal.y() * cam._from.y() + normal.z() * cam._from.z());

									t = -(dot(normal, cam._from) - D) / dot(normal, RayDirection);
									
									// Verificar se h� intersec��o entre o raio e o plano
									if (!(((dot(normal, RayDirection) < 0.00001)) && t < 0)) {

										// Ponto P de intersec��o do raio com o plano
										P = cam._from + t * RayDirection;

										C1 = cross(P - A, B - A);
										C2 = cross(P - B, C - B);
										C3 = cross(P - C, A - C);


										// Verificar se o ponto P est� dentro do triangulo
										if ((dot(C1, normal) >= 0) && (dot(C2, normal) >= 0) && (dot(C3, normal) >= 0)) {

											printf("Objeto selecionado");
											
										}
									}
								}
							}

							SDL_SetRelativeMouseMode(SDL_TRUE);
							*/

							vec3 orig = cam._from;
							vec3 P(0, 0, 0);
							float t = 0.0f;
							int x, y;
							SDL_GetMouseState(&x, &y);
							std::cout << x << " " << y << '\n';
							float px = -(2 * ((float)x / (float)WIDTH) - 1) * (float)cam.right / (float)cam._near;
							float py = (1 - 2 * ((float)y / (float)HEIGHT)) * (float)cam.top / (float)cam._near;
							std::cout << px << " " << py << " " << -1 << '\n';
							vec3 dir(px, py, -1);
							cam.camToWorld.mult_vec_matrix(dir, dir);
							dir.make_unit_vector();
							std::cout << dir.x() << " " << dir.y() << " " << dir.z() << '\n';
							bool object = false;
							for (auto obj : objects) {
								for (int i = 0; i < obj.mesh.tris.size(); i++) {
									object = cam.rayTriangleIntersect(orig, dir, obj.mesh.tris[i].vertex[0].pos, obj.mesh.tris[i].vertex[1].pos, obj.mesh.tris[i].vertex[2].pos, &P, &t) | object;
									// std::cout <<object<<"/n";
								}
							}
							std::cout << "ponto P " << P << " endereco da inter " << t << " encontrou ? " << object << '\n';
							// SDL_SetRelativeMouseMode(SDL_TRUE);

						}
					}
					if (event.type == SDL_MOUSEBUTTONUP)
					{
						//If the left mouse button was released
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							SDL_SetRelativeMouseMode(SDL_FALSE);
						}
					}

					// mouse move event
					if (event.type == SDL_MOUSEMOTION)
					{
						//get relative mouse motion, i.e mouse delta x,y from last frame
						float x = event.motion.xrel;
						float y = event.motion.yrel;

					}

					if (event.type == SDL_QUIT)
						done = SDL_TRUE;

				}
			}
		}

		if (renderer) {
			SDL_DestroyRenderer(renderer);
		}
		if (window) {
			SDL_DestroyWindow(window);
		}
	}
	SDL_Quit();
	return 0;
}