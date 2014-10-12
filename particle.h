#include <GL/glut.h>
#include "define.h"
#include <vector>

class Particle {

public:
	Particle();
	Particle(float x, float y, float z, float temperature, float vx, float vy, float vz);
	void Draw();

	// Setters
	void SetTemperature(float t);

	// Getters
	float GetTemperature();
	float GetX();
	float GetY();
	float GetZ();
	float GetVX();
	float GetVY();
	float GetVZ();
	
	
	void calculateNewPosition();
	void calculateForces(std::vector<Particle> neighbours);

private:
	float x, y, z; // Coords
	float temperature; 
	float vx, vy, vz; // Velocity
};
