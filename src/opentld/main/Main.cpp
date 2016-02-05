/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/*
 * MainX.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#include "Main.h"

#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "TLDUtil.h"
#include "Trajectory.h"

using namespace tld;
using namespace cv;

void Main::draw(Mat &img)
{
	int pegSize = _pegBox.pegs.size();
	int ringSize = _ringBox.rings.size();
	//Mat blank = Mat::zeros(img, width, CV_8UC3);
	for (int i = 0; i < ringSize; ++i)
	{
		Rect r = _ringBox.rings[i].getRoi();
		Point2f p = _ringBox.rings[i].getCenter();
		int code = _ringBox.rings[i].getCode();
		rectangle(img, r, Scalar(255, 0, 0), 16);
		putText(img, SSTR(code), p, FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 0, 255), 2, 8, false);
	}

	for (int i = 0; i < pegSize; ++i)
	{
		Rect r = _pegBox.pegs[i].roi;
		Point2f p = _pegBox.pegs[i].center;
		int code = _pegBox.pegs[i].code;
		rectangle(img, r, Scalar(0, 255, 0), 2);
		putText(img, SSTR(code), p, FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);
	}
	imshow("blank", img);
	waitKey(0);
}

void Main::HittingDetection(const Mat &prv_frame, const Mat &curr_frame, vector<unsigned int> &hittingData)
{
	// constants
	int thresh = 50;
	Mat kernel = (Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
	int smallImage_width = 128;
	int smallImage_height = 102;
	// vars declaration
	cv::Size smallSize(smallImage_width, smallImage_height);
	Mat diff, canny_output, dst;
	std::vector<Mat> smallImages;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// absolute diffrence between successive frames
	cv::absdiff(prv_frame, curr_frame, diff);

	// edge detection and normalization
	cv::Canny(diff, canny_output, thresh, thresh * 4, 3);
	cv::normalize(canny_output, canny_output, 0, 1, cv::NORM_MINMAX);

	// dilation and normalization
	cv::dilate(canny_output, dst, kernel);
	cv::dilate(dst, dst, kernel);
	cv::normalize(dst, dst, 0, 255, cv::NORM_MINMAX);

	// divide image int 10x10 blocks
	for (int y = 0; y < dst.rows - smallSize.height+1; y += smallSize.height)
	{
		for (int x = 0; x < dst.cols - smallSize.width+1; x += smallSize.width)
		{
			cv::Rect rect = cv::Rect(x, y, smallSize.width, smallSize.height);
			smallImages.push_back(cv::Mat(dst.clone(), rect));
		}
	}
	int smallimage_size = smallImages.size();
	int sum = 0;
	for (int i = 0; i < smallimage_size; ++i)
	{
		findContours(smallImages[i], contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		if (contours.size())
		{
			hittingData.push_back(contours.size());
		}
	}
	smallImages.clear();
}
Rect Main::getmovingRingROI(const Mat &curr_frame)
{
	Mat blank = cv::Mat::zeros(cv::Size(curr_frame.cols, curr_frame.rows), CV_8UC3);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Rect r = Rect(0, 0, 0, 0);
	Mat fgMask, fgMaskMovingRing;
	pMOG->operator()(curr_frame, fgMask, 0);
	if (updateModel)
	{
		pMOG->operator()(curr_frame, fgMask, 0.6);
		updateModel = false;
	}


	Mat movingToolRing, movingToolRing_hsv;
	curr_frame.copyTo(movingToolRing, fgMask);

	if (!fgMask.empty())
	{
		cvtColor(movingToolRing, movingToolRing_hsv, CV_BGR2HSV);

		Mat channel[3];
		split(movingToolRing_hsv, channel);

		//Mat fgMaskRing;
		cv::threshold(channel[0], fgMaskMovingRing, ring_hue_thresh_val, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
		cv::threshold(fgMaskMovingRing, fgMaskMovingRing, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;

		erode(fgMaskMovingRing, fgMaskMovingRing, element[0]);
		erode(fgMaskMovingRing, fgMaskMovingRing, element[0]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		
		//if (!fgMaskMovingRing.empty())
		//{
		//	imshow("fgMaskMovingRing", fgMaskMovingRing);
		//	waitKey(10);
		//}
		findContours(fgMaskMovingRing, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		
		// draw contours
		//drawContours(blank, contours, 0, Scalar(255, 0, 0), 2);
		//imshow("blank", blank);
		//waitKey(10);
		//
		//for (int i = 0; i < contours.size(); ++i)
		//{
		//	cout << "contour " << i << "-> size" << contours[i].size() << endl;
		//}
		//cout << endl;

		vector<bool> markedContours(contours.size(), false);
		vector<Point> maxContour;
		if (contours.size() > 0)
		{
			int Maxsize = contours[0].size();
			int index = 0;
			for (int i = 1; i < contours.size(); ++i)
			{
				int size = contours[i].size();
				if (Maxsize < size)
				{
					Maxsize = size;
					index = i;
				}
			}
			if (contours[index].size() > 70)
			{
				r = boundingRect(contours[index]);
			}
		}
	}
	return r;
}

void Main::activityDetection(const Mat &curr_frame, const Rect &trackingOut)
{
	
	int count1 = 0, count2 = 0;
	Mat drawing = curr_frame.clone();
	Mat curr_frame_hsv, fgMaskRing;
	// Do the ring segmentation of the current frame;
	cvtColor(curr_frame, curr_frame_hsv, CV_BGR2HSV);

	mvRingROI = getmovingRingROI(curr_frame);


	Mat channel[3];
	split(curr_frame_hsv, channel);


	//Mat fgMaskRing;
	cv::threshold(channel[0], fgMaskRing, ring_hue_thresh_val, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
	cv::threshold(fgMaskRing, fgMaskRing, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;

	erode(fgMaskRing, fgMaskRing, element[0]);
	dilate(fgMaskRing, fgMaskRing, element[1]);
	erode(fgMaskRing, fgMaskRing, element[0]);
	dilate(fgMaskRing, fgMaskRing, element[1]);
	Mat maskRing = fgMaskRing.clone();
	
	// *********  drawing ************************* 
	//Mat test;
	//cvtColor(fgMaskRing, test, CV_GRAY2BGR);
	//plot the pegs
	for (int i = 0; i < _pegBox.pegs.size(); ++i)
	{
		string text = SSTR(_pegBox.pegs[i].code);
		rectangle(drawing, _pegBox.pegs[i].roi, Scalar(0, 255, 0), 2);
		putText(drawing, text, _pegBox.pegs[i].center, FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	}
	// plot the tool tip
	if (trackingOut.width > 0)
	{
		rectangle(drawing, trackingOut, Scalar(0, 0, 255), 4);
		circle(drawing, Point(trackingOut.x + trackingOut.width, trackingOut.y + trackingOut.height + 40), 10, Scalar(0, 0, 255), 3, 8, 0);

	}
	//plot the moving ring
	if (mvRingROI.width > 0)
	{
		rectangle(drawing, mvRingROI, Scalar(255, 0, 255), 4);
		//putText(test, SSTR(dist), Point(mvRingROI.x + mvRingROI.width / 2, mvRingROI.y + mvRingROI.height/2), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);
	}
	//string filename = "D:/Official/Programs/endo_tracking/data/Result/" + SSTR(temp++) + ".png";
	//imwrite(filename, test);
	// *******************************************
	for (int i = 0; i < _ringBox.rings.size(); ++i)
	{
		string text = "Ring " + SSTR(i) + " Code " + SSTR(_ringBox.rings[i].getCode()) + "  Status " + _ringBox.rings[i].getStatus();
		putText(drawing, text, Point(10, (i * 15) + 100), FONT_HERSHEY_PLAIN, .7, Scalar(255, 255, 255), 1, 8, false);
	}
	imshow("output", drawing);
	waitKey(10);
	//_ringBox.update(fgMaskRing);
	
	unordered_map<int, double> graySum_firstHalf;
	unordered_map<int, double> graySum_secondHalf;
	for (int i = 0; i < _pegBox.pegs_firstHalf.size(); ++i)
	{
		int code = _pegBox.pegs_firstHalf[i].code;
		Rect roi = _pegBox.pegs_firstHalf[i].roi;
		Mat temp = maskRing(roi);
		double sumPixels = cv::sum(cv::sum(temp)).val[0];
		graySum_firstHalf[code] = sumPixels;
		//cout << "val -> " << sumPixels << endl;
	}
	for (int i = 0; i < _pegBox.pegs_secondHalf.size(); ++i)
	{
		int code = _pegBox.pegs_secondHalf[i].code;
		Rect roi = _pegBox.pegs_secondHalf[i].roi;
		Mat temp = maskRing(roi);
		double sumPixels = cv::sum(cv::sum(temp)).val[0];
		graySum_secondHalf[code] = sumPixels;
		//cout << "val -> " << sumPixels << endl;
	}

	//cout << endl << endl;
	if (status == "stationary")
	{
		if (no_of_rings_moved_firstHalf == 6)
		{
			// All rings moved; swap graySum_firstHalf and graySum_secondHalf; reset counter
			firstTimeRingDetection = false;
			cout << "mvRingROI.width  " << mvRingROI.width << endl;


			_pegBox.SwapFirstAndSecondHalf();

			no_of_rings_moved_firstHalf = 0;
			no_of_rings_placed_secondHalf = 0;

			// swap the content of the state vector
		}
		
		for (auto it = graySum_firstHalf.begin(); it != graySum_firstHalf.end(); ++it)
		{
			if (it->second == 0)
			{
				count1++;
			}
		}
		for (auto it = graySum_secondHalf.begin(); it != graySum_secondHalf.end(); ++it)
		{
			if (it->second > 1.2e6)
			{
				count2++;
			}
		}
		if (count1 == no_of_rings_moved_firstHalf)
		{
			// Determine whether stationary or picking status
			if (mvRingROI.width > 0)
			{
				if (firstTimeRingDetection)
				{
					status = "picking";
					// Find intersection of moving Ring ROI with the first half and determine which
					// Ring trainee is trying to pick up; then change the code of the ring
					for (int i = 0; i < _pegBox.pegs_firstHalf.size(); ++i)
					{
						Rect r = _pegBox.pegs_firstHalf[i].roi;
						int code = _pegBox.pegs_firstHalf[i].code;
						bool intersect = _ringBox.rectOverlap(r, mvRingROI);
						if (intersect)
						{
							for (int p = 0; p < _ringBox.rings.size(); ++p)
							{
								if (code == _ringBox.rings[p].getCode())
								{
									_ringBox.rings[p].setStatus(status);
									break;
								}
							}
							break;
						}
					}
				}
				else
				{
					firstTimeRingDetection = true;
					status = "stationary";
				}
			}
			else
			{
				status = "stationary";
			}
		}

		if (count2 < no_of_rings_placed_secondHalf)
		{

		}
	}
	
	else if (status == "picking")
	{
		for (auto it = graySum_firstHalf.begin(); it != graySum_firstHalf.end(); ++it)
		{
			if (it->second == 0)
			{
				count1++;
			}
		}
		for (auto it = graySum_secondHalf.begin(); it != graySum_secondHalf.end(); ++it)
		{
			if (it->second > 1.2e6)
			{
				count2++;
			}
		}
		if (count1 == no_of_rings_moved_firstHalf)
		{
			status = "picking";
		}
		if (count1 > no_of_rings_moved_firstHalf)
		{
			status = "moving";
			no_of_rings_moved_firstHalf = count1;
			for (int i = 0; i < _ringBox.rings.size(); ++i)
			{
				if (_ringBox.rings[i].getStatus() == "picking")
				{
					_ringBox.rings[i].setStatus("moving");
					break;
				}
			}
		}
		else if (no_of_rings_moved_firstHalf == 6)
		{
			// all rings moved
		}
	}
	else if (status == "moving")
	{
		for (auto it = graySum_firstHalf.begin(); it != graySum_firstHalf.end(); ++it)
		{
			if (it->second == 0)
			{
				count1++;
			}
		}
		for (auto it = graySum_secondHalf.begin(); it != graySum_secondHalf.end(); ++it)
		{
			if (it->second > 1.2e6)
			{
				count2++;
			}
		}
		if (count2 > no_of_rings_placed_secondHalf)
		{
			// use tracking output and moving ring output to determine weather ring is placed or not
			double threshold_distance;
			Point center_ring = Point(mvRingROI.x + (mvRingROI.width / 2.0), mvRingROI.y + mvRingROI.height / 2.0);
			Point center_tooltip = Point(trackingOut.x + trackingOut.width, trackingOut.y + trackingOut.height + 40);
			if (center_ring.x > 0 && center_ring.y > 0)
			{
				dist = sqrt(((center_ring.x - center_tooltip.x)*(center_ring.x - center_tooltip.x)) + ((center_ring.y - center_tooltip.y)*(center_ring.y - center_tooltip.y)));
				if (dist > 150)
				{
					status = "stationary";
					no_of_rings_placed_secondHalf = count2;
					updateModel = true;

					int index_peg = 0;
					for (int i = 0; i < _pegBox.pegs_secondHalf.size(); ++i)
					{
						Rect r = _pegBox.pegs_secondHalf[i].roi;
						int code = _pegBox.pegs_secondHalf[i].code;
						bool intersect = _ringBox.rectOverlap(r, mvRingROI);
						if (intersect)
						{
							index_peg = i;
							break;
						}
					}
					// update the code and status of the ring
					for (int i = 0; i < _ringBox.rings.size(); ++i)
					{
						if (_ringBox.rings[i].getStatus() == "moving")
						{
							_ringBox.rings[i].setStatus("stationary");
							_ringBox.rings[i].setCode(_pegBox.pegs_secondHalf[index_peg].code);
							break;
						}
					}
				}
			}
		}
		else
		{
			status = "moving";
		}
	}
	
	cout << "no_of_rings_moved_firstHalf -> " << no_of_rings_moved_firstHalf << endl;
	cout << "no_of_rings_placed_secondHalf -> " << no_of_rings_placed_secondHalf << endl;
	cout << "status -> " << status << endl << endl;


}
void Main::doWork()
{
	Trajectory trajectory;
    IplImage *img = imAcqGetImg(imAcq);
	IplImage *img1 = cvCloneImage(img);
	IplImage *img2 = cvCloneImage(img);
    Mat grey(img->height, img->width, CV_8UC1);
    cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);

    tld->detectorCascade->imgWidth = grey.cols;
    tld->detectorCascade->imgHeight = grey.rows;
	tld->detectorCascade->imgWidthStep = grey.step;

	// To get the bounding rectangle around the peg Group
	{
		string message = "Press Enter to confirm the bounding box or any other key to redraw the bounding box";
		cvRectangle(img2, pegGroupROI.tl(), pegGroupROI.br(), CV_RGB(255, 0, 0), 3);
		CvFont font1;
		cvInitFont(&font1, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
		cvPutText(img2, message.c_str(), cvPoint(0, 60), &font1, cvScalar(255, 255, 0));
		gui->showImage(img2);
		char key1 = cvWaitKey(0);
		if ((key1 != '\r' && key1 != '\n' && key1 != '\r\n') || !(pegGroupROI.x != -1) || !(pegGroupROI.y != -1))
		{
			CvRect box;
			cvReleaseImage(&img2);
			img2 = cvCloneImage(img);
			string message = "Draw a bounding box around the pegs and press enter";
			if (getBBFromUser(img2, box, gui, message) == PROGRAM_EXIT)
			{
				return;
			}
			pegGroupROI = Rect(box);
		}

	}

	if (initializePeg)
	{
		string message = "Press Enter to confirm the bounding box on the peg or any other key to redraw";
		cvReleaseImage(&img2);
		img2 = cvCloneImage(img);
		cvRectangle(img2, pegROI.tl(), pegROI.br(), CV_RGB(255, 0, 0), 3);
		CvFont font1;
		cvInitFont(&font1, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
		cvPutText(img2, message.c_str(), cvPoint(0, 60), &font1, cvScalar(255, 255, 0));
		gui->showImageByDestroyingWindow(img2);

		char key1 = cvWaitKey(0);
		if ((key1 != '\r' && key1 != '\n' && key1 != '\r\n') || !(pegROI.x != -1) || !(pegROI.y != -1))
		{
			CvRect box;
			cvReleaseImage(&img2);
			img2 = cvCloneImage(img);
			string message = "Draw a bounding box on the peg and press enter";
			if (getBBFromUser(img2, box, gui, message) == PROGRAM_EXIT)
			{
				return;
			}
			pegROI = Rect(box);
			cvReleaseImage(&img2);
			img2 = cvCloneImage(img);
		}


		Mat src, im1, im2, im3, fgMaskPeg;
		src = Mat(img);
		im1 = src(pegROI);

		cvtColor(im1, im2, CV_BGR2HSV);
		cvtColor(src, im3, CV_BGR2HSV);

		Scalar sumData = cv::sum(im2);
		for (int i = 0; i < 3; i++)
		{
			sumData.val[i] /= im2.rows * im2.cols;
		}
		int thresh_val_peg = getPegthresholdFromUser(img, gui, "Adjust the trackbar above to get all 12 pegs and press enter", sumData.val[2], pegGroupROI, fgMaskPeg);
		_pegBox.init(fgMaskPeg);
	}
	else
	{
		Mat fgMaskPeg;
		int thresh_val_peg = getPegthresholdFromUser(img, gui, "Adjust the trackbar above to get all 12 pegs and press enter", PegSaturation_minValue, pegGroupROI, fgMaskPeg);
		_pegBox.init(fgMaskPeg);
	}

	

	if (initializeRing)
	{
		string message = "Press Enter to confirm the bounding box on the Ring or any other key to redraw";
		cvReleaseImage(&img2);
		img2 = cvCloneImage(img);
		cvRectangle(img2, ringROI.tl(), ringROI.br(), CV_RGB(255, 255, 0), 3);
		CvFont font1;
		cvInitFont(&font1, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
		cvPutText(img2, message.c_str(), cvPoint(0, 60), &font1, cvScalar(255, 255, 0));
		gui->showImageByDestroyingWindow(img2);

		char key1 = cvWaitKey(0);
		if ((key1 != '\r' && key1 != '\n' && key1 != '\r\n') || !(ringROI.x != -1) || !(ringROI.y != -1))
		{
			CvRect box;
			cvReleaseImage(&img2);
			img2 = cvCloneImage(img);
			string message = "Draw a bounding box on the Ring and press enter";
			if (getBBFromUser(img2, box, gui, message) == PROGRAM_EXIT)
			{
				return;
			}
			ringROI = Rect(box);
			cvReleaseImage(&img2);
			img2 = cvCloneImage(img);
		}


		Mat src, im1, im2, im3, fgMaskRing;
		src = Mat(img);
		im1 = src(ringROI);

		cvtColor(im1, im2, CV_BGR2HSV);
		cvtColor(src, im3, CV_BGR2HSV);

		Scalar sumData = cv::sum(im2);
		for (int i = 0; i < 3; i++)
		{
			sumData.val[i] /= im2.rows * im2.cols;
		}
		int thresh_val_Ring = getRingthresholdFromUser(img, gui, "Adjust the trackbar above to get all 6 Rings and press enter", sumData.val[0], pegGroupROI, fgMaskRing);
		ring_hue_thresh_val = thresh_val_Ring;
		_ringBox.update(fgMaskRing);
		_ringBox.updateCode(_pegBox);

		int rois_size = _ringBox.rings.size();
		vector<pair<Rect, int> > roi_rings(rois_size);
		for (int i = 0; i < rois_size; ++i)
		{
			roi_rings[i].first = _ringBox.rings[i].getRoi();
			roi_rings[i].second = _ringBox.rings[i].getCode();
		}
		_pegBox.updateRoiCenter(roi_rings);


	}
	else
	{


		Mat fgMaskRing;
		int thresh_val_Ring = getRingthresholdFromUser(img, gui, "Adjust the trackbar above to get all 6 rings and press enter", ring_hue_thresh_val, pegGroupROI, fgMaskRing);
		ring_hue_thresh_val = thresh_val_Ring;
		//imshow("fgMaskRing", fgMaskRing);
		//waitKey(0);
		_ringBox.update(fgMaskRing);
		_ringBox.updateCode(_pegBox);


		int rois_size = _ringBox.rings.size();
		vector<pair<Rect, int> > roi_rings(rois_size);
		for (int i = 0; i < rois_size; ++i)
		{
			roi_rings[i].first = _ringBox.rings[i].getRoi();
			roi_rings[i].second = _ringBox.rings[i].getCode();
		}
		_pegBox.updateRoiCenter(roi_rings);

	}
	//Mat temp = Mat(img1);
	//draw(temp);


	if(showTrajectory)
	{
		trajectory.init(trajectoryLength);
	}

    if(selectManually)
    {

        CvRect box;
		string message = "Draw a bounding box and press enter";
        if(getBBFromUser(img, box, gui, message) == PROGRAM_EXIT)
        {
            return;
        }

        if(initialBB == NULL)

        {
            initialBB = new int[4];
        }

        initialBB[0] = box.x;
        initialBB[1] = box.y;
        initialBB[2] = box.width;
        initialBB[3] = box.height;
    }

    FILE *resultsFile = NULL;
    if(printResults != NULL)
    {
        resultsFile = fopen(printResults, "w");
        if(!resultsFile)
        {
            fprintf(stderr, "Error: Unable to create results-file \"%s\"\n", printResults);
            exit(-1);
        }
    }

    bool reuseFrameOnce = false;
    bool skipProcessingOnce = false;

    if(loadModel && modelPath != NULL)
    {
        tld->readFromFile(modelPath);
        reuseFrameOnce = true;
    }
    else if(initialBB != NULL)
    {
        Rect bb = tldArrayToRect(initialBB);

        printf("Starting at %d %d %d %d\n", bb.x, bb.y, bb.width, bb.height);

        tld->selectObject(grey, &bb);
        skipProcessingOnce = true;
        reuseFrameOnce = true;
    }

	prv_frame = Mat(img1);
	if (printResults != NULL)
	{
		// 
		fprintf(resultsFile, "Output-File for the video -> %s\n", imAcq->imgPath);
		fprintf(resultsFile, "Locations and code of the Pegs (x,y,width,height,code)\n");
		for (int i = 0; i < _pegBox.pegs.size(); ++i)
		{
			fprintf(resultsFile, "peg %d -> (%d, %d, %d, %d, %d)\n", i + 1, _pegBox.pegs[i].roi.x, _pegBox.pegs[i].roi.y, _pegBox.pegs[i].roi.width, _pegBox.pegs[i].roi.height, _pegBox.pegs[i].code);
		}
		fprintf(resultsFile, "\n\nFrame-No, Ring1-Status, Ring1-Code, Ring2-Status, Ring2-Code, Ring3-Status, Ring3-Code, Ring4-Status, Ring4-Code, Ring5-Status, Ring5-Code, Ring6-Status, Ring6-Code, Hitting-Detection-Output, Tracking-Output, Tracking-Output-Confidence\n");
	}
	gui->showImageByDestroyingWindow(img);
    while(imAcqHasMoreFrames(imAcq))
    {
        double tic = cvGetTickCount();
        if(!reuseFrameOnce)
        {
            cvReleaseImage(&img);
            img = imAcqGetImg(imAcq);

            if(img == NULL)
            {
                printf("current image is NULL, assuming end of input.\n");
                break;
            }

            cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);
        }

		/*  ********** Hitting Detection *****************  
		Calculate the adsolute difference between two successive frames
		divide image into 10x10 subimage
		calculate in how many subimages contours are dected
		* ****************************************************/
		// Detection for the hitting on the board
		curr_frame = Mat(img);
		vector<unsigned int > frameHitData;
		HittingDetection(prv_frame, curr_frame, frameHitData);
		hittingData.push_back(frameHitData);
		// Replace the previous with current frame and reset frameHitData
		prv_frame = curr_frame.clone();
		
		/***********************************************************

		/*  ********** Activity Detection *****************
		Detect activity in each frame;
		1. segment the rings
		2. check overlap of the ring with pegs
		3. if any overlap is zero assign moving code to that ring
		4. if placed assign code of corresponding peg
		input: current frame and tracking output
		output : vector<int> state; describes the state of the system
		*****************************************************/
		if (tld->currBB != NULL)
		{
			activityDetection(curr_frame, Rect(tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height));
		}
		else
		{
			activityDetection(curr_frame, Rect(0,0,0,0));
		}
		//******************************************************

		
		// Writing results
		if (printResults != NULL)
		{
			if (tld->currBB != NULL)
			{
				fprintf(resultsFile, "%d, %s, %d, %s, %d,%s, %d,%s, %d,%s, %d,%s, %d, %d, (%.2d, %.2d, %.2d, %.2d), %lf\n",
					imAcq->currentFrame - 1,
					_ringBox.rings[0].getStatus().c_str(), _ringBox.rings[0].getCode(),
					_ringBox.rings[1].getStatus().c_str(), _ringBox.rings[1].getCode(),
					_ringBox.rings[2].getStatus().c_str(), _ringBox.rings[2].getCode(),
					_ringBox.rings[3].getStatus().c_str(), _ringBox.rings[3].getCode(),
					_ringBox.rings[4].getStatus().c_str(), _ringBox.rings[4].getCode(),
					_ringBox.rings[5].getStatus().c_str(), _ringBox.rings[5].getCode(),
					frameHitData.size(),
					tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height,
					tld->currConf);
			}
			else
			{
				fprintf(resultsFile, "%d, %s, %d, %s, %d,%s, %d,%s, %d,%s, %d,%s, %d, %d, (NaN, NaN, NaN, NaN), NaN\n",
					imAcq->currentFrame - 1,
					_ringBox.rings[0].getStatus().c_str(), _ringBox.rings[0].getCode(),
					_ringBox.rings[1].getStatus().c_str(), _ringBox.rings[1].getCode(),
					_ringBox.rings[2].getStatus().c_str(), _ringBox.rings[2].getCode(),
					_ringBox.rings[3].getStatus().c_str(), _ringBox.rings[3].getCode(),
					_ringBox.rings[4].getStatus().c_str(), _ringBox.rings[4].getCode(),
					_ringBox.rings[5].getStatus().c_str(), _ringBox.rings[5].getCode(),
					frameHitData.size());
			}
		}

		frameHitData.clear();
		if(!skipProcessingOnce)
        {
            tld->processImage(cvarrToMat(img));
        }
        else
        {
            skipProcessingOnce = false;
        }



        double toc = (cvGetTickCount() - tic) / cvGetTickFrequency();

        toc = toc / 1000000;

        float fps = 1 / toc;

        int confident = (tld->currConf >= threshold) ? 1 : 0;

        if(showOutput || saveDir != NULL)
        {
            char string[128];

            char learningString[10] = "";

            if(tld->learning)
            {
                strcpy(learningString, "Learning");
            }

            sprintf(string, "#%d,Posterior %.2f; fps: %.2f, #numwindows:%d, %s", imAcq->currentFrame - 1, tld->currConf, fps, tld->detectorCascade->numWindows, learningString);
            CvScalar yellow = CV_RGB(255, 255, 0);
            CvScalar blue = CV_RGB(0, 0, 255);
            CvScalar black = CV_RGB(0, 0, 0);
			CvScalar white = CV_RGB(255, 255, 255);
			CvScalar red = CV_RGB(255, 0, 0);

            if(tld->currBB != NULL)
            {
                CvScalar rectangleColor = (confident) ? blue : yellow;
                cvRectangle(img, tld->currBB->tl(), tld->currBB->br(), rectangleColor, 2, 8, 0);
				cvCircle(img, cvPoint(tld->currBB->br().x /*- (tld->currBB->width / 2.0)*/, tld->currBB->br().y + 40), 10, red, 3, 8, 0);

				if(showTrajectory)
				{
					CvPoint center = cvPoint(tld->currBB->x+tld->currBB->width/2, tld->currBB->y+tld->currBB->height/2);
					cvLine(img, cvPoint(center.x-2, center.y-2), cvPoint(center.x+2, center.y+2), rectangleColor, 2);
					cvLine(img, cvPoint(center.x-2, center.y+2), cvPoint(center.x+2, center.y-2), rectangleColor, 2);
					trajectory.addPoint(center, rectangleColor);
				}
            }
			else if(showTrajectory)
			{
				trajectory.addPoint(cvPoint(-1, -1), cvScalar(-1, -1, -1));
			}

			if(showTrajectory)
			{
				trajectory.drawTrajectory(img);
			}

            CvFont font;
            cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
            cvRectangle(img, cvPoint(0, 0), cvPoint(img->width, 50), black, CV_FILLED, 8, 0);
            cvPutText(img, string, cvPoint(25, 25), &font, white);

            if(showForeground)
            {

                for(size_t i = 0; i < tld->detectorCascade->detectionResult->fgList->size(); i++)
                {
                    Rect r = tld->detectorCascade->detectionResult->fgList->at(i);
                    cvRectangle(img, r.tl(), r.br(), white, 1);
                }

            }
            if(showOutput)
            {
                gui->showImage(img);
                char key = gui->getKey();

                if(key == 'q') break;

                if(key == 'b')
                {

                    ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;

                    if(fg->bgImg.empty())
                    {
                        fg->bgImg = grey.clone();
                    }
                    else
                    {
                        fg->bgImg.release();
                    }
                }

                if(key == 'c')
                {
                    //clear everything
                    tld->release();
                }

                if(key == 'l')
                {
                    tld->learningEnabled = !tld->learningEnabled;
                    printf("LearningEnabled: %d\n", tld->learningEnabled);
                }

                if(key == 'a')
                {
                    tld->alternating = !tld->alternating;
                    printf("alternating: %d\n", tld->alternating);
                }

                if(key == 'e')
                {
                    tld->writeToFile(modelExportFile);
                }

                if(key == 'i')
                {
                    tld->readFromFile(modelPath);
                }

                if(key == 'r')
                {
                    CvRect box;
					std::string message = "Draw a bounding box and press enter";
                    if(getBBFromUser(img, box, gui, message) == PROGRAM_EXIT)
                    {
                        break;
                    }

                    Rect r = Rect(box);

                    tld->selectObject(grey, &r);
                }
            }

            if(saveDir != NULL)
            {
                char fileName[256];
                sprintf(fileName, "%s/%.5d.png", saveDir, imAcq->currentFrame - 1);

                cvSaveImage(fileName, img);
            }
        }

        if(reuseFrameOnce)
        {
            reuseFrameOnce = false;
        }
    }

    cvReleaseImage(&img);
    img = NULL;

    if(exportModelAfterRun)
    {
        tld->writeToFile(modelExportFile);
    }

    if(resultsFile)
    {
        fclose(resultsFile);
    }
}

