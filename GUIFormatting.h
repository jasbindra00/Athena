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


namespace GUIFormatting {
	struct Background {
		sf::Color sbg_color{ sf::Color::Red };
		std::string tbg_name{ "" };
		sf::Color outlinecolor{ sf::Color::Red };
		unsigned int outlinethickness{ 2 };
		sf::IntRect tbgtexturerect;
		friend std::stringstream& operator>>(std::stringstream& stream, Background& background) {
			std::string word;
			stream >> word;
			if (word == "color_solid" || word == "color_outline") {
				unsigned int r = 0, g = 0, b = 0, a = 0;
				stream >> r >> g >> b >> a;
				(word == "color_solid") ? background.sbg_color = sf::Color::Color(r, g, b, a) : background.outlinecolor = sf::Color::Color(r, g, b, a);
			}
			else if (word == "outline_thickness") stream >> background.outlinethickness;
			else if (word == "texture_name")stream >> background.tbg_name;
			else if (word == "texture_rect") {
				stream >> background.tbgtexturerect.top >> background.tbgtexturerect.left >> background.tbgtexturerect.width >> background.tbgtexturerect.height;
			}
			return stream;
		}
	};
	struct Text {
		sf::Vector2f textlocalpos;
		sf::Color textcolor{ sf::Color::Red };
		std::string textstr{ "" };
		std::string fontname{ "" };
		unsigned int charactersize{ 0 };
		friend std::stringstream& operator>>(std::stringstream& stream, Text& text) {
			std::string word;
			stream >> word;
			if (word == "color") {
				unsigned int r, g, b, a = 0;
				stream >> r >> g >> b >> a;
				text.textcolor = sf::Color::Color(std::move(r), std::move(g), std::move(b), std::move(a));
			}
			else if (word == "string") {
				stream >> text.textstr;
			}
			else if (word == "font_name") stream >> text.fontname;
			else if (word == "character_size") stream >> text.charactersize;
			else if (word == "local_position") stream >> text.textlocalpos.x >> text.textlocalpos.y;
			return stream;
		}

	};
	struct GUIStyle {
		Text text;
		Background background;
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