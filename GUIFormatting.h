#ifndef GUIFORMATTING_H
#define GUIFORMATTING_H
#include <SFML\Graphics\Transformable.hpp>
#include <SFML\Graphics\Color.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Text.hpp>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include "GUIData.h"
#include "KeyProcessing.h"
#include "CustomException.h"


namespace GUIFormatting {
	struct Background {
		sf::Color sbg_color{ sf::Color::Color(255,255,255,255 )};
		std::string tbg_name{ "" };
		sf::Color outlinecolor{ sf::Color::Red };
		unsigned int outlinethickness{ 2 };
		sf::IntRect tbgtexturerect;
		void ReadIn(KeyProcessing::Keys& keys, const std::string& attributetype) {
			using KeyProcessing::KeyPair;
			auto attributekey = keys.find(attributetype);
			if (attributetype == "COLOR_SOLID" || attributetype == "COLOR_OUTLINE") {
				KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"R", "255"}, { "G","255" }, { "B","255" }, { "A","255" }}, keys);
				sf::Color& color = (attributetype == "COLOR_SOLID") ? sbg_color : outlinecolor;
				if (attributetype == "COLOR_SOLID")
					try {
					color.r = std::stoi(keys.find("R")->second);
					color.g = std::stoi(keys.find("G")->second);
					color.b = std::stoi(keys.find("B")->second);
					color.a = std::stoi(keys.find("A")->second);
				}
				catch (const std::exception& exception) { //throw CustomException("Invalid R/G/B/A value for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to 0 ");}
				}
			}
			else if (attributetype == "TEXTURE_RECT") {
				std::string defaultedstr;
				std::vector<KeyPair> missingkeys = KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"TOP_LEFT_X", "50"}, { "TOP_LEFT_Y", "50" }, { "SIZE_X", "50" }, { "SIZE_Y", "50" }}, keys);
				if (!missingkeys.empty()) {
					for (auto& missingkey : missingkeys) {
						defaultedstr += KeyProcessing::ConstructKeyStr(missingkey.first, missingkey.second) + " ";
					}
				}
				try{
					tbgtexturerect.left = std::stoi(keys.find("TOP_LEFT_X")->second);
					tbgtexturerect.top = std::stoi(keys.find("TOP_LEFT_Y")->second);
					tbgtexturerect.width = std::stoi(keys.find("SIZE_X")->second);
					tbgtexturerect.height = std::stoi(keys.find("SIZE_Y")->second);
					}
				catch (const std::exception& exception) {
				}
				if (!defaultedstr.empty()) throw CustomException("The following texture rect attributes have been defaulted : " + std::move(defaultedstr));
				
			}
			if (attributekey == keys.end()) throw CustomException("Unable to identify "+KeyProcessing::ConstructKeyStr(attributetype, "val - DEFAULTED. "));
			if (attributetype == "OUTLINE_THICKNESS") {
				try { outlinethickness = std::stoi(attributekey->second); }
				catch (const std::exception& exception) { //throw CustomException("Invalid argument for {OUTLINE_THICKNESS,val} KEY - defaulted to 2 "); }
				}
			}
			else if (attributetype == "TEXTURE_NAME") tbg_name = attributekey->second;

			}
		
		
	};
	struct Text {

		sf::Vector2f textlocalpos;
		sf::Color textcolor{ sf::Color::Red };
		std::string textstr{ "" };
		std::string fontname{ "" };
		unsigned int charactersize{ 0 };
		void ReadIn(KeyProcessing::Keys& keys, const std::string& attributetype) {

		}

	};
	struct GUIStyle {
	
		Text text;
		Background background;
		friend void operator>>(KeyProcessing::Keys& keys, GUIStyle& style) {
			using KeyProcessing::Keys;
			using KeyProcessing::KeyPair;
			if (KeyProcessing::FillMissingKey(KeyPair{ "STYLE_PROPERTY", "ERROR" }, keys)) throw CustomException("Unable to identify the {STYLE_PROPERTY,PROPERTY} key ");
			if (KeyProcessing::FillMissingKey(KeyPair{ "PROPERTY_ATTRIBUTE","ERROR" }, keys)) throw CustomException("Unable to identify the {PROPERTY_ATTRIBUTE,ATTRIBUTE} key");
			auto styleproperty = keys.find("STYLE_PROPERTY");
			std::string propertytype = styleproperty->second;
			keys.erase(styleproperty);
			auto propertyattribute = keys.find("PROPERTY_ATTRIBUTE");
			std::string attributetype = propertyattribute->second;
			keys.erase(propertyattribute);
			//create enum for each attribute
			//or let it deduce the attribute itself
			if (propertytype == "SBG" || propertytype == "TBG") {
				style.background.ReadIn(keys, attributetype);
			}
			else if (propertytype == "TEXT") {
				style.text.ReadIn(keys, attributetype);
			}
		}
	};
	struct GUIVisual {
		sf::RectangleShape sbg;
		sf::RectangleShape tbg;
		sf::Text text;
		std::shared_ptr<sf::Font> font{ nullptr };
		std::shared_ptr<sf::Texture> tbg_texture{ nullptr };
		GUIVisual() {
			tbg.setFillColor(sf::Color::Transparent);
			text.setFillColor(sf::Color::Transparent);
		}
		inline void SetPosition(const sf::Vector2f& pos) { //move entire elt visual.
			sbg.setPosition(pos);
			tbg.setPosition(pos);
			text.setPosition(pos); //origin maintained.
		}
		inline void SetSize(const sf::Vector2f& size) {
			sbg.setSize(size);
			tbg.setSize(size);
		}
	};

}

#endif