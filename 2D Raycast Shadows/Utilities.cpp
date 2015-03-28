#include "Utilities.h"
#include <cmath>

void DrawDebugLine(const sf::Vector2f& A, const sf::Vector2f& B, sf::RenderTarget& target, const sf::Color& colour)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = A;
    line[1].position = B;
    line[0].color = colour;
    line[1].color = colour;
    target.draw(line);
}

void DrawDebugLine(const float& ax, const float& ay, const float& bx, const float& by, sf::RenderTarget& target, const sf::Color& colour)
{
	sf::VertexArray line(sf::Lines, 2);
	line[0].position = sf::Vector2f(ax, ay);
	line[1].position = sf::Vector2f(bx, by);
	line[0].color = colour;
	line[1].color = colour;
	target.draw(line);
}

void DrawDebugLine(sf::Vector2f pos, sf::Vector2f& dir, float& norm, sf::RenderTarget& target, const sf::Color& colour)
{
	dir /= Norm(dir);
	sf::VertexArray line(sf::Lines, 2);
	line[0].position = pos;
	line[0].color = colour;
	line[1].position = pos + dir*norm;
	line[1].color = colour;
	target.draw(line);
}

void DrawDebugCircle(const sf::Vector2f& pos, const float& radius, sf::RenderTarget& target, const sf::Color& colour)
{
    sf::CircleShape circle;
    circle.setRadius(radius);
    circle.setOutlineColor(colour);
    circle.setOutlineThickness(1);
    circle.setFillColor(sf::Color::Transparent);
    circle.setPosition(pos);
    CenterOrgin(circle);
    target.draw(circle);
}

const float Norm(const sf::Vector2f & v)
{
	return std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
}

const float NormSquared(const sf::Vector2f & v)
{
    return std::pow(v.x, 2) + std::pow(v.y, 2);
}

const sf::Vector2f Normalize(const sf::Vector2f& vec)
{
    return vec / Norm(vec);
}

const float Distance(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
	return std::sqrt(std::pow((v2.x - v1.x), 2) + std::pow((v2.y - v1.y), 2));
}

const float DotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

const float DotProduct(const sf::Vector2f& a, const float& s)
{
    return a.x * s + a.y * s;
}

const float CrossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.y - a.y * b.x;
}
const sf::Vector2f CrossProduct( const sf::Vector2f& a, const float s )
{
    return sf::Vector2f(s * a.y, -s * a.x);
}
const sf::Vector2f CrossProduct( const float s, const sf::Vector2f& a)
{
    return sf::Vector2f(-s * a.y, s * a.x);
}

float Lerp(const float & a, const float & b, const float & t)
{
	return (1 - t)*a + t*b;
}

float LerpFast(const float & a, const float & b, const float & t) {
	return a + t*(b - a);
}

float Clamp(const float& value, const float& min, const float& max)
{
	float result;
	if (value > max)
		result = max;
	else if (value < min)
		result = min;
	else
		result = value;
	return result;
}

std::string VectorToStr(const sf::Vector2f& vec)
{
	return std::string(" [ " + std::to_string(vec.x) + " , " + std::to_string(vec.y) + " ] ");
}
std::string VectorToStr(const sf::Vector2i& vec)
{
	return std::string(" [ " + std::to_string(vec.x) + " , " + std::to_string(vec.y) + " ] ");
}
std::string VectorToStr(const sf::Vector2u& vec)
{
	return std::string(" [ " + std::to_string(vec.x) + " , " + std::to_string(vec.y) + " ] ");
}

void CenterOrgin(sf::Shape& shape)
{
    shape.setOrigin(sf::Vector2f(shape.getLocalBounds().height, shape.getLocalBounds().width) / 2.f );
}