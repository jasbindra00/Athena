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
#include "ManagedResources.h"
class GUIElement;
class GUIInterface;
class GUILabel;
class GUITextfield;
class GUICheckbox;
namespace GUIFormatting {
	static const unsigned int maxcharactersize = 30;
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
		sf::Vector2f originproportion{ 0,0 };
		sf::Color textcolor{ sf::Color::Color(255,255,255,255 )};
		std::string fontname{ "arial.ttf" };
		bool texthidden = true;
		unsigned int charactersize{ maxcharactersize };
			void ReadIn(KeyProcessing::Keys& keys, const std::string& attributetype) {
			using KeyProcessing::KeyPair;
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
			else if (attributetype == "ORIGIN%") {
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
			else if (attributetype == "LOCAL_POSITION%") {
				KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"POSITIONX%", "ERROR"}, { "POSITIONY%", "ERROR" }}, keys);
				try {
					localpositionproportion.x = std::stof(keys.find("POSITIONX%")->second) / 100;
					localpositionproportion.y = std::stof(keys.find("POSITIONY%")->second) / 100;
				}
				catch (const std::exception& exception) { throw CustomException("Invalid position% argument for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to 0%"); }
			}
			//init single key attributes here.
			else {
				KeyProcessing::FillMissingKey(KeyProcessing::KeyPair{ attributetype, "ERROR" }, keys);
				auto key = keys.find(attributetype);
				if (attributetype == "TEXT_HIDDEN") {
					texthidden = (key->second == "TRUE");
				}
				else if (attributetype == "CHARACTER_SIZE") {
					auto charkey = keys.find("CHARACTER_SIZE");
					if (charkey->second == "MAX") charactersize = maxcharactersize;
					else {
						try { charactersize = std::stoi(charkey->second); }
						catch (const std::exception& exception) { throw CustomException("Character size attribute for {PROPERTY_ATTRIBUTE," + attributetype + "} has been defaulted to MAX "); }
					}
				}
			}
			
		}
	};
	struct GUIStyle {
		Text text;
		Background background;
		//reading one line at a time. a single line has multiple keys.
		friend void operator>>(KeyProcessing::Keys& keys, GUIStyle& style) {
			using KeyProcessing::Keys;
			using KeyProcessing::KeyPair;
			if (KeyProcessing::FillMissingKey(KeyPair{ "STYLE_PROPERTY", "ERROR" }, keys)) throw CustomException("Unable to identify the {STYLE_PROPERTY,PROPERTY} key ");
			if (KeyProcessing::FillMissingKey(KeyPair{ "PROPERTY_ATTRIBUTE","ERROR" }, keys)) throw CustomException("Unable to identify the {PROPERTY_ATTRIBUTE,ATTRIBUTE} key");
			auto styleproperty = keys.find("STYLE_PROPERTY");
			std::string propertytype = styleproperty->second;
			auto propertyattribute = keys.find("PROPERTY_ATTRIBUTE");
			std::string attributetype = propertyattribute->second;
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
		void AdjustForSystemTexture() {
			background.outlinecolor = sf::Color::Transparent;
			background.outlinethickness = 0;
			background.sbg_color = sf::Color::Color(255, 255, 255, 255);
		}
	};
	class GUIVisual {
		friend class GUIElement;
		friend class GUIInterface;
		friend class GUILabel;
		friend class GUITextfield;
		friend class GUICheckbox;
	protected:
		std::vector<sf::Text> customtext;
		sf::RectangleShape sbg;
		sf::RectangleShape tbg;
		sf::Text text;
		std::shared_ptr<sf::Font> font{ nullptr };
		std::shared_ptr<sf::Texture> tbg_texture{ nullptr };
		template<typename T, typename  = typename ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<T>>
		std::shared_ptr<T>& GetResource() {
			if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Font>) return font;
			else if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Texture>) return tbg_texture;
		}
		inline void CalibrateText(const sf::FloatRect& eltlocalboundingbox, const sf::Vector2f& positionproportion, const sf::Vector2f& originproportion, const unsigned int& charactersize) {
			sf::Vector2f textsize;
			sf::Vector2f localorigin;
			sf::Vector2f textposition;
			auto CalculateAndApply = [&textsize, &localorigin, &textposition, &originproportion, &positionproportion, &eltlocalboundingbox](sf::Text& text, const unsigned int& charactersize) {
				text.setCharacterSize(charactersize);
				textsize = sf::Vector2f{ text.getLocalBounds().width, text.getLocalBounds().height }; //size of the bounding box.
				localorigin = { originproportion.x * textsize.x, originproportion.y * textsize.y }; //local origin as a proportion of the size.
				textposition = sf::Vector2f{ eltlocalboundingbox.left, eltlocalboundingbox.top };//plus the top left position of the element its being drawn relative to
				textposition += sf::Vector2f{ positionproportion.x * eltlocalboundingbox.width, positionproportion.y * eltlocalboundingbox.height };//plus the position of the actual text itself as a proportion of the parent element size
				textposition -= sf::Vector2f{ text.getLocalBounds().left, text.getLocalBounds().top };//minus the default padding that sfml inserts with sf::Text::getLocalBounds()
				text.setOrigin(localorigin);
				text.setPosition(textposition);
			};
			auto TextFits = [&eltlocalboundingbox, &textposition, &localorigin, &textsize, &originproportion, &positionproportion]()->bool {
				sf::Vector2f texttopleft{ textposition - localorigin };
				if (texttopleft.x < eltlocalboundingbox.left) return false;
				if (texttopleft.x + textsize.x > eltlocalboundingbox.left + eltlocalboundingbox.width) return false;
				if (texttopleft.y < eltlocalboundingbox.top) return false;
				if (texttopleft.y + textsize.y > eltlocalboundingbox.top + eltlocalboundingbox.height) return false;
				return true;
			};
			CalculateAndApply(text, charactersize);
			if (!TextFits()) {
				//must find the maximum charactersize, while maintaining this position, which allows us to fit in our element.
				unsigned int newcharsize = maxcharactersize;
				while (newcharsize > 1) {
					CalculateAndApply(text, newcharsize);
					if (TextFits()) break; //text fits snugly in our element
					--newcharsize;
				}
			}
		}
	public:
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
		inline void ApplyStyle(const GUIStyle& style, sf::FloatRect& eltrect, const sf::Vector2f& textpositionproportion, const sf::Vector2f& textoriginproportion, const unsigned int& charactersize) {
			sbg.setFillColor(style.background.sbg_color);
			sbg.setOutlineColor(style.background.outlinecolor);
			sbg.setOutlineThickness(style.background.outlinethickness);
			text.setFillColor(style.text.textcolor);
			
			CalibrateText(eltrect, textpositionproportion, textoriginproportion, charactersize);
		}
		
	};
	
}


#endif