#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <memory>
#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Player.h"
#include "ShadowManager.h"

class Game : private sf::NonCopyable
{
public:
    
    static Game* Instance()
    {
        if (s_pInstance == nullptr)
        {
            s_pInstance = new Game();
        }
        return s_pInstance;
    }
    
    Game();
    void					run();
    
    sf::RenderWindow*		getWindow() { return &mWindow; }
    sf::View*				getWorldView() { return &mWorldView; }
    
    
    bool					getMouseButtonState(sf::Mouse::Button button) { return sf::Mouse::isButtonPressed(button); }
    sf::Vector2f			getMousePosition() { return mWindow.mapPixelToCoords((sf::Mouse::getPosition(mWindow))); }
    sf::Vector2f			getMousePositionRelativeToWorldView() { return mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow), mWorldView);}
    
private:
    void					processEvents();
    void					update(sf::Time dtTime);
    void					render();
    
    void					updateStatistics(sf::Time elapsedTime);

	sf::ConvexShape*		createCircle(int numPts, int radius, sf::Vector2f location);
    
private:
    static Game*			s_pInstance;
    
    static const sf::Time	TimePerFrame;
    
    sf::RenderWindow		mWindow;
    
    sf::Font				mFont;
    sf::Text				mStatisticsText;
    sf::Time				mStatisticsUpdateTime;
    std::size_t				mStatisticsNumFrames;
    sf::RectangleShape		mTextBackground;
    
    sf::View				mUIView;
    sf::View				mWorldView;
    
    sf::Vector2f			camPos;
	
	ShadowManager			mShadowManager;
    
    TextureHolder           GameTextures;
    FontHolder              GameFonts;
    
    Player                  mPlayer;
    
    sf::RectangleShape      mBackdrop;
    
    std::vector<sf::ConvexShape*> shapes;
    
};

typedef Game TheGame;
