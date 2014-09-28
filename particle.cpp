/*
 * particle.cpp
 *
 *  Created on: 21/09/2014
 *      Author: not ben
 */

#include "particle.h"

Particle::Particle() {
	Particle::x = 0.0;
	Particle::y = 0.0;
	Particle::z = 0.0;

	Particle::temperature = DEF_TEMPERATURE;
	
	Particle::vx = 0.0;
	Particle::vy = 0.0;
	Particle::vz = 0.0;
}

Particle::Particle(float x, float y, float z, float temperature, float vx, float vy, float vz){
	Particle::x = x;
	Particle::y = y;
	Particle::z = z;

	Particle::temperature = temperature;

	Particle::vx = vx;
	Particle::vy = vy;
	Particle::vz = vz;
}

void Particle::Draw(){
	glPushMatrix();
	glTranslatef(x,y,z);

	gluSphere(gluNewQuadric(), RADIUS, SPHERE_SLICES, SPHERE_STACKS ); //TODO Quadric??

	glPopMatrix();
}

// Setters
void Particle::SetTemperature(float t){
	if(t > MIN_TEMPERATURE)
		temperature = t;
	else
		temperature = MIN_TEMPERATURE;
}

// Getters
float Particle::GetTemperature(){
	return temperature;
}

float Particle::GetX(){
	return x;
}

float Particle::GetY(){
	return y;
}

float Particle::GetZ(){
	return z;
}

float Particle::GetVX(){
	return vx;
}

float Particle::GetVY(){
	return vy;
}

float Particle::GetVZ(){
	return vz;
}

