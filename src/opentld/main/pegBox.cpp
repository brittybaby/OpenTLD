#include "pegBox.h"


pegBox::pegBox()
{
	//pegs_firstHalf.resize(6);
	//pegs_secondHalf.resize(6);
	min_peg_threshold = 25;
}

bool pegBox::init(const cv::Mat &inp)
{
	mask = inp.clone();
	vector<vector<Point> > pegsI;
	Mat mask = inp.clone();
	vector<Vec4i> hierarchy_ring;
	vector<Point2f> centre;
	vector<Rect> initial_roi;

	//imshow("Initial mask", inp);
	//waitKey(0);
	findContours(mask, pegsI, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	unsigned int No_of_pegs = pegsI.size();
	vector<bool> markedPegs(No_of_pegs, true);

	for (int i = 0; i < No_of_pegs; ++i)
	{
		if (pegsI[i].size() < min_peg_threshold)//TODO (define min threshold)
		{
			markedPegs[i] = false;
		}
	}
	unsigned int count = 0;
	for (int i = 0; i < No_of_pegs; ++i)
	{
		if (markedPegs[i])
		{
			ContourFeature cFeat(pegsI[i]);
			centre.push_back(cFeat.getCentre()); //centre.y += 10;
			initial_roi.push_back(boundingRect(pegsI[i])); // This is a OpenCV function
			++count;
		}
	}
	if (count == 12)
	{
		//pegs_firstHalf.resize(6);
		//pegs_secondHalf.resize(6);
		int no = initial_roi.size();

		pegs.resize(no);
		count = 0;
		vector<pair<Point2f, int> >center_id(no);
		std::unordered_map<int, Rect> roi_id;
		for (int i = 0; i < no; ++i)
		{
			roi_id[i] = initial_roi[i];
			center_id[i] = make_pair(centre[i], i);
		}
		sort(center_id.begin(), center_id.end(), _comparator);
		vector<pair<Point2f, int> > pts(3);
		for (int p = 0; p <= center_id.size() - 3; p = p + 3)
		{
			pts[0] = make_pair(Point2f(center_id[p].first.y, center_id[p].first.x), center_id[p].second);
			pts[1] = make_pair(Point2f(center_id[p + 1].first.y, center_id[p + 1].first.x), center_id[p + 1].second);
			pts[2] = make_pair(Point2f(center_id[p + 2].first.y, center_id[p + 2].first.x), center_id[p + 2].second);
			sort(pts.begin(), pts.end(), _comparator);
			for (int i = 0; i < 3; i++)
			{
				pegs[count].center = Point2f(pts[i].first.y, pts[i].first.x);
				pegs[count].code = count;
				pegs[count].roi = roi_id[pts[i].second];
				count++;
			}
		}
		return true;
	}
	else
	{
		return false;
	}


}

void pegBox::print()
{
	for (int i = 0; i < pegs.size(); ++i)
	{
		Peg peg = pegs[i];
		cout << "Code(" << peg.code << ") center (" << peg.center.x << peg.center.y << ")  ROI (" << peg.roi.x << peg.roi.y << peg.roi.width << peg.roi.height << ")\n";
	}
}

pegBox::pegBox(const cv::Mat &inp)
{
	vector<vector<Point> > pegsI;
	Mat mask = inp.clone();
	vector<Vec4i> hierarchy_ring;
	vector<Point2f> centre;
	vector<Rect> initial_roi;

	//imshow("Initial mask", initial_ring_mask);

	findContours(mask, pegsI, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	unsigned int No_of_pegs = pegsI.size();
	vector<bool> markedPegs(No_of_pegs);

	for (int i = 0; i < No_of_pegs; ++i)
	{
		if (pegsI[i].size() < 35)//TODO (define min threshold)
		{
			markedPegs[i] = false;
		}
	}
	unsigned int count = 0;
	for (int i = 0; i < No_of_pegs; ++i)
	{
		if (markedPegs[i])
		{
			ContourFeature cFeat(pegsI[i]);
			centre.push_back(cFeat.getCentre()); //centre.y += 10;
			initial_roi.push_back(boundingRect(pegsI[i])); // This is a OpenCV function
			++count;
		}
	}
	assert(count == 16); // there must be 12 pegs
	pegBox(initial_roi, centre);
}
pegBox::pegBox(const vector<cv::Rect> &roi, const vector<Point2f> &center)
{
	int no = roi.size();
	pegs.resize(no);
	int  count = 0;
	vector<pair<Point2f, int> >center_id(no);
	std::unordered_map<int, Rect> roi_id;
	for (int i = 0; i < no; ++i)
	{
		roi_id[i] = roi[i];
		center_id[i] = make_pair(center[i], i);
	}
	sort(center_id.begin(), center_id.end(), _comparator);
	vector<pair<Point2f, int> > pts(4);
	for (int p = 0; p < center_id.size() - 3; p = p + 4)
	{
		pts[0] = make_pair(Point2f(center_id[p].first.y, center_id[p].first.x), center_id[p].second);
		pts[1] = make_pair(Point2f(center_id[p+1].first.y, center_id[p+1].first.x), center_id[p+1].second);
		pts[2] = make_pair(Point2f(center_id[p+2].first.y, center_id[p+2].first.x), center_id[p+2].second);
		pts[3] = make_pair(Point2f(center_id[p+3].first.y, center_id[p+3].first.x), center_id[p+3].second);
		sort(pts.begin(), pts.end(), _comparator);
		for (int i = 0; i < 4; i++)
		{
			pegs[count].center = Point2f(pts[i].first.x, pts[i].first.y);
			pegs[count].code = pts[i].second;
			pegs[count].roi = roi_id[pegs[count].code];
			count++;
		}
	}
}


pegBox::~pegBox()
{
	pegs.clear();
}

// TODO define pegs_firstHalf and pegs_secondHalf according to user input
void pegBox::updateRoiCenter(vector<pair<Rect, int> > &rois)
{
	int count = 0;
	set<int> s1;
	set<int> s2;
	set<int> s3;
	s1.insert(0);
	s1.insert(1);
	s1.insert(2);
	s1.insert(3);
	s1.insert(4);
	s1.insert(5);

	s2.insert(6);
	s2.insert(7);
	s2.insert(8);
	s2.insert(9);
	s2.insert(10);
	s2.insert(11);

	for (int i = 0; i < rois.size(); ++i)
	{
		s3.insert(rois[i].second);
	}
	std::vector<int> c1;
	std::vector<int> c2;
	set_intersection(s1.begin(), s1.end(), s3.begin(), s3.end(), std::back_inserter(c1));
	set_intersection(s2.begin(), s2.end(), s3.begin(), s3.end(), std::back_inserter(c2));

	if (c2.empty())
	{
		for (int i = 0; i < rois.size(); ++i)
		{
			int code = rois[i].second;
			for (int p = 0; p < pegs.size(); ++p)
			{
				if (pegs[p].code == code)
				{
					// first half
					Rect r = rois[i].first;
					pegs[p].roi = r;
					pegs[p].center = Point2f((pegs[p].roi.x + pegs[p].roi.width / 2.0), (pegs[p].roi.y + pegs[p].roi.height / 2.0));

					Point2f cen = pegs[p + 6].center;
					pegs[p + 6].roi = Rect(cen.x - r.width / 2, cen.y - r.height / 4, r.width, r.height);

					count++;
				}
			}
		}
	}
	else if (c1.empty())
	{
		for (int i = 0; i < rois.size(); ++i)
		{
			int code = rois[i].second;
			for (int p = 0; p < pegs.size(); ++p)
			{
				if (pegs[p].code == code)
				{
					// first half
					Rect r = rois[i].first;
					pegs[p].roi = r;
					pegs[p].center = Point2f((pegs[p].roi.x + pegs[p].roi.width / 2.0), (pegs[p].roi.y + pegs[p].roi.height / 2.0));

					Point2f cen = pegs[p - 6].center;
					pegs[p - 6].roi = Rect(cen.x - r.width / 2, cen.y - r.height / 4, r.width, r.height);
					count++;
				}
			}
		}
	}
	else
	{
		cout << "Wrong placement of the rings \n";
		exit(0);
	}


}

void pegBox::SwapFirstAndSecondHalf()
{
	//vector<Peg> temp(pegs_firstHalf.size());
	//for (int i = 0; i < pegs_firstHalf.size(); ++i)
	//{
	//	temp[i] = pegs_firstHalf[i];
	//	pegs_firstHalf[i] = pegs_secondHalf[i];
	//	pegs_secondHalf[i] = temp[i];
	//}

}