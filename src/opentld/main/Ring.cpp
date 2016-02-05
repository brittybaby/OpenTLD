#include "Ring.h"


Ring::Ring() :center(Point2f(0, 0)), radius(0), roi()
{
}

Ring::Ring(const Point2f & cen, float radi, const Rect &region,  string s, int code) : center(cen), radius(radi), roi(region), ringState(s), code_pos(code)
{
}


Point2f Ring::getCenter()
{
	return this->center;
}

float Ring::getRadii()
{
	return this->radius;
}
int Ring::getCode()
{
	return code_pos;
}
Rect Ring::getRoi()
{
	return this->roi;
}

void Ring::setCode(int code)
{ 
	this->code_pos = code; 
}
void Ring::setStatus(const string &status)
{
	this->ringState = status; 
}
string Ring::getStatus()
{ 
	return this->ringState; 
}
