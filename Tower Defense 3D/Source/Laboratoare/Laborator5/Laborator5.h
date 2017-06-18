#pragma once
#include <Component/SimpleScene.h>
#include "LabCamera.h"

class Laborator5 : public SimpleScene
{
	public:
		Laborator5();
		~Laborator5();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix) override;
		void RenderMeshMinimap(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix);
		void RenderBothCameras(Mesh * mesh, Shader * shader, glm::mat4 & modelMatrix);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	protected:
		Laborator::Camera *camera;
		Laborator::Camera *minimapCamera;
		glm::mat4 projectionMatrix;
		float playerX, playerZ;
		float playerAngle;
		bool renderCameraTarget;
		float spawnTime;
		float lastSpawnTime;
		float timeElapsed;
		float startingPointX;
		float startingPointZ;
		int weapon;
		float lastBullet;
		float lastBulletTower1;
		float lastBulletTower2;
		float lastBulletTower3;
		float trigger;
		float resolutionX;
		float resolutionY;
		float ballAngle;
		bool life1;
		bool life2;
		bool life3;
		float endAngle;
		float towerAngle1;
		float towerAngle2;
		float towerAngle3;
};
