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

	void Draw();
	void DrawModel();
};


