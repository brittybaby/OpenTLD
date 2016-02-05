#include "ContourFeature.hpp"

ContourFeature::ContourFeature():mom(),centre(),hull() 
{
  area = 0;
  eccentricity = 0;
  perimeter = 0;
  areaRatio = 0;
  hullArea = 0;
  convexRatio = 0;
  solidity = 0;
}

ContourFeature::ContourFeature(const ContourFeature& cf):
  mom(cf.mom),hull(cf.hull) 
{

	  this->area = cf.area;
	  this->eccentricity =  ((pow(mom.mu20 - mom.mu02, 2)) 
				 - (4*pow(mom.mu11, 2))) 
		/ (pow(mom.mu20 + mom.mu02, 2));
		cf.eccentricity;
	  this->perimeter = cf.perimeter;
	  this->areaRatio = cf.areaRatio;
	  this->hullArea = cf.hullArea;
	  this->convexRatio = cf.convexRatio;
	  cv::Point c = cf.getCentre();
	  this->centre.x = c.x;
	  this->centre.y = c.y;
	  this->solidity = cf.solidity;
}

ContourFeature::ContourFeature(const std::vector<cv::Point>& contour) 
{
	  mom = moments(cv::Mat(contour));
	  area = mom.m00;
	  eccentricity = ((pow(mom.mu20 - mom.mu02, 2)) 
			  - (4*pow(mom.mu11, 2))) 
		/ (pow(mom.mu20 + mom.mu02, 2));
	  perimeter = cv::arcLength(cv::Mat(contour), true);
	  areaRatio = (4 * CV_PI * area) / (perimeter*perimeter);
	  convexHull(cv::Mat(contour), hull);
	  hullArea = cv::contourArea(cv::Mat(hull));
	  convexRatio = area / hullArea;
	  centre = cv::Point2d(mom.m10 / mom.m00, mom.m01 / mom.m00);
	  solidity = area/hullArea;
}

void ContourFeature::update(const std::vector<cv::Point>& contour)
{
	  hull.clear();
  
	  mom = moments(cv::Mat(contour));
	  area = mom.m00;
	  eccentricity = ((pow(mom.mu20 - mom.mu02, 2)) 
  			  - (4*pow(mom.mu11, 2))) 
		/ (pow(mom.mu20 + mom.mu02, 2));
	  perimeter = cv::arcLength(cv::Mat(contour), true);
	  areaRatio = (4 * CV_PI * area) / (perimeter*perimeter);
	  convexHull(cv::Mat(contour), hull);
	  hullArea = cv::contourArea(cv::Mat(hull));
	  convexRatio = area / hullArea;
	  centre = cv::Point2d(mom.m10 / mom.m00, mom.m01 / mom.m00);
	  solidity = area/hullArea;

}


void ContourFeature::info() const 
{
	  std::cout << "Perimeter: " << perimeter << std::endl;
	  std::cout << "Centre : (" << centre.x << "; " << centre.y << ")" << std::endl;
	  std::cout << "Area: " << area << std::endl;
	  std::cout << "Eccentricity: " << eccentricity << std::endl;
	  std::cout << "Area ratio: " << areaRatio << std::endl;
	  std::cout << "Convex hull area: " << hullArea << std::endl;
	  std::cout << "Convex ratio: " << convexRatio  << std::endl;  
	  std::cout << "Solidity: " << solidity  << std::endl;  
	  std::cout << std::endl << std::endl;
}

const cv::Point2d& ContourFeature::getCentre() const 
{
  return centre;
}

double ContourFeature::getArea() const 
{
  return area;
}

double ContourFeature::getPerimeter() const 
{
  return perimeter;
}

double ContourFeature::getEccentricity() const 
{
  return eccentricity;
}

double ContourFeature::getHullArea() const 
{
  return hullArea;
}

double ContourFeature::getConvexRatio() const 
{
  return convexRatio;
}

double ContourFeature::getAreaRatio() const 
{
  return areaRatio;
}

double ContourFeature::getSolidity() const 
{
  return solidity;
}
