#pragma once
#include <include/glm.h>
#include <include/math.h>

namespace Laborator
{
	class Camera
	{
		public:
			Camera()
			{
				position = glm::vec3(0, 2, 5);
				forward = glm::vec3(0, 0, -1);
				up		= glm::vec3(0, 1, 0);
				right	= glm::vec3(1, 0, 0);
				distanceToTarget = 2;
			}

			Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
			{
				Set(position, center, up);
			}

			~Camera()
			{ }

			// Update camera
			void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
			{
				this->position = position;
				forward = glm::normalize(center-position);
				right	= glm::cross(forward, up);
				this->up = glm::cross(right,forward);
			}

			void MoveForward(float distance)
			{
				glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
				// movement will keep the camera at the same height always
				// Example: If you rotate up/down your head and walk forward you will still keep the same relative distance (height) to the ground!
				// Translate the camera using the DIR vector computed from forward
				this->position = this->position + dir*distance;
			}

			void TranslateForward(float distance)
			{
				// TODO : Translate the camera using the "forward" vector
				glm::vec3 dir = glm::normalize(glm::vec3(forward.x, forward.y, forward.z));
				this->position = this->position + dir*distance;
			}

			void TranslateUpword(float distance)
			{
				// TODO : Translate the camera using the up vector
				this->position = this->position + glm::normalize(this->up)*distance;
			}

			void TranslateRight(float distance)
			{
				// TODO
				// Translate the camera using the "right" vector
				// Usually translation using camera "right' is not very useful because if the camera is rotated around the "forward" vector 
				// translation over the right direction will have an undesired effect; the camera will get closer or farther from the ground
				// Using the projected right vector (onto the ground plane) makes more sense because we will keep the same distance from the ground plane
				glm::vec3 dir = glm::normalize(glm::vec3(right.x, right.y, right.z));
				this->position = this->position + dir*distance;
			}

			void RotateFirstPerson_OX(float angle)
			{
				// TODO
				// Compute the new "forward" and "up" vectors
				// Attention! Don't forget to normalize the vectors
				// Use glm::rotate()
				glm::vec3 newForward = glm::rotate(glm::mat4(2.0f), angle, glm::vec3(right)) * glm::vec4(forward, 1);
				forward = glm::normalize(glm::vec3(newForward));
				up = glm::cross(right, forward);

			}

			void RotateFirstPerson_OY(float angle)
			{
				// TODO
				// Compute the new "forward", "up" and "right" vectors
				// Don't forget to normalize the vectors
				// Use glm::rotate()
				glm::vec3 newForward = glm::rotate(glm::mat4(2.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
				forward = glm::normalize(glm::vec3(newForward));
				glm::vec3 newRight = glm::rotate(glm::mat4(2.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
				right = glm::normalize(glm::vec3(newRight));
				up = glm::cross(right, forward);
				
			}

			void RotateFirstPerson_OZ(float angle)
			{
				// TODO
				// Compute the new Right and Up, Forward stays the same
				// Don't forget to normalize the vectors
				glm::vec3 newRight = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(forward)) * glm::vec4(right, 1);
				right = glm::normalize(glm::vec3(newRight));
				up = glm::cross(right, forward);

			}

			void RotateThirdPerson_OX(float angle)
			{
				// TODO
				// Rotate the camera in Third Person mode - OX axis
				// Use distanceToTarget as translation distance
				this->TranslateForward(distanceToTarget);
				this->RotateFirstPerson_OX(angle);
				this->TranslateForward(-distanceToTarget);
			
			}

			void RotateThirdPerson_OY(float angle)
			{
				// TODO
				// Rotate the camera in Third Person mode - OY axis
				this->TranslateForward(distanceToTarget);
				this->RotateFirstPerson_OY(angle);
				this->TranslateForward(-distanceToTarget);
			}

			void RotateThirdPerson_OZ(float angle)
			{
				// TODO
				// Rotate the camera in Third Person mode - OZ axis
				this->TranslateForward(distanceToTarget);
				this->RotateFirstPerson_OZ(angle);
				this->TranslateForward(-distanceToTarget);
			}

			glm::mat4 GetViewMatrix()
			{
				// Returns the View Matrix
				return glm::lookAt(position, position + forward, up);
			}

			glm::vec3 GetTargetPosition()
			{
				return position + forward * distanceToTarget;
			}

		public:
			float distanceToTarget;
			glm::vec3 position;
			glm::vec3 forward;
			glm::vec3 right;
			glm::vec3 up;
		};
}