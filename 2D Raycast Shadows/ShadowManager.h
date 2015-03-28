#pragma once

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>

class ShadowManager
{
public:
	ShadowManager(sf::View* view);
	void			addShape(sf::ConvexShape* newShape);
	void			updateShadows(const sf::Vector2f& lightPos);
	void			drawShadows(sf::RenderTarget& target);
	
	sf::Vector2f	GetIntersectionPoint(const sf::Vector2f& x1, const sf::Vector2f& x2, const sf::Vector2f& y1, const sf::Vector2f& y2);
	
private:
	std::vector<sf::ConvexShape*>	mShadowCasters;
	std::vector<sf::ConvexShape*>	mShadows;
	
	std::array<sf::Vector2f, 4>		mScreenCorners;
	
	std::vector<sf::Vector2f>		mDebugLines;
	std::vector<sf::Vector2f>		mDebugPoints;
	
	sf::View*						pView;
	
	sf::Vector2f					lightPos;

	static const float				shadowOffset; ///< offset the shadow so the object can be partly seen

};