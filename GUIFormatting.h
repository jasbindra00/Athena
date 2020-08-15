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
				if (attributetype == "COLOR_SOLID") {
					try {
						color.r = std::stoi(keys.find("R")->second);
						color.g = std::stoi(keys.find("G")->second);
						color.b = std::stoi(keys.find("B")->second);
						color.a = std::stoi(keys.find("A")->second);
					}
					catch (const std::exception& exception) { throw CustomException("Invalid R/G/B/A value for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to 0 "); }
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
				try {
					tbgtexturerect.left = std::stoi(keys.find("TOP_LEFT_X")->second);
					tbgtexturerect.top = std::stoi(keys.find("TOP_LEFT_Y")->second);
					tbgtexturerect.width = std::stoi(keys.find("SIZE_X")->second);
					tbgtexturerect.height = std::stoi(keys.find("SIZE_Y")->second);
				}
				catch (const std::exception& exception) {
				}
				if (!defaultedstr.empty()) throw CustomException("The following texture rect attributes have been defaulted : " + std::move(defaultedstr));
			}
			if (attributekey == keys.end()) throw CustomException("Unable to identify " + KeyProcessing::ConstructKeyStr(attributetype, "val - DEFAULTED. "));
			if (attributetype == "OUTLINE_THICKNESS") {
				try { outlinethickness = std::stoi(attributekey->second); }
				catch (const std::exception& exception) { throw CustomException("Invalid argument for {OUTLINE_THICKNESS,val} KEY - defaulted to 2 "); }
			}
			else if (attributetype == "TEXTURE_NAME") tbg_name = attributekey->second;
		}
			
	};
	struct Text {
		sf::Vector2f localpositionproportion;
		sf::Color textcolor{ sf::Color::Color(255,255,255,255 )};
		std::string fontname{ "arial.ttf" };
		unsigned int charactersize{ 30 };
		sf::Vector2f originproportion{ 0,0 };
			void ReadIn(KeyProcessing::Keys& keys, const std::string& attributetype) {
			using KeyProcessing::KeyPair;
			std::vector<KeyPair> missingkeys = KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"LOCAL_POSITION", "ERROR"}, { "ORIGINX%", "ERROR" }, { "ORIGINY%", "ERROR" }, { "FONT_NAME", "ERROR" }}, keys);
			std::string missingkeystr;
			for (auto& missingkey : missingkeys) {
				missingkeystr += KeyProcessing::ConstructKeyStr(missingkey.first, missingkey.second) + " ";
			}
			//REFACTOR THIS
			if (attributetype == "COLOR_SOLID") {
				KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"R", "ERROR"}, { "G","ERROR" }, { "B","ERROR" }, { "A","ERROR" }}, keys);
				try {

					textcolor.r = std::stoi(keys.find("R")->second);
					textcolor.g = std::stoi(keys.find("G")->second);
					textcolor.b = std::stoi(keys.find("B")->second);
					textcolor.a = std::stoi(keys.find("A")->second);
				}
				catch (const std::exception& exception) { throw CustomException("Invalid R/G/B/A value for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to 255 "); }
			}
			else if (attributetype == "CHARACTER_SIZE") {
				try { charactersize = std::stoi(keys.find("CHARACTER_SIZE")->second); }
				catch (const std::exception& exception) { throw CustomException("Character size attribute for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to 30 "); }
			}
			else if (attributetype == "ORIGIN") {
				KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"ORIGINX%", "ERROR"}, { "ORIGINY%", "ERROR" }}, keys);
				try {
					originproportion.x = std::stof(keys.find("ORIGINX%")->second) / 100;
					originproportion.y = std::stof(keys.find("ORIGINY%")->second) / 100;
				}
				catch (const std::exception& exception) { throw CustomException("Invalid ORIGIN% argument for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to 0%"); }
			}
			else if (attributetype == "FONT_NAME") {
				if (KeyProcessing::FillMissingKey(KeyPair{ "FONT_NAME", "ERROR" }, keys)) throw CustomException("Invalid font name argument for {PROPERTY_ATTRIBUTE,", fontname + "} has been defaulted to arial.ttf ");
				fontname = keys.find("FONT_NAME")->second;
			}
			else if (attributetype == "LOCAL_POSITION") {
				KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"POSITIONX%", "ERROR"}, { "POSITIONY%", "ERROR" }}, keys);
				try {
					localpositionproportion.x = std::stof(keys.find("POSITIONX%")->second) / 100;
					localpositionproportion.y = std::stof(keys.find("POSITIONY%")->second) / 100;
				}
				catch (const std::exception& exception) { throw CustomException("Invalid position% argument for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to 0%"); }
			}
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
			if (propertytype == "SBG" || propertytype == "TBG") {
				try { style.background.ReadIn(keys, attributetype); }
				catch (const CustomException& exception) {

				}
			}
			else if (propertytype == "TEXT") {
				try { style.text.ReadIn(keys, attributetype); }
				catch (const CustomException& exception) {

				}
			}
		}
	};
	struct GUIVisual {
		const static unsigned int maxcharactersize = 100;
		sf::RectangleShape sbg;
		sf::RectangleShape tbg;
		sf::Text text;
		std::shared_ptr<sf::Font> font{ nullptr };
		std::shared_ptr<sf::Texture> tbg_texture{ nullptr };
		GUIVisual() {
			tbg.setFillColor(sf::Color::Transparent);
			text.setFillColor(sf::Color::Transparent);
			text.setPosition(sf::Vector2f{ 0,0 });
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
		inline sf::FloatRect CalibrateText(const sf::FloatRect& eltlocalboundingbox, const sf::Vector2f& originproportion, const unsigned int& charactersize, const sf::Vector2f& positionproportion) {
			text.setCharacterSize(charactersize);
			//adjust the text position such that its specified position matches the input origin and position
			sf::FloatRect textbounds = text.getLocalBounds();
			const sf::Vector2f textsize{ textbounds.width, textbounds.height };
			const sf::Vector2f localorigin{ originproportion.x * textsize.x, originproportion.y * textsize.y };
			text.setOrigin(localorigin);
			sf::Vector2f textposition = sf::Vector2f{ eltlocalboundingbox.left, eltlocalboundingbox.top } + sf::Vector2f{ positionproportion.x * eltlocalboundingbox.width, positionproportion.y * eltlocalboundingbox.height };
			textposition -= sf::Vector2f{ text.getLocalBounds().left, text.getLocalBounds().top }; //sfml has a padding around the text which messes up the offset. padding encapsulated by local bound coords.
			text.setPosition(textposition);
			auto TextFits = [&textposition, &textsize, &eltlocalboundingbox]()->bool {
				return !(textposition.x < 0 || textposition.x + textsize.x > eltlocalboundingbox.width || textposition.y < 0 || textposition.y + textsize.y > eltlocalboundingbox.height);
			};
			return text.getLocalBounds();
// 			if (!TextFits()) {
// 				//must find the maximum charactersize, while maintaining this position, which allows us to fit in our element.
// 				unsigned int charactersize = 1;
// 				while (charactersize < GUIVisual::maxcharactersize) {
// 					text.setCharacterSize(charactersize);
// 					text.setPosition(textposition);
// 					if (TextFits()) break;
// 					++charactersize;
// 				}
// 			}
			
		}
		
	};
	
}


#endif