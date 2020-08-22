#ifndef GUIFORMATTING_H
#define GUIFORMATTING_H
#include <string>
#include <array>
#include "EnumConverter.h"
#include "GUIData.h"
#include <SFML/Graphics.hpp>
#include "Manager_Font.h"
#include "Manager_Texture.h"
#include "ManagedResources.h"
#include "KeyProcessing.h"
#include "CustomException.h"
namespace GUIFormattingData {
	using KeyProcessing::Keys;
	using KeyProcessing::FoundKeys;
	static auto ProcessColor(const Keys& keys, sf::Color res) {
		FoundKeys foundkeys = KeyProcessing::GetKeys({ "R", "G", "B", "A" }, keys);
		bool successful = false;
		std::array<unsigned int, 4> rgba{ res.r, res.g,res.b,res.a };
		for (int i = 0; i < 4; ++i) {
			auto& foundkey = foundkeys.at(i);
			if (foundkey.first) {
				try { rgba.at(i) = std::stoi(foundkey.second->second); successful = true; }
				catch(...){ }
			}
		}
		res.r = std::move(rgba.at(0));
		res.g = std::move(rgba.at(1));
		res.b = std::move(rgba.at(2));
		res.a = std::move(rgba.at(3));
		return std::make_pair( std::move(successful), std::move(res) );
	}
	namespace BackgroundData {
		static enum class BGAttribute {
			FILL_COLOR, OUTLINE_COLOR, OUTLINE_THICKNESS, TEXTURE_NAME, TEXTURE_RECT, NULLTYPE = -1
		};
		static EnumConverter<BGAttribute> BGAttributeConv([](const std::string& str)->BGAttribute {
			if (str == "FILL_COLOR") return BGAttribute::FILL_COLOR;
			else if (str == "OUTLINE_COLOR") return BGAttribute::OUTLINE_COLOR;
			else if (str == "OUTLINE_THICKENSS") return BGAttribute::OUTLINE_THICKNESS;
			else if (str == "TEXTURE_NAME") return BGAttribute::TEXTURE_NAME;
			else if (str == "TEXTURE_RECT") return BGAttribute::TEXTURE_RECT;
			return BGAttribute::NULLTYPE;
			});
		struct BG {
			std::string texture_name;
			sf::IntRect texture_rect;
			sf::Color fill_color;
			sf::Color outline_color;
			unsigned int outline_thickness;
			BGAttribute ReadIn(const std::string& attributetype, const Keys& keys) {
				BGAttribute attribute = BGAttributeConv(attributetype);
				if (attribute == BGAttribute::NULLTYPE) return attribute;
				auto attributekey = KeyProcessing::GetKey(attributetype,keys);
				switch (attribute) {
				case BGAttribute::FILL_COLOR: {
					std::pair<bool, sf::Color> res = ProcessColor(keys, fill_color);
					if (!res.first) return BGAttribute::NULLTYPE;
					fill_color = std::move(res.second);
					break;
				}
				case BGAttribute::OUTLINE_COLOR: {
					std::pair<bool, sf::Color> res = ProcessColor(keys, outline_color);
					if (!res.first) return BGAttribute::NULLTYPE;
					outline_color = std::move(res.second);
					break;
				}
				case BGAttribute::OUTLINE_THICKNESS: {
					if (!attributekey.first) return BGAttribute::NULLTYPE;
					try { outline_thickness = std::stoi(attributekey.second->second); }
					catch (...) { return BGAttribute::NULLTYPE;}
					break;
				}
				case BGAttribute::TEXTURE_NAME: {
					if (!attributekey.first) return BGAttribute::NULLTYPE;
					texture_name = attributekey.second->second;
					break;
				}
				case BGAttribute::TEXTURE_RECT: {
					bool successful = false;
					std::array<unsigned int, 4> dimensionvals;
					FoundKeys dimensions = KeyProcessing::GetKeys({ "TOP_LEFT_X", "TOP_LEFT_Y", "SIZE_X", "SIZE_Y" }, keys);
					for (int i = 0; i < 4; ++i) {
						auto& dimension = dimensions.at(i);
						if (dimension.first) {
							try { dimensionvals.at(i) = std::stoi(dimension.second->second); }
							catch (const std::exception& exception) {}
							successful = true;
						}
					}
					if (!successful) return BGAttribute::NULLTYPE;
					texture_rect.left = dimensionvals.at(0);
					texture_rect.top = dimensionvals.at(1);
					texture_rect.width = dimensionvals.at(2);
					texture_rect.height = dimensionvals.at(3);
					if (!successful) return BGAttribute::NULLTYPE;
					break;
				}
				}
				return attribute;
			}
		};
		template<typename T>
		using IS_BG = std::is_same<typename std::decay_t<T>, BG>;
	}
	namespace TextData {
		static enum class TEXTAttribute {
			CHARACTER_SIZE, STRING, POSITION_PERCENTAGE, ORIGIN_PERCENTAGE, FONT_NAME, HIDDEN, NULLTYPE = -1
		};
		static EnumConverter<TEXTAttribute> TEXTAttributeConv([](const std::string& str)->TEXTAttribute {
			if (str == "CHARACTER_SIZE") return TEXTAttribute::CHARACTER_SIZE;
			else if (str == "STRING") return TEXTAttribute::STRING;
			else if (str == "POSITION_PERCENTAGE") return TEXTAttribute::POSITION_PERCENTAGE;
			else if (str == "ORIGIN_PERCENTAGE") return TEXTAttribute::ORIGIN_PERCENTAGE;
			else if (str == "HIDDEN") return TEXTAttribute::HIDDEN;
			else if (str == "FONT_NAME") return TEXTAttribute::FONT_NAME;
			return TEXTAttribute::NULLTYPE;
			});
		struct Text { //ENTIRE NOT ACCESSIBLE BY USER.
			sf::Vector2f position_proportion{ 0.5,0.5 };
			sf::Vector2f origin_proportion{ 0.5,0.5 };
			mutable std::string text_string{ "DEFAULT_TEXT" };
			std::string font_name{ "arial.ttf" };
			sf::Color fill_color;
			unsigned int character_size{ 20 };
			bool text_hidden{ false };
			TEXTAttribute ReadIn(const std::string& attributetypestr, const KeyProcessing::Keys& keys) {
				TEXTAttribute attribute = TEXTAttributeConv(attributetypestr);
				if (attribute == TEXTAttribute::NULLTYPE) return attribute;
				auto attributekey = KeyProcessing::GetKey(attributetypestr, keys);
				switch (attribute) {
				case TEXTAttribute::CHARACTER_SIZE: {
					if (!attributekey.first) return TEXTAttribute::NULLTYPE;
					try { character_size = std::stoi(attributekey.second->second); }
					catch (...) { return TEXTAttribute::NULLTYPE; }
					break;
				}
				case TEXTAttribute::STRING: {
					if (!attributekey.first) return TEXTAttribute::NULLTYPE;
					text_string = attributekey.second->second;
					break;
				}
				case TEXTAttribute::POSITION_PERCENTAGE: {
					auto percentagex = KeyProcessing::GetKey("POSITION_PERCENTAGE_X", keys);
					auto percentagey = KeyProcessing::GetKey("POSITION_PERCENTAGE_Y", keys);
					bool valid = false;
					if (percentagex.first) {
						valid = true;
						try { position_proportion.x = std::stoi(percentagex.second->second) / 100; }
						catch(const std::exception& exception){}
					}
					if (percentagey.first) {
						valid = true;
						try { position_proportion.y = std::stoi(percentagey.second->second) / 100; }
						catch (const std::exception& exception) {}
					}
					if (!valid) return TEXTAttribute::NULLTYPE; //at least one key has to be present for the change to be registered.
					break;
				}
				case TEXTAttribute::ORIGIN_PERCENTAGE: {
					auto originpercentagex = KeyProcessing::GetKey("ORIGIN_PERCENTAGE_X", keys);
					auto originpercentagey = KeyProcessing::GetKey("ORIGIN_PERCENTAGE_Y", keys);
					bool valid = false;
					if (originpercentagex.first) {
						valid = true;
						try { origin_proportion.x = std::stoi(originpercentagex.second->second) / 100; }
						catch (const std::exception& exception) {}
					}
					if (originpercentagey.first) {
						valid = true;
						try { origin_proportion.y = std::stoi(originpercentagey.second->second) / 100; }
						catch (const std::exception& exception) {}
					}
					if (!valid) return TEXTAttribute::NULLTYPE; //at least one key has to be present for the change to be registered.
					bool successful = false;
					if (!successful) return TEXTAttribute::NULLTYPE;
					break;
				}
				case TEXTAttribute::FONT_NAME: {
					if (!attributekey.first) return TEXTAttribute::NULLTYPE;
					font_name = attributekey.second->second;
					break;
				}
					 return attribute;
				}
			}
		};
		template<typename T>
		using IS_TEXT = std::is_same<typename std::decay_t<T>, Text>;
	}
	using namespace BackgroundData;
	using namespace TextData;
	class GUIStyle {
		friend class GUIVisual;
	private:
		mutable bool pendingtextapply = false;
		mutable bool pendingbgapply = false;
		BG background;
		Text text;
	public:
		template<typename T, typename = typename std::enable_if_t<IS_TEXT<T>::value || IS_BG<T>::value>>
		auto ReadIn(const KeyProcessing::Keys& keys) {
			std::string propertytype;
			try { propertytype = keys.find("PROPERTY_ATTRIBUTE")->second; }
			catch (const std::exception& exception) { throw CustomException("Unable to find the {PROPERTY_ATTRIBUTE,TYPE} key"); }
			if constexpr (IS_BG<T>::value) {
				BGAttribute res = background.ReadIn(std::move(propertytype), keys);
				if (res != BGAttribute::NULLTYPE) pendingbgapply = true;
				return res;
			
			}
			else if constexpr (IS_TEXT<T>::value) {
				TEXTAttribute res = text.ReadIn(std::move(propertytype), keys);
				if (res != TEXTAttribute::NULLTYPE) pendingtextapply = true;
				return res;
			}
		}
	};

	template<typename PROPERTY, typename = typename std::enable_if_t<IS_TEXT<PROPERTY>::value || IS_BG<PROPERTY>::value>>
	using DEDUCE_PROPERTY_ATTRIBUTE = std::conditional_t<IS_BG<PROPERTY>::value, BGAttribute,TEXTAttribute>;


	using GUIStateStyles = std::array<GUIStyle, 3>;
	using GUIData::GUIStateData::GUIState;
	class GUIVisual {
	private:
		GUIStateStyles statestyles;

		sf::Vector2f elementsize;
		sf::Vector2f elementlocalposition;

		GUIState activestate;
		GUIState previousstate;

		bool pendingparentredraw;
		bool pendingstateapply;
		bool pendingsizeapply;
		bool pendingpositionapply;
		bool pendingupdate;


		sf::RectangleShape background;
		sf::Text text;
		std::shared_ptr<sf::Font> font;
		std::shared_ptr<sf::Texture> texture;

		Manager_Texture* texturemgr;
		Manager_Font* fontmgr;

		template<typename T, typename = typename ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<T>::type>
		std::shared_ptr<T>& GetResource() {
			if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Font>) return font;
			else if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Texture>) return texture;
		}
		template<typename RESOURCE,typename = typename ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<RESOURCE>::type>
		RESOURCE* RequestVisualResource() {
			typename ManagedResourceData::DEDUCE_RESOURCE_MANAGER_TYPE<RESOURCE> resmanager;
			auto& activestyle = statestyles.at(static_cast<int>(activestate));
			std::string resname;
			if constexpr (ManagedResourceData::IS_FONT<RESOURCE>::value) {
				resmanager = fontmgr;
				resname = activestyle.text.font_name;
			}
			else if constexpr (ManagedResourceData::IS_TEXTURE<RESOURCE>::value) {
				resmanager = texturemgr;
				resname = activestyle.background.texture_name;
			}
			if (resname.empty()) return nullptr;
			GetResource<RESOURCE>() = resmanager->RequestResource(resname);
			return GetResource<RESOURCE>().get();
		}
	

		template<typename RESOURCE, typename = typename ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<RESOURCE>::type>
		void ReleasePreviousStyleResource() {
			auto& activestyle = statestyles.at(static_cast<int>(activestate));
			std::string resname;
			GetResource<RESOURCE>().reset();
			if constexpr (resname = activestyle.text.font_name; !resname.empty() && ManagedResourceData::IS_FONT<RESOURCE>::value) fontmgr->RequestResourceDealloc(resname);
			else if constexpr (resname = activestyle.background.texture_name; !resname.empty() && ManagedResourceData::IS_TEXTURE<RESOURCE>::value) texturemgr->RequestResourceDealloc(resname);
		}
		void ReleasePrevStyleResources() {
			auto& style = GetStyle(previousstate);
			if (!style.background.texture_name.empty()) {
				texture.reset();
				if (texturemgr) texturemgr->RequestResourceDealloc(style.background.texture_name);
			}
			if (!style.text.font_name.empty()) {
				font.reset();
				if (fontmgr) fontmgr->RequestResourceDealloc(style.text.font_name);
			}
		}
		void ApplySize() {
			background.setSize(elementsize);
			pendingsizeapply = false;
		}
		void ApplyState(GUIStyle& activestyle, const sf::FloatRect& eltboundingbox) {
			ReleasePrevStyleResources();
			ApplyBackground(activestyle);
			ApplyText(activestyle, eltboundingbox);
			pendingstateapply = false;
		}
		void ApplyText(GUIStyle& activestyle, const sf::FloatRect& eltlocalboundingbox) {
			auto& textattr = activestyle.text;
			text.setFillColor(textattr.fill_color);
			text.setCharacterSize(textattr.character_size);
			RequestVisualResource<sf::Texture>();
			text.setFont(*RequestVisualResource<sf::Font>());
			auto& str = textattr.text_string;
			text.setString(str);
			str.erase(std::remove(str.begin(), str.end(), '\b'), str.end());
			//remove any \\n so that sf::text formats \n properly.
			for (auto it = str.begin(); it != str.end(); ++it) {
				if (*it == '\\') {
					if (it + 1 != str.end()) {
						if (*(it + 1) != 'n') continue;
						*it = '\n';
						str.erase(it + 1);
					}
				}
			}
			sf::Vector2f textsize;
			sf::Vector2f localorigin;
			sf::Vector2f textposition;
			auto CalculateAndApply = [&textsize, &localorigin, &textposition, &eltlocalboundingbox, &textattr](sf::Text& text, const unsigned int& charactersize) {
				text.setCharacterSize(textattr.character_size);
				textsize = sf::Vector2f{ text.getLocalBounds().width, text.getLocalBounds().height }; //size of the bounding box.
				localorigin = { textattr.origin_proportion.x * textsize.x, textattr.origin_proportion.y * textsize.y }; //local origin as a proportion of the size.
				textposition = sf::Vector2f{ eltlocalboundingbox.left, eltlocalboundingbox.top };//plus the top left position of the element its being drawn relative to
				textposition += sf::Vector2f{ textattr.position_proportion.x * eltlocalboundingbox.width, textattr.position_proportion.y * eltlocalboundingbox.height };//plus the position of the actual text itself as a proportion of the parent element size
				textposition -= sf::Vector2f{ text.getLocalBounds().left, text.getLocalBounds().top };//minus the default padding that sfml inserts with sf::Text::getLocalBounds()
				text.setOrigin(localorigin);
				text.setPosition(textposition);
			};
			auto TextFits = [&eltlocalboundingbox, &textposition, &localorigin, &textsize]()->bool {
				sf::Vector2f texttopleft{ textposition - localorigin };
				if (texttopleft.x < eltlocalboundingbox.left) return false;
				if (texttopleft.x + textsize.x > eltlocalboundingbox.left + eltlocalboundingbox.width) return false;
				if (texttopleft.y < eltlocalboundingbox.top) return false;
				if (texttopleft.y + textsize.y > eltlocalboundingbox.top + eltlocalboundingbox.height) return false;
				return true;
			};
			CalculateAndApply(text, textattr.character_size);
			if (!TextFits()) {
				//must find the maximum charactersize, while maintaining this position, which allows us to fit in our element.
				unsigned int newcharsize = textattr.character_size;
				while (newcharsize > 1) {
					CalculateAndApply(text, newcharsize);
					if (TextFits()) break;
					--newcharsize;
				}
				textattr.character_size = newcharsize;
			}
			activestyle.pendingtextapply = false;
		}
		void ApplyBackground(GUIStyle& activestyle) {
			const auto& bg = activestyle.background;
			background.setFillColor(bg.fill_color);
			background.setOutlineColor(bg.outline_color);
			background.setOutlineThickness(bg.outline_thickness);
			background.setTexture(RequestVisualResource<sf::Texture>());
			background.setTextureRect(bg.texture_rect);
			activestyle.pendingbgapply = false;
		}
		void ApplyPosition() {
			background.setPosition(elementlocalposition);
			text.setPosition(elementlocalposition);
			pendingpositionapply = false;
		}

	public:
		GUIVisual(Manager_Texture* tmgr, Manager_Font* fmgr) :texturemgr(tmgr), fontmgr(fmgr) {
			background.setFillColor(sf::Color::Transparent);
			text.setFillColor(sf::Color::Transparent);
			text.setPosition(sf::Vector2f{ 0,0 });
		}

		template<BGAttribute arg>
		const auto& GetBGAttribute() {
			auto& background = statestyles.at(static_cast<int>(activestate)).background;
			if constexpr (arg == BGAttribute::FILL_COLOR) return background.fill_color;
			else if constexpr (arg == BGAttribute::OUTLINE_COLOR) return background.outline_color;
			else if constexpr (arg == BGAttribute::OUTLINE_THICKNESS) return background.outline_thickness;
			else if constexpr (arg == BGAttribute::TEXTURE_NAME) return background.texture_name;
			else if constexpr (arg == BGAttribute::TEXTURE_RECT) return background.texture_rect;
		}
		template<TEXTAttribute arg>
		const auto& GetTEXTAttribute() {
			auto& text = statestyles.at(static_cast<int>(activestate)).text;
			if constexpr (arg == TEXTAttribute::CHARACTER_SIZE) return text.character_size;
			else if constexpr (arg == TEXTAttribute::FONT_NAME) return text.font_name;
			else if constexpr (arg == TEXTAttribute::ORIGIN_PERCENTAGE) return text.origin_proportion;
			else if constexpr (arg == TEXTAttribute::POSITION_PERCENTAGE) return text.position_proportion;
			else if constexpr (arg == TEXTAttribute::STRING) return text.text_string;
			else if constexpr (arg == TEXTAttribute::HIDDEN) return text.text_hidden;
		}
		bool& Update(const sf::FloatRect& eltrect) {
			auto& activestyle = GetStyle(activestate);
			if (pendingpositionapply) ApplyPosition();
			if (pendingsizeapply) ApplySize();
			if (pendingstateapply) ApplyState(activestyle, eltrect);
			//apply individual, non state changes made by the user.
			if (activestyle.pendingbgapply) ApplyBackground(activestyle);
			if (activestyle.pendingtextapply) ApplyText(activestyle, eltrect);

			return pendingparentredraw;
		}
		template<typename PROPERTY, typename = typename std::enable_if_t<IS_BG<PROPERTY>::value || IS_TEXT<PROPERTY>::value>, typename PROPERTY_ATTRIBUTE = typename DEDUCE_PROPERTY_ATTRIBUTE<PROPERTY>>
		void ReadIn(const GUIData::GUIStateData::GUIState& state, const KeyProcessing::Keys& keys) {
			statestyles[static_cast<int>(state)].ReadIn<PROPERTY>(keys);
			/*if (state == activestate) QueueState(state);*/
		}
		void QueuePosition(const sf::Vector2f& position) {
			elementlocalposition = position;
			pendingpositionapply = true;
			pendingparentredraw = true;
		}
		const sf::Vector2f& GetElementSize() const { return elementsize; }
		const sf::Vector2f& GetElementPosition() const { return elementlocalposition; }
		void QueueSize(const sf::Vector2f& size) {
			elementsize = size;
			pendingsizeapply = true;
			pendingparentredraw = true;
		}
		void QueueState(const GUIState& state) {
			previousstate = activestate;
			activestate = state;
			GetStyle(activestate).pendingbgapply = true;
			GetStyle(activestate).pendingtextapply = true;
			pendingstateapply = true;
			pendingparentredraw = true;
		}
		void Render(sf::RenderTarget& target, const bool& toparent) {
			target.draw(background);
			if (!GetStyle(activestate).text.text_hidden)target.draw(text);
			//if this has been draw onto its parent, then it has been redrawn.
			if(toparent) pendingparentredraw = false;
		}
		GUIStyle& GetStyle(const GUIState& state) {
			return statestyles.at(static_cast<int>(state));
		}
		void ChangeStyle(const GUIState& state, GUIStyle& style) {
			GetStyle(state) = style;
			if (activestate == state) QueueState(state);
		}
		const bool& PendingParentRedraw() const { return pendingparentredraw; }
	};

}
#endif