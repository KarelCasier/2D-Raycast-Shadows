#include "Game.h"
#include <algorithm>
#include <functional>
#include "Utilities.h"
#include "GlobalConstants.hpp"



const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f); // = 0.6 seconds per frame, therefore 60fps

Game* Game::s_pInstance = nullptr;                                              ///< set game instance pointer to nullptr

Game::Game()
: mWindow(sf::VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Untitled by Karel Casier")		///< Initialize window
, mFont()
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
, mWorldView(mWindow.getDefaultView())                                                            ///< View used to render world
, mUIView(sf::Vector2f(WINDOWWIDTH, WINDOWHEIGHT) / 2.f, sf::Vector2f(WINDOWWIDTH, WINDOWHEIGHT)) ///< View used to render UI
, camPos(0, 0)
, mShadowManager(&mWorldView)
{
    mWindow.setKeyRepeatEnabled(true);
    mWindow.setVerticalSyncEnabled(true);
    
    ///<------------------------------------------------------->                ///< Statistics Initializing
    
    GameFonts.load(FontID::Default, "MYRIADPRO-BOLD.OTF");
    
    mFont = GameFonts.get(FontID::Default);
    
    mStatisticsText.setFont(mFont);
    mStatisticsText.setPosition(5.f, 5.f);
    mStatisticsText.setCharacterSize(15);
    
    mTextBackground.setSize(sf::Vector2f(160.f, 30.f));
    mTextBackground.setFillColor(sf::Color(110, 110, 110, 80));
    ///<------------------------------------------------------->

    
    mBackdrop.setSize(sf::Vector2f(2* WINDOWWIDTH, 2* WINDOWHEIGHT));
    mBackdrop.setFillColor(sf::Color(100,100,100));
    
	std::cout << "Set-up complete" << std::endl;
	
	mPlayer.setPosition(20, 20);
    
	///< Spawn some items
    sf::ConvexShape* s1 = new sf::ConvexShape();
    s1->setPointCount(4);
    s1->setPoint(0, sf::Vector2f(200,200));
    s1->setPoint(1, sf::Vector2f(250,200));
    s1->setPoint(2, sf::Vector2f(250,250));
    s1->setPoint(3, sf::Vector2f(200,250));
	shapes.push_back(s1);
	mShadowManager.addShape(s1);
	
	sf::ConvexShape* s2 = new sf::ConvexShape();
	s2->setPointCount(4);
	s2->setPoint(0, sf::Vector2f(500,500));
	s2->setPoint(1, sf::Vector2f(510,610));
	s2->setPoint(2, sf::Vector2f(430,550));
	s2->setPoint(3, sf::Vector2f(400,520));
	shapes.push_back(s2);
	mShadowManager.addShape(s2);
	
	sf::ConvexShape* topWall = new sf::ConvexShape();
	topWall->setPointCount(4);
	topWall->setPoint(0, sf::Vector2f(-10, -10));
	topWall->setPoint(1, sf::Vector2f(2 * WINDOWWIDTH + 10, -10));
	topWall->setPoint(2, sf::Vector2f(2 * WINDOWWIDTH + 10, 0));
	topWall->setPoint(3, sf::Vector2f(-10,0));
	shapes.push_back(topWall);
	mShadowManager.addShape(topWall);
	
	sf::Vector2f pos(800,200);
	sf::Vector2f offset(0,25);
	
	for (int i = 0; i < 50; ++i)
	{
		sf::ConvexShape* b4 = new sf::ConvexShape();
		b4->setPointCount(4);
		b4->setPoint(0, pos + offset * (float)i);
		b4->setPoint(1, pos + sf::Vector2f(20,0) + offset * (float)i);
		b4->setPoint(2, pos + sf::Vector2f(20,20) + offset * (float)i);
		b4->setPoint(3, pos + sf::Vector2f(0,20) + offset * (float)i);
		shapes.push_back(b4);
		mShadowManager.addShape(b4);
	}

	sf::ConvexShape* circle = createCircle(6, 25, sf::Vector2f(0, 500));
	shapes.push_back(circle);
	mShadowManager.addShape(circle);
}

sf::ConvexShape* Game::createCircle(int numPts, int r, sf::Vector2f location)
{
	sf::ConvexShape* circle = new sf::ConvexShape;
	circle->setPointCount(numPts);
	float angleIncrement = 2 * 3.14159f / numPts;
	float currentAngle = 0;
	for (int i = 0; i < numPts; ++i)
	{
		circle->setPoint(i, sf::Vector2f(location.x + r * std::cos(currentAngle), location.y + r * std::sin(currentAngle)));
		currentAngle += angleIncrement;
	}

	return circle;
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen())
    {
        sf::Time elapsedTime = clock.restart();                                 ///< Note restart returns the time on the clock then restarts the clock
        timeSinceLastUpdate += elapsedTime;
        while (timeSinceLastUpdate > TimePerFrame)                              ///< Ensures that the logic is caught up before rendering again
        {
            timeSinceLastUpdate -= TimePerFrame;
            
            processEvents();
            update(TimePerFrame);
        }
        
        updateStatistics(elapsedTime);
        render();
    }
    mWindow.close();
}

void Game::processEvents()
{
    
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Key::Escape)
            mWindow.close();
    
        mPlayer.handleInput(event);
    }
    
    mPlayer.handleRealtimeInput();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        mWorldView.zoom(1.01);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        mWorldView.zoom(0.99);
    }
    
    
    
    //Center camera around the point btween camera and player
    
    sf::Vector2f playerToCameraDifference =getMousePositionRelativeToWorldView() - mPlayer.getPosition();
    sf::Vector2f dir = Normalize(playerToCameraDifference);
    float lengthSquared = Clamp(Norm(playerToCameraDifference), 0, 400);
    dir *= lengthSquared;
    
    //sf::Vector2f clampedPlayerToCameraDifference(clamp(playerToCameraDifference.x, 0, 750), clamp(playerToCameraDifference.y, 0, 750));
    
    mWorldView.setCenter(mPlayer.getPosition() + 0.5f * (dir));
    
}

void Game::update(sf::Time dTime)
{
    mPlayer.update(dTime);
	mShadowManager.updateShadows(mPlayer.getPosition());
}

void Game::updateStatistics(sf::Time elapsedTime)
{
    std::stringstream strStream;
    
    mStatisticsUpdateTime += elapsedTime;
    mStatisticsNumFrames += 1;
    
    if (mStatisticsUpdateTime >= sf::seconds(1.0f))
    {
        strStream << "Frames / Second = " << mStatisticsNumFrames << "\n";// << "Time / Update = " << mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames << "us" << std::endl;
        
        mStatisticsText.setString(strStream.str());
        
        mStatisticsUpdateTime -= sf::seconds(1.0f);
        mStatisticsNumFrames = 0;
    }
}

void Game::render()
{
    mWindow.clear(sf::Color(50, 50, 50,1.0));
    
    mWindow.setView(mWorldView);
    mWindow.draw(mBackdrop);

	for (const sf::ConvexShape* shape : shapes)
	{
		mWindow.draw(*shape);
	}
	mShadowManager.drawShadows(mWindow);
    mWindow.draw(mPlayer);
    mWindow.setView(mUIView);
    mWindow.draw(mTextBackground);
    mWindow.draw(mStatisticsText);
    mWindow.setView(mWorldView);
    mWindow.display();
}