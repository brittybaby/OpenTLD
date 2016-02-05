#ifndef RING_HPP
#define RING_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class Ring
{
public:
	Ring();
	Ring(const Point2f&, float, const Rect&, string ringState = "stationary", int code = 0);
	Point2f getCenter();
	float getRadii();
	int getCode();
	void setCode(int code);
	void setStatus(const string &status);
	string getStatus();
	//Mat getRing_mask();
	Rect getRoi();
protected:
	Point2f center;
	float  radius;
	//Mat ring_mask;
	Rect roi;
	int code_pos;
	string ringState;
};

#endif

