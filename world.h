/*
 * world.h
 *
 *  Created on: 21/09/2014
 *      Author: riddelbenj
 */


#include "octree.h"
#include "particle.h"

class World {

public:
	World();
	
	void LoadModel(char* fileName);
	void LoadDefaultModel();

	void UpdateModel();
	void CalculateTemperatures(Octree<Particle>* nextFrame);
	void CalculateVelocities(Octree<Particle>* nextFrame);
	void CalculatePositions(Octree<Particle>* nextFrame);

	void Draw();
	void DrawModel();
};


