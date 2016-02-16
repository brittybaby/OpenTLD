#include "RingBox.h"

RingBox::RingBox()
{

}
void RingBox::update(const cv::Mat &inp)
{
	vector<vector<Point> > contours_ring;
	Mat initial_ring_mask = inp.clone();
	vector<Vec4i> hierarchy_ring;
	vector<Point2f> centre;
	vector<Rect> initial_roi;
	vector<float> radius;

	//imshow("Initial mask", initial_ring_mask);

	findContours(initial_ring_mask, contours_ring, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	unsigned int No_of_ring = contours_ring.size();
	vector<bool> markedRing(No_of_ring, true);

	for (int i = 0; i < No_of_ring; ++i)
	{
		if (contours_ring[i].size() < 35)
		{
			markedRing[i] = false;
		}
	}
	unsigned int count = 0;
	for (int i = 0; i < No_of_ring; ++i)
	{
		if (markedRing[i])
		{
			ContourFeature cFeat(contours_ring[i]);
			centre.push_back(cFeat.getCentre()); //centre.y += 10;

			initial_roi.push_back(boundingRect(contours_ring[i])); // This is a OpenCV function
			radius.push_back(sqrt(pow(boundingRect(contours_ring[i]).width, 2.0) + pow(boundingRect(contours_ring[i]).height, 2.0)) / 2);
			++count;
		}
	}
	assert(count == 6);
	int boxSize = centre.size();
	rings.resize(boxSize);
	for (int i = 0; i < boxSize; ++i)
	{
		rings[i] = Ring(centre[i], radius[i], initial_roi[i]);
	}
}

void RingBox::updateCode(const pegBox &pegBox)
{
	for (int i = 0; i < rings.size(); ++i)
	{
		Rect r1 = rings[i].getRoi();
		for (int p = 0; p < pegBox.pegs.size(); ++p)
		{
			Rect r2 = pegBox.pegs[p].roi;
			if (rectOverlap(r1, r2))
			{
				rings[i].setCode(pegBox.pegs[p].code);
			}
		}
	}
}
RingBox::RingBox(const cv::Mat &inp)
{
	vector<vector<Point> > contours_ring;
	Mat initial_ring_mask = inp.clone();
	vector<Vec4i> hierarchy_ring;
	vector<Point2f> centre;
	vector<Rect> initial_roi;
	vector<float> radius;

	//imshow("Initial mask", initial_ring_mask);
	
	findContours(initial_ring_mask, contours_ring, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	unsigned int No_of_ring = contours_ring.size();
	vector<bool> markedRing(No_of_ring);
	
	for (int i = 0; i < No_of_ring; ++i)
	{
		if (contours_ring[i].size() < 35)
		{
			markedRing[i] = false;
		}
	}
	unsigned int count = 0;
	for (int i = 0; i < No_of_ring; ++i)
	{
		if (markedRing[i])
		{
			ContourFeature cFeat(contours_ring[i]);
			centre.push_back(cFeat.getCentre()); //centre.y += 10;

			initial_roi.push_back(boundingRect(contours_ring[i])); // This is a OpenCV function
			radius.push_back(sqrt(pow(boundingRect(contours_ring[i]).width, 2.0) + pow(boundingRect(contours_ring[i]).height, 2.0)) / 2);
			++count;
		}
	}
	assert(count == 6);
	RingBox(centre, radius, initial_roi);
}
RingBox::RingBox(const vector<Point2f> &centers, const vector<float> &radii, const vector<Rect> &roi)
{
	int boxSize = centers.size();
	rings.resize(boxSize);
	for (int i = 0; i < boxSize; ++i)
	{
		rings[i] = Ring(centers[i], radii[i], roi[i]);
	}
}

RingBox::~RingBox()
{
	rings.clear();
}
