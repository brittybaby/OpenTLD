#ifndef pegBox_Included
#define pegBox_Included
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ContourFeature.hpp"
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <set>
using namespace cv;
using namespace std;

// TODO do proper OOP
class pegBox
{
public:
	struct Peg
	{
		cv::Rect roi;
		Point2f center;
		int code;
	};

	struct Comparator {
		bool operator() (pair<Point2f, int> pt1, pair<Point2f, int> pt2) { return (pt1.first.x < pt2.first.x); }
	} _comparator;
public:
	pegBox();
	bool init(const cv::Mat &inp);
	void print();
	void updateRoiCenter(vector<pair<Rect, int> > &rois);
	pegBox(const cv::Mat &inp);
	pegBox(const vector<cv::Rect> &roi, const vector<Point2f> &center);
	void SwapFirstAndSecondHalf();
	
	vector<Peg> pegs;
	//vector<Peg> pegs_firstHalf;
	//vector<Peg> pegs_secondHalf;
 	cv::Mat mask;
	~pegBox();
private:
	int min_peg_threshold;

};
#endif