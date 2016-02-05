#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

class Util
{

public:
	Util();
	~Util();
	bool valInRange(int value, int min, int max)
	{
		return (value >= min) && (value <= max);
	}

	bool overlapRectangle(Rect A, Rect B)
	{
		bool xOverlap = valInRange(A.x, B.x, B.x + B.width) ||
			valInRange(B.x, A.x, A.x + A.width);

		bool yOverlap = valInRange(A.y, B.y, B.y + B.height) ||
			valInRange(B.y, A.y, A.y + A.height);

		return xOverlap && yOverlap;
	}
};


#endif