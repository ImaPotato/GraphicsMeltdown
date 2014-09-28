#include <GL/glut.h>
#include "define.h"

# define RADIUS 0.5
# define SPHERE_SLICES 20
# define SPHERE_STACKS 20

#define MIN_TEMPERATURE -273
#define DEF_TEMPERATURE 20 // Default temperature 

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

private:
	float x, y, z; // Coords
	float temperature; 
	float vx, vy, vz; // Velocity
};
