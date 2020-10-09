#ifndef GUISTYLE_H
#define GUISTYLE_H
#include <functional>
#include <SFML/Graphics.hpp>
/*
-Whenever the user sets a given GUIStyle attribute, the change should be logged automatically
so as to promote the queue processing of a given Attribute.
-Bool flag.
-Processing of a queue to achieve this.
-How are we able to process a given queue?
*/
/*
-We should be able to have a base style component type for a given GUIStyle
-
*/
template<typename Attribute_Type, typename Property>
class GUIAttribute
{
	using AttributeApplier = std::function<void(Property&)>;
private:
	bool pending_change;
	Attribute_Type attribute;
	AttributeApplier applier;
public:
	GUIAttribute(Attribute_Type& arg, AttributeApplier i_applier)
		:attribute(std::forward<decltype(arg)>),
		applier(i_applier)
	{
	}
	void SetAttribute(Attribute_Type&& arg)
	{
		attribute = std::forward<Attribute_Type>(arg);
		pending_change = true;
	}
	void ApplyAttribute(Property& m_property)
	{
		applier(m_property);
		pending_change = false;
	}
};
struct CustomTransformable : public sf::Transformable
{
	CustomTransformable()
	{
	}
};
struct MyCustomGUIShape : public sf::Shape
{

};
template<typename Property>
struct BaseStyle //Abstract base class for a given GUIStyle component
{
public:
	CustomTransformable s;
	GUIAttribute<sf::Color, sf::Shape> fill_color;
	GUIAttribute<sf::Color, sf::Shape> outline_color;
	GUIAttribute<unsigned int, sf::Shape> outline_thickness;

	GUIAttribute<sf::Vector2f, sf::Transformable> position;
	GUIAttribute<sf::Vector2f, sf::Transformable> size;
	GUIAttribute<float, sf::Transformable> rotation;
	GUIAttribute<sf::Vector2f, sf::Transformable> origin;

	BaseStyle()
	{
	}
};
struct TextStyle
{
	
	TextStyle()
	{
	}
};

struct BackgroundStyle
{
	GUIAttribute<sf::IntRect, sf::RectangleShape> texture_rect;
};
class GUIStyle
{
private:
public:
};
#endif