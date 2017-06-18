#include "Laborator5.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>
#define PISTOL 1
#define GUN 2
#define GRENADE_LAUNCHER 3
#define TOWER 4
#define SPAWN_PISTOL 0.8
#define SPAWN_GUN 0.3
#define SPAWN_GRENADE_LAUNCHER 2
#define SPAWN_TOWER 3

using namespace std;

struct enemy {
	float x;
	float y;
	float z;
	float angle;
	float life;
	float animationAngle;
	float scale;
};

struct bullet {
	int type;
	float x;
	float y;
	float z;
	float startX;
	float startZ;
	float angle;
	float speed;
	float size;
	float power;
	float distance;
	float arg;
};

vector<enemy> enemies;
vector<bullet> bullets;

Laborator5::Laborator5()
{
}

Laborator5::~Laborator5()
{
}

void Laborator5::Init()
{
	renderCameraTarget = false;

	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	minimapCamera = new Laborator::Camera();
	minimapCamera->Set(glm::vec3(0, 40, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	minimapCamera->RotateFirstPerson_OZ(RADIANS(-90));

	playerX = 0;
	playerZ = 0;
	trigger = 0;
	ballAngle = 0;
	startingPointX = -10;
	startingPointZ = -10;
	weapon = 1;
	life1 = true;
	life2 = true;
	life3 = true;
	endAngle = 0;
	towerAngle1 = 0;
	towerAngle2 = 0;
	towerAngle3 = 0;

	playerAngle = RADIANS(90);
	spawnTime = 5;
	lastSpawnTime = Engine::GetElapsedTime();
	timeElapsed = 10;

	lastBullet = Engine::GetElapsedTime();
	lastBulletTower1 = Engine::GetElapsedTime();
	lastBulletTower2 = Engine::GetElapsedTime();
	lastBulletTower3 = Engine::GetElapsedTime();

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("Steve");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "Steve.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("Enemy");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "MonsterLowPoly.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("Tower");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "tower.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void Laborator5::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	resolutionX = resolution.x;
	resolutionY = resolution.y;
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

float computeDistance(float x1, float y1, float x2, float y2) {
	float distanceX = abs(x1 - x2);
	float distanceY = abs(y1 - y2);
	float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

	return distance;
}

float computeAngle(float x1, float y1, float x2, float y2) {
	float deltaX = x2 - x1;
	float deltaY = y2 - y1;
	float angle = atan2(deltaY, deltaX) * 180 / M_PI;

	return RADIANS(angle);
}

void Laborator5::Update(float deltaTimeSeconds)
{
	if (life1 | life2 | life3) {
		// asez turnurile pe harta
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-3, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.003, 0.003, 0.003));
			modelMatrix = glm::rotate(modelMatrix, towerAngle1, glm::vec3(0.f, 1.f, 0.f));
			RenderBothCameras(meshes["Tower"], shaders["VertexNormal"], modelMatrix);
		}

		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(3, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.003, 0.003, 0.003));
			modelMatrix = glm::rotate(modelMatrix, towerAngle2, glm::vec3(0.f, 1.f, 0.f));
			RenderBothCameras(meshes["Tower"], shaders["VertexNormal"], modelMatrix);
		}

		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -3));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.003, 0.003, 0.003));
			modelMatrix = glm::rotate(modelMatrix, towerAngle3, glm::vec3(0.f, 1.f, 0.f));
			RenderBothCameras(meshes["Tower"], shaders["VertexNormal"], modelMatrix);
		}

		// pozitionez vietile in jurul player-ului
		if (life1) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 0.3, playerZ));
			modelMatrix = glm::rotate(modelMatrix, ballAngle, glm::vec3(0.f, 1.f, 0.f));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.4, 0.3, -0.4));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20, 0.20, 0.20));
			RenderBothCameras(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
		}

		if (life2) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX + 0.3, 0.45, playerZ));
			modelMatrix = glm::rotate(modelMatrix, ballAngle, glm::vec3(1.f, 0.f, 0.f));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0.45, -0.3));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20, 0.20, 0.20));
			RenderBothCameras(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
		}

		if (life3) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 0.475, playerZ + 0.4));
			modelMatrix = glm::rotate(modelMatrix, ballAngle, glm::vec3(0.f, 0.f, 1.f));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35, 0.475, -0.35));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20, 0.20, 0.20));
			ballAngle += deltaTimeSeconds;
			RenderBothCameras(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
		}

		// pozitionez playerul
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			playerX = camera->GetTargetPosition().x;
			playerZ = camera->GetTargetPosition().z;

			modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 0, playerZ));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));
			modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
			if (weapon != GUN) {
				RenderBothCameras(meshes["Steve"], shaders["VertexNormal"], modelMatrix);
				camera->Set(glm::vec3(playerX, 0, playerZ) - camera->forward * camera->distanceToTarget, glm::vec3(playerX, 0, playerZ), camera->up);
			}
			else camera->Set(glm::vec3(playerX, 0.3, playerZ) - camera->forward * camera->distanceToTarget, glm::vec3(playerX, 0.3, playerZ), camera->up);
		}

		// pozitionez inamicii
		for (int i = 0; i < enemies.size(); i++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].x, enemies[i].y, enemies[i].z));
			if (enemies[i].animationAngle != 0)
				enemies[i].y -= deltaTimeSeconds;
			modelMatrix = glm::rotate(modelMatrix, enemies[i].angle, glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, enemies[i].animationAngle, glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(enemies[i].scale, enemies[i].scale, enemies[i].scale));
			RenderBothCameras(meshes["Enemy"], shaders["VertexNormal"], modelMatrix);
			float speed = deltaTimeSeconds * 2;
			if (enemies[i].z == startingPointZ)
				enemies[i].x += speed;
			else if (enemies[i].z > startingPointZ && enemies[i].z < startingPointZ * -1) {
				enemies[i].z += speed;
				enemies[i].x -= speed;
			}
			else if (enemies[i].z > startingPointZ * -1) {
				enemies[i].angle = RADIANS(90);
				enemies[i].x += speed;
			}
			if (enemies[i].x > startingPointX * -1) {
				enemies[i].z += speed;
				enemies[i].angle = -RADIANS(45);
			}
			if (enemies[i].x > startingPointX * -1 && enemies[i].z > startingPointZ * -1) {
				if (life1)
					life1 = false;
				else if (life2)
					life2 = false;
				else if (life3)
					life3 = false;
				enemies.erase(enemies.begin() + i);
			}
		}

		// pozitionez gloantele
		for (int i = 0; i < bullets.size(); i++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(bullets[i].x, bullets[i].y, bullets[i].z));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(bullets[i].size, bullets[i].size, bullets[i].size));
			RenderBothCameras(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			switch (bullets[i].type) {
			case PISTOL:
				bullets[i].x += bullets[i].speed * cos(bullets[i].angle);
				bullets[i].z += bullets[i].speed * sin(bullets[i].angle);
				if (computeDistance(bullets[i].x, bullets[i].z, bullets[i].startX, bullets[i].startZ) > bullets[i].distance) {
					bullets.erase(bullets.begin() + i);
				}
				break;
			case GUN:
				bullets[i].x += bullets[i].speed * cos(bullets[i].angle);
				bullets[i].z += bullets[i].speed * sin(bullets[i].angle);
				if (computeDistance(bullets[i].x, bullets[i].z, bullets[i].startX, bullets[i].startZ) > bullets[i].distance) {
					bullets.erase(bullets.begin() + i);
				}
				break;
			case GRENADE_LAUNCHER:
				bullets[i].x += bullets[i].speed * cos(bullets[i].angle);
				bullets[i].z += bullets[i].speed * sin(bullets[i].angle);
				bullets[i].y += sin(bullets[i].arg) * 0.06;
				bullets[i].arg += 0.06;
				if (computeDistance(bullets[i].x, bullets[i].z, bullets[i].startX, bullets[i].startZ) > bullets[i].distance) {
					bullets.erase(bullets.begin() + i);
				}
				break;
			case TOWER:
				bullets[i].x += bullets[i].speed * cos(bullets[i].angle);
				bullets[i].z += bullets[i].speed * sin(bullets[i].angle);
				bullets[i].y -= deltaTimeSeconds / 2;
				if (computeDistance(bullets[i].x, bullets[i].z, bullets[i].startX, bullets[i].startZ) > bullets[i].distance) {
					bullets.erase(bullets.begin() + i);
				}
				break;
			}
		}

		// generez inamicii
		if (Engine::GetElapsedTime() - lastSpawnTime > spawnTime) {
			//la fiecare 6s scad timpul de generare al inamicilor cu 0.3s; timpul va scadea pana cand ajunge la 2s
			if (spawnTime > 2 && Engine::GetElapsedTime() > timeElapsed) {
				spawnTime -= 0.3;
				timeElapsed += 10;
			}
			lastSpawnTime = Engine::GetElapsedTime();

			enemy enemy;
			enemy.x = startingPointX;
			enemy.y = 1.5;
			enemy.z = startingPointZ;
			enemy.angle = RADIANS(90);
			enemy.life = 500 / spawnTime;
			enemy.animationAngle = 0;
			enemy.scale = 1;

			enemies.push_back(enemy);
		}

		// generez gloantele pentru turnuri
		for (int i = 0; i < enemies.size(); i++) {
			float tower1Distance = computeDistance(enemies[i].x, enemies[i].z, -3, 0);
			float tower2Distance = computeDistance(enemies[i].x, enemies[i].z, 3, 0);
			float tower3Distance = computeDistance(enemies[i].x, enemies[i].z, 0, -3);
			float shootingArea = 6;

			if (tower1Distance < shootingArea | tower2Distance < shootingArea | tower3Distance < shootingArea) {
				bullet bullet;
				bullet.type = TOWER;
				bullet.y = 2;
				bullet.speed = 0.1;
				bullet.size = 0.3;
				bullet.distance = 16;
				bullet.power = 60;
				bullet.arg = 0;
				if (tower1Distance < shootingArea) {
					bullet.x = -3;
					bullet.z = 0;
					bullet.startX = -3;
					bullet.startZ = 0;
					bullet.angle = computeAngle(bullet.x, bullet.z, enemies[i].x, enemies[i].z);
					towerAngle1 = bullet.angle;
					if (Engine::GetElapsedTime() - lastBulletTower1 > SPAWN_TOWER) {
						lastBulletTower1 = Engine::GetElapsedTime();
						bullets.push_back(bullet);
					}
				}
				if (tower2Distance < shootingArea) {
					bullet.x = 3;
					bullet.z = 0;
					bullet.startX = 3;
					bullet.startZ = 0;
					bullet.angle = computeAngle(bullet.x, bullet.z, enemies[i].x, enemies[i].z);
					towerAngle2 = bullet.angle;
					if (Engine::GetElapsedTime() - lastBulletTower2 > SPAWN_TOWER) {
						lastBulletTower2 = Engine::GetElapsedTime();
						bullets.push_back(bullet);
					}
				}
				if (tower3Distance < shootingArea) {
					bullet.x = 0;
					bullet.z = -3;
					bullet.startX = 0;
					bullet.startZ = -3;
					bullet.angle = computeAngle(bullet.x, bullet.z, enemies[i].x, enemies[i].z);
					towerAngle3 = bullet.angle;
					if (Engine::GetElapsedTime() - lastBulletTower3 > SPAWN_TOWER) {
						lastBulletTower3 = Engine::GetElapsedTime();
						bullets.push_back(bullet);
					}
				}
			}
		}

		for (int i = 0; i < enemies.size(); i++) {
			bool collision = false;
			// daca inamicul a murit ca sa-i pot crea animatia
			if (enemies[i].life <= 0) {
				if (enemies[i].scale <= 0) {
					enemies.erase(enemies.begin() + i);
					break;
				}
				enemies[i].animationAngle += deltaTimeSeconds;
				enemies[i].scale -= deltaTimeSeconds / 1.5;
			}
			// verifica coliziunea inamicilor cu gloantele
			for (int j = 0; j < bullets.size(); j++) {
				float distance = computeDistance(enemies[i].x, enemies[i].z, bullets[j].x, bullets[j].z);
				if (distance < 1) {
					collision = true;
					enemies[i].life -= bullets[j].power;
					bullets.erase(bullets.begin() + j);
					break;
				}
			}
			if (collision)
				break;
		}
	}
	else {
		// se intra pe ramura asta daca jocul s-a terminat
		glm::mat4 modelMatrix = glm::mat4(1);
		playerX = camera->GetTargetPosition().x;
		playerZ = camera->GetTargetPosition().z;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, 0, playerZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));
		if (endAngle < 1.5)
			endAngle += deltaTimeSeconds;
		modelMatrix = glm::rotate(modelMatrix, endAngle, glm::vec3(0, 0, 1));
		RenderBothCameras(meshes["Steve"], shaders["VertexNormal"], modelMatrix);
	}
}

void Laborator5::FrameEnd()
{
	glViewport(0, 0, resolutionX, resolutionY);
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
	glViewport(resolutionX - 400, 30, 450, 250);
	DrawCoordinatSystem(minimapCamera->GetViewMatrix(), projectionMatrix);
}

void Laborator5::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

void Laborator5::RenderMeshMinimap(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(minimapCamera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

void Laborator5::RenderBothCameras(Mesh * mesh, Shader * shader, glm::mat4 & modelMatrix) {
	glViewport(0, 0, resolutionX, resolutionY);
	RenderMesh(mesh, shader, modelMatrix);
	glViewport(resolutionX - 400, 30, 450, 250);
	if (strcmp(mesh->GetMeshID(), "Steve") == 0) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(15, 15, 15));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1.f, 0.f, 0.f));
	}
	else if (strcmp(mesh->GetMeshID(), "Enemy") == 0) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.3, 1.3, 1.3));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0.f, 0.f, 1.f));
	}
	else if (strcmp(mesh->GetMeshID(), "Tower") == 0) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(500, 500, 500));
	}
	else if (strcmp(mesh->GetMeshID(), "sphere") == 0) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0, 0, 0));
	}
	RenderMeshMinimap(meshes["box"], shader, modelMatrix);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator5::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 4.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			// TODO : translate the camera forward
			camera->TranslateForward(deltaTime*cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// TODO : translate the camera to the left
			camera->TranslateRight(-deltaTime*cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// TODO : translate the camera backwards
			camera->TranslateForward(-deltaTime*cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// TODO : translate the camera to the right
			camera->TranslateRight(deltaTime*cameraSpeed);
		}
		// generez gloantele trase de player
		if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT)) {
			bullet bullet;
			bullet.type = weapon;
			bullet.x = playerX;
			bullet.y = 0.5;
			bullet.z = playerZ;
			bullet.startX = playerX;
			bullet.startZ = playerZ;
			bullet.angle = -playerAngle;

			float time;
			switch (weapon) {
			case PISTOL:
				bullet.speed = 0.1;
				bullet.size = 0.15;
				bullet.distance = 10;
				bullet.power = 30;
				bullet.arg = 0;
				if (Engine::GetElapsedTime() - lastBullet > SPAWN_PISTOL) {
					lastBullet = Engine::GetElapsedTime();
					bullets.push_back(bullet);
				}
				break;
			case GUN:
				bullet.speed = 0.3;
				bullet.size = 0.1;
				bullet.distance = 20;
				bullet.power = 20;
				bullet.arg = 0;
				if (Engine::GetElapsedTime() - lastBullet > SPAWN_GUN) {
					lastBullet = Engine::GetElapsedTime();
					bullets.push_back(bullet);
				}
				break;
			case GRENADE_LAUNCHER:
				bullet.speed = 0.15;
				bullet.size = 0.3;
				bullet.distance = 16;
				bullet.power = 60;
				bullet.arg = 0;
				if (Engine::GetElapsedTime() - lastBullet > SPAWN_GRENADE_LAUNCHER) {
					lastBullet = Engine::GetElapsedTime();
					bullets.push_back(bullet);
				}
				break;
			}
		}
	}

}

void Laborator5::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_1) {
		weapon = PISTOL;
		if (trigger == 1) {
			camera->MoveForward(-1.8);
			trigger = 0;
		}
	}
	if (key == GLFW_KEY_2) {
		weapon = GUN;
		if (trigger == 0) {
			camera->MoveForward(1.8);
		}
		trigger = 1;
	}
	if (key == GLFW_KEY_3) {
		weapon = GRENADE_LAUNCHER;
		if (trigger == 1) {
			camera->MoveForward(-1.8);
			trigger = 0;
		}
	}
}

void Laborator5::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = -0.001f;
		float sensivityOY = -0.001f;

		if (window->GetSpecialKeyState() == 0 && weapon != GUN) {
			renderCameraTarget = true;
			// TODO : rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(sensivityOX*deltaY);
			camera->RotateThirdPerson_OY(sensivityOY*deltaX);
			playerAngle += sensivityOX * deltaX;
		}

		if (window->GetSpecialKeyState() == 0 && weapon == GUN) {
			renderCameraTarget = false;
			// TODO : rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateFirstPerson_OX(sensivityOX*deltaY);
			camera->RotateFirstPerson_OY(sensivityOY*deltaX);
			playerAngle += sensivityOX * deltaX;
		}

	}
}

void Laborator5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{

}

void Laborator5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator5::OnWindowResize(int width, int height)
{
}
