#include "ShadowManager.h"
#include "Utilities.h"
#include <math.h>
#include <algorithm>

sf::Vector2f ShadowManager::GetIntersectionPoint(const sf::Vector2f& x1, const sf::Vector2f& x2, const sf::Vector2f& y1, const sf::Vector2f& y2)
{
	///< Get line in form: y = mx + b
	float m1;
	if (x1.x - x2.x == 0)
	{
		m1 = 999999;
	}
	else
	{
		m1 = (x1.y - x2.y) / (x1.x - x2.x);
	}
	float b1 = x2.y - m1 * x2.x;
	
	///< Get line in form: y = mx + b
	float m2;
	
	if (y1.x - y2.x == 0)
	{
		m2 = 999999;
	}
	else
	{
		m2 = (y1.y - y2.y) / (y1.x - y2.x);
	}
	
	float b2 = y2.y - m2 * y2.x;
	
	if (m1 - m2 == 0)
	{
		//no intersection
		//return y1;
	}
	float xIntercect = (b2-b1) / (m1-m2);
	float yIntercect = m1 * xIntercect + b1;
	
	return sf::Vector2f(xIntercect, yIntercect);
	
}

bool isPointOnLineSegment(const sf::Vector2f& p, const sf::Vector2f& a1, const sf::Vector2f& a2)
{
	// a1---a2
	//  \   /
	//   \ /
	//    p
	
	return fabsf((Norm(a1-p) + Norm(a2-p)) - Norm(a1-a2)) < 0.01;
}

sf::Vector2f getMinimumDistfromPoint(const sf::Vector2f& p, const std::vector<sf::Vector2f>& points)
{
	if (points.empty())
	{
		return sf::Vector2f(0,0);
	}
	sf::Vector2f min = points.front();
	float minDistSquared = NormSquared(p-min);
	
	for (const sf::Vector2f& point : points)
	{
		float thisDistSquared = NormSquared(p-point);
		if (thisDistSquared < minDistSquared)
		{
			minDistSquared = thisDistSquared;
			min = point;
		}
	}
	return min;
}
//----------------------------------------------------------------------------
typedef const sf::Vector2f& Point;

///< Code enclosed here is not mine
bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;
 
	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r)
{
	// See 10th slides from following link for derivation of the formula
	// http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf
	int val = (q.y - p.y) * (r.x - q.x) -
	(q.x - p.x) * (r.y - q.y);
 
	if (val == 0) return 0;  // colinear
 
	return (val > 0)? 1: 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);
 
	// General case
	if (o1 != o2 && o3 != o4)
		return true;
 
	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
 
	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
 
	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
 
	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;
 
	return false; // Doesn't fall in any of the above cases
}

//--------------------------------------------------------------------



typedef double coord2_t;  // must be big enough to hold 2*max(|coordinate|)^2
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
coord2_t cross(const sf::Vector2f &O, const sf::Vector2f &A, const sf::Vector2f &B)
{
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

bool vectorCompare(sf::Vector2f& a, sf::Vector2f& b)
{
	return a.x < b.x || (a.x == b.x && a.y < b.y);
}

// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
std::vector<sf::Vector2f> convex_hull(std::vector<sf::Vector2f>& P)
{
	int n = P.size(), k = 0;
	std::vector<sf::Vector2f> H(2*n);
 
	// Sort points lexicographically
	sort(P.begin(), P.end(), vectorCompare);
 
	// Build lower hull
	for (int i = 0; i < n; ++i) {
		while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	// Build upper hull
	for (int i = n-2, t = k+1; i >= 0; i--) {
		while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	H.resize(k);
	return H;
}



ShadowManager::ShadowManager(sf::View* view):
pView(view)
{
	
}

void ShadowManager::addShape(sf::ConvexShape *newShape)
{
	mShadowCasters.push_back(newShape);
	mShadows.push_back(new sf::ConvexShape);
}

void ShadowManager::updateShadows(const sf::Vector2f& newLightPos)
{
	lightPos = newLightPos;
	
	///< Update screen corners for shadow drawing
	const sf::Vector2f& viewPos = pView->getCenter();
	const sf::Vector2f& viewDimentions = pView->getSize();
	mScreenCorners.at(0) = viewPos + sf::Vector2f(-0.5f*viewDimentions.x-1, -0.5f*viewDimentions.y-1); //Top Left
	mScreenCorners.at(1) = viewPos + sf::Vector2f(0.5f*viewDimentions.x, -0.5f*viewDimentions.y); //Top Right
	mScreenCorners.at(2) = viewPos + sf::Vector2f(0.5f*viewDimentions.x+1, 0.5f*viewDimentions.y+1); //Bottom Right
	mScreenCorners.at(3) = viewPos + sf::Vector2f(-0.5f*viewDimentions.x, 0.5f*viewDimentions.y); //Bottom Left
	
	sf::Vector2f topEdgeVec = mScreenCorners.at(0) - mScreenCorners.at(1);
	sf::Vector2f rightEdgeVec = mScreenCorners.at(1) - mScreenCorners.at(2);
	sf::Vector2f botEdgeVec = mScreenCorners.at(2) - mScreenCorners.at(3);
	sf::Vector2f leftEdgeVec = mScreenCorners.at(0) - mScreenCorners.at(3);
	
	///< Create a ray from the light source through each vertex of each object to the endge of the screen
	std::vector<sf::ConvexShape*>::iterator shadow = mShadows.begin();
	for (const sf::ConvexShape* shape : mShadowCasters)
	{
		///< Create a shape for the shadow
		(*shadow)->setFillColor(sf::Color::Black);
		std::vector<sf::Vector2f>	shadowPoints;
		
		int numVerticies = shape->getPointCount();
		
		for (int i = 0; i < numVerticies; i++)
		{
			sf::Vector2f v = shape->getPoint(i);
			
			///< Get intersection on each face
			sf::Vector2f topIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[0], mScreenCorners[1]);
			sf::Vector2f rightIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[1], mScreenCorners[2]);
			sf::Vector2f botIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[2], mScreenCorners[3]);
			sf::Vector2f leftIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[3], mScreenCorners[0]);
			
			///< Choose the minimal intersection point for all walls
			sf::Vector2f minimalIntersection;
			
			if (v.y <= lightPos.y && v.x >= lightPos.x) //Bottom left quad (vertex is the orgin)
			{
				if (NormSquared(topIntersect) < NormSquared(rightIntersect))
				{
					minimalIntersection = topIntersect;
				} else
				{
					minimalIntersection = rightIntersect;
				}
			} else if (v.y <= lightPos.y && v.x <= lightPos.x) //Bottom right quad
			{
				if (NormSquared(topIntersect) < NormSquared(leftIntersect))
				{
					minimalIntersection = topIntersect;
				} else
				{
					minimalIntersection = leftIntersect;
				}
			} else if (v.y >= lightPos.y && v.x >= lightPos.x) //Top left quad
			{
				if (NormSquared(botIntersect) < NormSquared(rightIntersect))
				{
					minimalIntersection = botIntersect;
				} else
				{
					minimalIntersection = rightIntersect;
				}
			} else if (v.y >= lightPos.y && v.x <= lightPos.x) //Top right quad
			{
				if (NormSquared(botIntersect) < NormSquared(leftIntersect))
				{
					minimalIntersection = botIntersect;
				} else
				{
					minimalIntersection = leftIntersect;
				}
			}
			
			///< Avoid creating shadows that don't actually exist when objects are big (ie edges of screen when vertex is not on the screen
			if (NormSquared(lightPos - minimalIntersection) > NormSquared(lightPos - v))
			{
				shadowPoints.push_back(minimalIntersection);
			}
			
			std::vector<sf::Vector2f>	possiblePoints;
			
			///< Find the points that make up the shadow
			
			///< Get all intersections the ray makes with the edges of the shape
			for (int j = 0; j < numVerticies-1; j++)
			{
				sf::Vector2f edgeIntersection = GetIntersectionPoint(lightPos, minimalIntersection, shape->getPoint(j), shape->getPoint(j+1));
				if (isPointOnLineSegment(edgeIntersection, shape->getPoint(j), shape->getPoint(j+1)))
				{
					sf::Vector2f offset = Normalize(v-lightPos) * 20.f;
					possiblePoints.push_back(edgeIntersection + offset);
				}
				
			}
			sf::Vector2f edgeIntersection = GetIntersectionPoint(lightPos, minimalIntersection, shape->getPoint(numVerticies-1), shape->getPoint(0));
			if (isPointOnLineSegment(edgeIntersection, shape->getPoint(numVerticies-1), shape->getPoint(0)))
			{
				sf::Vector2f offset = Normalize(v-lightPos) * 20.f;
				possiblePoints.push_back(edgeIntersection + offset);
			}
			
			if (!possiblePoints.empty())
			{
				shadowPoints.push_back(getMinimumDistfromPoint(lightPos, possiblePoints));
			}
			
		}
		///< Shoot rays at the corners and add them if they cross one of the shape edges
		for (int i = 0; i < numVerticies-1; i++)
		{
			if (doIntersect(lightPos, mScreenCorners[0], shape->getPoint(i), shape->getPoint(i+1)))
			{
				shadowPoints.push_back(mScreenCorners[0]);
			}
			if (doIntersect(lightPos, mScreenCorners[1], shape->getPoint(i), shape->getPoint(i+1)))
			{
				shadowPoints.push_back(mScreenCorners[1]);
			}
			if (doIntersect(lightPos, mScreenCorners[2], shape->getPoint(i), shape->getPoint(i+1)))
			{
				shadowPoints.push_back(mScreenCorners[2]);
			}
			if (doIntersect(lightPos, mScreenCorners[3], shape->getPoint(i), shape->getPoint(i+1)))
			{
				shadowPoints.push_back(mScreenCorners[3]);
			}
		}
		
		///< Sort points in clockwise order (for creating a convex shape)
		shadowPoints = convex_hull(shadowPoints);
		
		(*shadow)->setPointCount(shadowPoints.size());
		
		
		
		for (int k = 0; k < shadowPoints.size(); k++)
		{
			(*shadow)->setPoint(k, shadowPoints.at(k));
		}
		
		
		if (shadow != mShadows.end()) {
			shadow++;
		}
	}
	
}

void ShadowManager::drawShadows(sf::RenderTarget& target)
{
	for (const sf::ConvexShape* shadow : mShadows)
	{
		target.draw(*shadow);
	}
	
	for (const sf::Vector2f& vec : mDebugLines)
	{
		DrawDebugLine(lightPos, vec, target);
	}
	mDebugLines.clear();
	
	for (const sf::Vector2f& vec : mDebugPoints)
	{
		DrawDebugCircle(vec, 5.f, target);
	}
	mDebugPoints.clear();
}