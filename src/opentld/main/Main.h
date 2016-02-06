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
 * main.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Georg Nebehay
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "TLD.h"
#include "ImAcq.h"
#include "Gui.h"
#include "RingBox.h"
#include "ContourFeature.hpp"
#include "pegBox.h"
#include <string>
#include <sstream>

using namespace std;
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()
enum Retval
{
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

class Main
{
public:
    tld::TLD *tld;
    ImAcq *imAcq;
    tld::Gui *gui;
    bool showOutput;
	bool showTrajectory;
	int trajectoryLength;
    const char *printResults;
    const char *saveDir;
    double threshold;
    bool showForeground;
    bool showNotConfident;
    bool selectManually;
    int *initialBB;
    bool reinit;
    bool exportModelAfterRun;
    bool loadModel;
    const char *modelPath;
    const char *modelExportFile;
    int seed;
	bool initializePeg;
	bool initializeRing;
	double RingHueValue;
	double PegSaturation_minValue, PegSaturation_maxValue;
	
	vector<vector<unsigned int> > hittingData; // for each frame no of small images having contours
	Mat prv_frame, curr_frame;
	void doWork();
	void draw(Mat &img);
	void HittingDetection(const Mat &prv_frame, const Mat &curr_frame, vector<unsigned int>  &hittingData);
	void activityDetection(const Mat &curr_frame, const Rect &trackingOut);
	Rect getmovingRingROI(const Mat &curr_frame);
	// pegBox and ringBox
	RingBox _ringBox;
	pegBox _pegBox;
	double ring_hue_thresh_val;
	cv::Mat element[4];
	string status;
	int no_of_rings_moved_firstHalf, no_of_rings_placed_secondHalf;
	cv::BackgroundSubtractor* pMOG;
	bool updateModel;
	double dist;
	int temp;
	bool firstTimeRingDetection;
	Rect mvRingROI;
	Rect pegGroupROI;
	Rect pegROI;
	Rect ringROI;
	bool first_tracking_failed_detection;
	bool trackingStart;
	Main()
    {
		element[0] = getStructuringElement(MORPH_CROSS, Size(5, 5), Point(0, 0));
		element[1] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));
		element[2] = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(0, 0));
		element[3] = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(0, 0));

        tld = new tld::TLD();
        showOutput = 1;
        printResults = NULL;
        saveDir = ".";
        threshold = 0.5;
        showForeground = 0;
		namedWindow("output", CV_WINDOW_AUTOSIZE);
		moveWindow("output", 100, 100);
		showTrajectory = false;
		trajectoryLength = 0;

        selectManually = 0;

        initialBB = NULL;
        showNotConfident = true;

        reinit = 0;

        loadModel = false;

        exportModelAfterRun = false;
        modelExportFile = "model";
        seed = 0;

        gui = NULL;
        modelPath = NULL;
        imAcq = NULL;
		initializePeg = true;
		initializeRing = true;
		RingHueValue = 20.0;
		PegSaturation_minValue = 120;
		PegSaturation_maxValue = 255;
		ring_hue_thresh_val = 20.0;
		status = "stationary";
		no_of_rings_moved_firstHalf = 0;
		no_of_rings_placed_secondHalf = 0;
		pMOG = new BackgroundSubtractorMOG2(20, 100, false);
		updateModel = false;
		dist = 0;
		temp = 0;
		firstTimeRingDetection = false;
		mvRingROI = Rect(0, 0, 0, 0);
		pegGroupROI = Rect(160, 240, 830, 550);
		pegROI = Rect(476, 258, 20, 47);
		ringROI = Rect(526, 595, 17, 21);
		first_tracking_failed_detection = false;
		trackingStart = false;
	}

    ~Main()
    {
        delete tld;
        imAcqFree(imAcq);
		delete pMOG;
    }

};

#endif /* MAIN_H_ */
