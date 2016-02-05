#ifndef CONTOURS_FEATURE_HPP
#define CONTOURS_FEATURE_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>

class ContourFeature {

public:
  //Default constructor
  ContourFeature();
  
  //Constructs ContourFeature from a set of points
  ContourFeature(const std::vector<cv::Point>& contour);
  
  //Copy constructor
  ContourFeature(const ContourFeature&);
  
  
  //ContourFeature& operator=(const ContourFeature& cf) {
  //  this->mom = cf.mom;
  //  this->eccentricity = cf.eccentricity;
  //  this->perimeter = cf.perimeter;
  //  this->areaRatio = cf.areaRatio;
  //  this->hull = cf.hull;
  //  this->hullArea = cf.hullArea;
  //  this->convexRatio = cf.convexRatio;
  //  this->centre = cf.centre;
  //  this->solidity = cf.solidity;


  //}

  //Update the contours features
  void update(const std::vector<cv::Point>& contour);

  //Return contour caracteristics
  const cv::Point2d& getCentre() const;
  double getArea() const;
  double getPerimeter() const;
  double getEccentricity() const;
  double getHullArea() const;
  double getAreaRatio() const;  
  double getConvexRatio() const;
  double getSolidity() const;

  //Display contour caracteristics
  void info() const;

protected:
  double perimeter;
  cv::Point2d centre;
  cv::Moments mom;
  std::vector<cv::Point> hull;
  double area;
  double eccentricity;
  double areaRatio;
  double hullArea;
  double solidity;//1 if non-deformable shape
  double convexRatio;
};
#endif
