#include "ControllerState.h"

void ControllerState::setInitialAxesValues(float lx, float ly, float rx, float ry) {
	this->lx = this->init_lx = lx;
	this->ly = this->init_ly = ly;
	this->rx = this->init_rx = rx;
	this->ry = this->init_ry = ry;

	rangeLXMin = init_lx - precision;
	rangeLXMax = init_lx + precision;

	rangeLYMin = init_ly - precision;
	rangeLYMax = init_ly + precision;

	rangeRXMin = init_rx - precision;
	rangeRXMax = init_rx + precision;

	rangeRYMin = init_rx - precision;
	rangeRYMax = init_rx + precision;
}

bool ControllerState::modelShouldRotate(float lx, float ly, float rx, float ry) {
	bool rotate = false;

	if (/*lx <= rangeLXMin || lx >= rangeLXMax ||
		ly <= rangeLYMin || ly >= rangeLYMax ||*/
		rx <= rangeRXMin || rx >= rangeRXMax ||
		ry <= rangeRYMin || ry >= rangeRYMax) rotate = true;
		
	this->lx = lx;
	this->ly = ly;
	this->rx = rx;
	this->ry = ry;

	return rotate;
}