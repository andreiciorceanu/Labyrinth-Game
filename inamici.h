#pragma once
// Clasa de inamici
class Inamici {
public:
	Inamici();
	Inamici(float pos_x, float pos_y, float pos_z, int exist);
	~Inamici();

	float x, y, z;
	int is;
};