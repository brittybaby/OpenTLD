#ifndef Ring_Box_Included
#define Ring_Box_Included
#include "Ring.h"
#include "ContourFeature.hpp"
#include "pegBox.h"

#include <cassert>
#include <vector>
using namespace std;
class RingBox
{
public:
	RingBox();
	RingBox(const cv::Mat &inp);
	void update(const cv::Mat &inp);
	void updateCode(const pegBox &pegBox);
	RingBox(const vector<Point2f> &centers, const vector<float> &radii, const vector<Rect> &roi);
	~RingBox();
	vector<Ring> rings;

public:

	bool RingBox::valueInRange(int value, int min, int max)
	{
		return (value >= min) && (value <= max);
	}

	bool rectOverlap(Rect A, Rect B)
	{
		bool xOverlap = valueInRange(A.x, B.x, B.x + B.width) ||
			valueInRange(B.x, A.x, A.x + A.width);

		bool yOverlap = valueInRange(A.y, B.y, B.y + B.height) ||
			valueInRange(B.y, A.y, A.y + A.height);

		return xOverlap && yOverlap;
	}
};

#endif