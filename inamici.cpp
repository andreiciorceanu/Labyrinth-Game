#include "inamici.h"

Inamici::Inamici() {}

Inamici::Inamici(float pos_x, float pos_y, float pos_z, int exist) {
	x = pos_x;
	y = pos_y;
	z = pos_z;
	is = exist;
}

Inamici::~Inamici() {}