#ifndef MANAGER_ENTITY_H
#define MANAGER_ENTITY_H
#define MAXIMUM_COMPONENTS 32
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "Bitmask.h"
#include "Component_Base.h"
#include "Utility.h"


class Manager_Texture;
class Manager_System;
using EntityID = unsigned int;
using ComponentPtr = std::unique_ptr<Component_Base>;
using ComponentStorage = std::unordered_map<ComponentType, ComponentPtr>;


using EntityComponents = std::unordered_map<EntityID, std::pair<Bitmask,ComponentStorage>>;

using ComponentProducer = std::function <ComponentPtr(void)>;
using ComponentFactory = std::unordered_map<ComponentType, ComponentProducer>;; //produce a single component

using ComponentAttributes = std::vector <std::pair<ComponentType, std::string>>;//characteristics of each component from read file, stored in a string for each active component
using LoadedEntities = std::unordered_map<std::string, std::pair<Bitmask, ComponentAttributes>>;

class Manager_Entity {
protected:
	EntityID maxid;
	EntityComponents entitycomponents;
	ComponentFactory componentfactory;
	LoadedEntities loadedentities;

	Manager_Texture* texturemgr{ nullptr };
	Manager_System* systemmgr{ nullptr };

	template<typename T>
	void RegisterComponentProducer(const ComponentType& type) {
		componentfactory[type] = [this]() {return std::make_unique<T>(); };
	}
	auto FindEntity(const EntityID& entityid)->EntityComponents::iterator {
		auto foundentity = entitycomponents.find(entityid);
		if (foundentity == entitycomponents.end()) {
			LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Could not find entity of ID " + std::to_string(entityid));
		}
		return foundentity;
	}
	bool HasComponent(const EntityID& entityid, const ComponentType& component);
	void CreateComponent(const EntityID& entityid, const ComponentType& t, const std::string& componentattributes = {}); //creating a component for a new entity.
	
public:
	Manager_Entity(Manager_System* mgr, Manager_Texture* tmgr);
	void RegisterEntity(const std::string& path); //read entity file containing component(s) info
	EntityID CreateEntity(const Bitmask& b, const ComponentAttributes& attributes = ComponentAttributes{}); //add entity based on constructed bitmask.
	EntityID CreateEntity(const std::string& filename); //creates an entity based on an already registered entity file.
	
	void AddComponent(const EntityID& entityid, const ComponentType& t, const std::string& componentattributes = {}); //adds component to an already existing entity.
	void RemoveComponent(const EntityID& entityid, const ComponentType& t); 
	void RemoveEntity(const EntityID& entityid); //deletes all com

	template<typename T>
	T* GetComponent(const EntityID& entityid, const ComponentType& t)  {
		auto foundentity = FindEntity(entityid);
		if (foundentity == entitycomponents.end()) return nullptr;
		auto& bitmask = foundentity->second.first;
		if (!bitmask.GetBit(Utility::ConvertToUnderlyingType(t))) return nullptr;
		auto componentptr = foundentity->second.second.at(t).get();
		return dynamic_cast<T*>(componentptr);
	}
};


/*
	struct BaseStyle { //Exposed to the user as a const object.
		BaseStyle()
			:fill_color(Attribute_Type::Fill_Color, [](sf::Shape& shape, sf::Color& col) {shape.setFillColor(col); }, nullptr),
			outline_color(Attribute_Type::Outline_Color, [](sf::Shape& shape, sf::Color& col) {shape.setOutlineColor(col); }, nullptr),
			outline_thickness(Attribute_Type::Outline_Thickness, [](sf::Shape& shape, uint8_t& thickness) {shape.setOutlineThickness(thickness); }, nullptr),
			origin(Attribute_Type::Origin, [](sf::Transformable& transformable, sf::Vector2f& origin) {transformable.setOrigin(origin); }, nullptr),
			local_position(Attribute_Type::Local_Position, [](sf::Transformable& transformable, sf::Vector2f& pos) {transformable.setPosition(pos); }, nullptr),
			scale(Attribute_Type::Scale, [](sf::Transformable& transformable, sf::Vector2f& scale) {transformable.setScale(scale); }, nullptr),
			rotation(Attribute_Type::Rotation, [](sf::Transformable& transformable, float& rotation) {transformable.rotate(rotation); }, nullptr)
			//resource_name(Attribute_Type::Resource_Name, [](sf::Transformable, std::string& str){}, nullptr)
		{
		}
		StyleComponent<sf::Color, sf::Shape> fill_color;
		StyleComponent<sf::Color, sf::Shape> outline_color;
		StyleComponent<uint8_t, sf::Shape> outline_thickness;
		StyleComponent<sf::Vector2f, sf::Transformable> origin;
		StyleComponent<sf::Vector2f, sf::Transformable> local_position;
		StyleComponent<sf::Vector2f, sf::Transformable> scale;
		StyleComponent<float, sf::Transformable> rotation;
		virtual ~BaseStyle() = 0;
	private:

	};


	struct TextStyle : public BaseStyle //Exposed by const
	{
		TextStyle()
			:character_size(Attribute_Type::Character_Size, [](sf::Text& text, uint32_t& char_size) {text.setCharacterSize(char_size); }, nullptr),
			letter_spacing(Attribute_Type::Letter_Spacing, [](sf::Text& text, float& letter_spacing) {text.setLetterSpacing(letter_spacing); }, nullptr),
			line_spacing(Attribute_Type::Line_Spacing, [](sf::Text& text, float& line_spacing) {text.setLineSpacing(line_spacing); }, nullptr),
			style(Attribute_Type::Text_Style, [](sf::Text& text, uint32_t& style) {text.setStyle(style); }, nullptr),
			text_string(Attribute_Type::Text_String, [](sf::Text& text, std::string& str) {text.setString(str);},nullptr)
		{
		}
		StyleComponent<uint32_t, sf::Text> character_size;
		StyleComponent<float, sf::Text> letter_spacing;
		StyleComponent<float, sf::Text> line_spacing;
		StyleComponent<uint32_t, sf::Text> style;
		StyleComponent<std::string, sf::Text> text_string;
	};
	struct BackgroundStyle : public BaseStyle
	{
		StyleComponent<sf::Vector2f, sf::RectangleShape> size;
		StyleComponent<sf::IntRect, sf::RectangleShape> texture_rect;
		BackgroundStyle()
			:size(Attribute_Type::Element_Size, [](sf::RectangleShape& rect, sf::Vector2f& size) {rect.setSize(size); }, nullptr),
			texture_rect(Attribute_Type::Texture_Rect, [](sf::RectangleShape& rect, sf::IntRect& texture_rect) {rect.setTextureRect(texture_rect); }, nullptr)
		{
		}
	};


*/
#endif