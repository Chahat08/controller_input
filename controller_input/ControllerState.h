#pragma once

#include <vector>

class ControllerState {
private:
	float init_lx, init_ly;
	float init_rx, init_ry;

	float lx, ly;
	float rx, ry;

	float precision = 0.1f;

	float rangeLXMin;
	float rangeLXMax;
					
	float rangeLYMin;
	float rangeLYMax;
					
	float rangeRXMin;
	float rangeRXMax;
					
	float rangeRYMin;
	float rangeRYMax;

public:
	ControllerState(float lx, float ly, float rx, float ry) :lx(lx), ly(ly), rx(rx), ry(ry) {};

	void setInitialAxesValues(float, float, float ,float);
	bool modelShouldRotate(float, float, float, float);
};