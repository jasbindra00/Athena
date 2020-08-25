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
#include <type_traits>
#include "MagicEnum.h"
#include <variant>
#include "Utility.h"


class Manager_GUI;
namespace GUIFormattingData {
	using KeyProcessing::Keys;
	using KeyProcessing::FoundKeys;
	enum class PROPERTY_TYPE {
		BG, TEXT, NULLTYPE
	};
	
	enum class STYLE_ATTRIBUTE {
		BG_FILL_COLOR, BG_OUTLINE_COLOR, BG_OUTLINE_THICKNESS,BG_TEXTURE_NAME, BG_TEXTURE_RECT, 
		TEXT_STRING, TEXT_FILL_COLOR, TEXT_CHARACTER_SIZE,
		TEXT_POSITION, TEXT_ORIGIN, TEXT_FONT_NAME,TEXT_HIDDEN,
		NULLTYPE
	};
	static std::pair<unsigned int, unsigned int> BG_RANGE{ 0,4 };
	static std::pair<unsigned int, unsigned int> TEXT_RANGE{ 5,11 };
	using AttributeVariant = std::variant<sf::Color, std::string, sf::IntRect, double, bool, sf::Vector2f>;
	using AttributeLookup = std::unordered_map<STYLE_ATTRIBUTE, AttributeVariant>;
	class GUIStyle {//EXPOSED TO USER THROUGH GET STYLE.
		friend class Manager_GUI;
		friend class GUIVisual;
	private:
		AttributeLookup attributes;
		mutable bool pending_text_apply{ true };
		mutable bool pending_bg_apply{ true };
		STYLE_ATTRIBUTE ReadIn(const KeyProcessing::Keys& keys) {
			//find the STYLE_ATTRIBUTE key, eg {STYLE_ATTRIBUTE,TEXT_POSITION}
			auto style_attribute_key = KeyProcessing::GetKey("STYLE_ATTRIBUTE", keys);
			STYLE_ATTRIBUTE type{ STYLE_ATTRIBUTE::NULLTYPE };
			if (!style_attribute_key.first) return type;
			//{STYLE_ATTRIBUTE,ATTRIBUTE} exists -> convert the ATTRIBUTE into the registered enum values. Return if invalid.
			{auto valid_attribute = magic_enum::enum_cast<STYLE_ATTRIBUTE>(style_attribute_key.second->second);
			if (!valid_attribute.has_value()) return type;
			type = valid_attribute.value();
			}
			auto attrstr = static_cast<std::string>(magic_enum::enum_name(type));
			//We want to read as many valid entires as possible. The read is successful if at least one entry is read.
			bool successful{ false };
			AttributeVariant result;
			//Helper Lambda for setting the result of the read in.
			auto SetResult = [&result, &successful](AttributeVariant arg)->void {
				result = std::move(arg);
				successful = true;
			};
			if (type == STYLE_ATTRIBUTE::BG_FILL_COLOR || type == STYLE_ATTRIBUTE::BG_OUTLINE_COLOR || type == STYLE_ATTRIBUTE::TEXT_FILL_COLOR) {
				std::pair<bool, sf::Color> res = Utility::Conversions::ProcessColor(keys, std::get<sf::Color>(attributes.at(type)));
				if (res.first) SetResult(std::move(res.second));
			}
			else if (type == STYLE_ATTRIBUTE::TEXT_POSITION || type == STYLE_ATTRIBUTE::TEXT_ORIGIN) {
				sf::Vector2f defaultvec = std::get<sf::Vector2f>(attributes.at(type));
				KeyProcessing::FoundKeys foundkeys;
				if (type == STYLE_ATTRIBUTE::TEXT_POSITION) foundkeys = KeyProcessing::GetKeys({ "POSITION_X", "POSITION_Y" }, keys);
				else foundkeys = KeyProcessing::GetKeys({ "ORIGIN_X", "ORIGIN_Y" }, keys);
				//Populate the array with the default TEXT_POSITION / TEXT_ORIGIN value. 
				std::array<double, 2>  arr_res{ defaultvec.x, defaultvec.y };
				//Read any valid entries into the array.
				for (int i = 0; i < 2; ++i) {
					auto& key = foundkeys.at(i);
					if (key.first) {
						std::pair<bool, double> res = Utility::Conversions::ConvertToDouble(key.second->second);
						if (res.first) {
							successful = true;
							arr_res.at(i) = std::move(res.second) / 100; //USER INSERTS A PERCENTAGE.
						}
					}
				}
				if (successful) SetResult(sf::Vector2f{ static_cast<float>(arr_res.at(0)), static_cast<float>(arr_res.at(1)) });
			}
			else if (type == STYLE_ATTRIBUTE::BG_TEXTURE_RECT) {
				sf::IntRect defaultintrect = std::get<sf::IntRect>(attributes.at(type));
				KeyProcessing::FoundKeys init = KeyProcessing::GetKeys({ "TOP_LEFT_X","TOP_LEFT_Y","PIXEL_SIZE_X","PIXEL_SIZE_Y" }, keys);
				std::array<int, 4> vals{ std::move(defaultintrect.left), std::move(defaultintrect.top), std::move(defaultintrect.width), std::move(defaultintrect.height) };
				if (successful) SetResult(sf::IntRect{ std::move(vals.at(0)), std::move(vals.at(1)), std::move(vals.at(2)), std::move(vals.at(3)) });
			}
			else if (auto initkey = KeyProcessing::GetKey(attrstr, keys); type == STYLE_ATTRIBUTE::BG_OUTLINE_THICKNESS || type == STYLE_ATTRIBUTE::TEXT_CHARACTER_SIZE) {
				if (initkey.first) {
					auto res = Utility::Conversions::ConvertToDouble(initkey.second->second);
					if (res.first) SetResult(std::move(res.second));
				}
			}
			else if (type == STYLE_ATTRIBUTE::TEXT_HIDDEN) {
				if (initkey.first) {
					std::string str_res = initkey.second->second;
					if (str_res == "TRUE" || str_res == "FALSE") SetResult(str_res == "TRUE");
				}
			}
			else if (type == STYLE_ATTRIBUTE::BG_TEXTURE_NAME || type == STYLE_ATTRIBUTE::TEXT_FONT_NAME || type == STYLE_ATTRIBUTE::TEXT_STRING) {
				if (initkey.first) SetResult(std::move(initkey.second->second));
			}

			if (successful) {
				attributes.at(type) = std::move(result);
				(GetPropertyType(type) == PROPERTY_TYPE::BG) ? pending_bg_apply = true : pending_text_apply = true;
				return type;
			}
			return STYLE_ATTRIBUTE::NULLTYPE;
		}
		static PROPERTY_TYPE GetPropertyType(const STYLE_ATTRIBUTE& type) {
			auto ind = magic_enum::enum_index(type);
			if (ind >= BG_RANGE.first && ind <= BG_RANGE.second) return PROPERTY_TYPE::BG;
			else if (ind >= TEXT_RANGE.first && ind <= TEXT_RANGE.second) return PROPERTY_TYPE::TEXT;
			return PROPERTY_TYPE::NULLTYPE;
		}

	public:
		GUIStyle() {
			sf::Color defaultc(255, 255, 255, 255);
			attributes[STYLE_ATTRIBUTE::BG_FILL_COLOR] = defaultc;
			attributes[STYLE_ATTRIBUTE::BG_OUTLINE_COLOR] = defaultc;
			attributes[STYLE_ATTRIBUTE::TEXT_FILL_COLOR] = std::move(defaultc);
			attributes[STYLE_ATTRIBUTE::BG_TEXTURE_NAME] = std::string{ "" };
			attributes[STYLE_ATTRIBUTE::TEXT_FONT_NAME] = std::string{};
			attributes[STYLE_ATTRIBUTE::BG_TEXTURE_RECT] = sf::IntRect{0,0,0,0};
			attributes[STYLE_ATTRIBUTE::TEXT_STRING] = std::string{};
			attributes[STYLE_ATTRIBUTE::BG_OUTLINE_THICKNESS] = 1.0;
			attributes[STYLE_ATTRIBUTE::TEXT_CHARACTER_SIZE] = 30.0;
			attributes[STYLE_ATTRIBUTE::TEXT_POSITION] = sf::Vector2f{ 0.5,0.5 };
			attributes[STYLE_ATTRIBUTE::TEXT_ORIGIN] = sf::Vector2f{ 0.5,0.5 };
			attributes[STYLE_ATTRIBUTE::TEXT_HIDDEN] = false;
		}
		STYLE_ATTRIBUTE ReadIn(const STYLE_ATTRIBUTE& attribute, const AttributeVariant& val) {
			auto attrstr = static_cast<std::string>(magic_enum::enum_name(attribute));
			if (attrstr.empty()) return STYLE_ATTRIBUTE::NULLTYPE;
			//check if the required types match.
			auto& attribute_variant = attributes.at(attribute);
			if (val.index() != attribute_variant.index()) return STYLE_ATTRIBUTE::NULLTYPE;
			attribute_variant = val;
			PROPERTY_TYPE property_type = GetPropertyType(attribute);
			(property_type == PROPERTY_TYPE::BG) ? pending_bg_apply = true : pending_text_apply = true;
			return attribute;
		}
		//CHANGE FCN FOR AUTOMATIC CONVERSION TO TYPE (SHIELD STD::GET<> FROM USER)
		const AttributeVariant& GetAttribute(const STYLE_ATTRIBUTE& attr) const { return attributes.at(attr); }
		const bool& PendingTextApply() const { return pending_text_apply; }
		const bool& PendingBGApply() const { return pending_bg_apply;}
	};



	using GUIStateStyles = std::array<GUIStyle, 3>;
	using GUIData::GUIStateData::GUIState;
	class GUIVisual {//EXPOSED TO USER.
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

		sf::RectangleShape texture_background;
		sf::RectangleShape solid_background;
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
				resname = std::get<std::string>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_FONT_NAME));
			}
			else if constexpr (ManagedResourceData::IS_TEXTURE<RESOURCE>::value) {
				resmanager = texturemgr;
				resname = std::get<std::string>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_TEXTURE_NAME));
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
			if constexpr (ManagedResourceData::IS_FONT<RESOURCE>::value) fontmgr->RequestResourceDealloc(std::get<std::string>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_FONT_NAME)));
			else if constexpr (ManagedResourceData::IS_TEXTURE<RESOURCE>::value) texturemgr->RequestResourceDealloc(std::get<std::string>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_TEXTURE_NAME)));
		}
		void ApplySize() {
			solid_background.setSize(elementsize);
			texture_background.setSize(elementsize);
			pendingsizeapply = false;
			pendingparentredraw = true;
		}
		void ApplyText(GUIStyle& activestyle, const sf::FloatRect& eltlocalboundingbox) {
			text.setFillColor(std::get<sf::Color>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_FILL_COLOR)));
			text.setCharacterSize(std::get<double>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_CHARACTER_SIZE)));
			text.setFont(*RequestVisualResource<sf::Font>());
			auto& str = std::get<std::string>(activestyle.attributes.at(STYLE_ATTRIBUTE::TEXT_STRING));
			str.erase(std::remove(str.begin(), str.end(), '\b'), str.end());
			for (auto it = str.begin(); it != str.end(); ++it) {
				if (*it == '\\') {
					if (it + 1 != str.end()) {
						if (*(it + 1) != 'n') continue;
						*it = '\n';
						str.erase(it + 1);
					}
				}
			}
			text.setString(str);
			sf::Vector2f textsize;
			sf::Vector2f localorigin;
			sf::Vector2f textposition;
			const auto& origin_proportion = std::get<sf::Vector2f>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_ORIGIN));
			const auto& position_proportion = std::get<sf::Vector2f>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_POSITION));
			auto CalculateAndApply = [&textsize, &localorigin, &textposition, &eltlocalboundingbox, &origin_proportion, &position_proportion](sf::Text& text, const unsigned int& character_size) {
				text.setCharacterSize(character_size);
				textsize = sf::Vector2f{ text.getLocalBounds().width, text.getLocalBounds().height }; //size of the bounding box.
				localorigin = { origin_proportion.x * textsize.x, origin_proportion.y * textsize.y }; //local origin as a proportion of the size.
				textposition = sf::Vector2f{ eltlocalboundingbox.left, eltlocalboundingbox.top };//plus the top left position of the element its being drawn relative to
				textposition += sf::Vector2f{ position_proportion.x * eltlocalboundingbox.width, position_proportion.y * eltlocalboundingbox.height };//plus the position of the actual text itself as a proportion of the parent element size
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
			
			auto new_char_size = static_cast<double>(text.getCharacterSize());
			CalculateAndApply(text, new_char_size);
			if (!TextFits()) {
				//must find the maximum charactersize, while maintaining this position, which allows us to fit in our element.
				while (new_char_size > 1) {
					CalculateAndApply(text, new_char_size);
					if (TextFits()) break;
					--new_char_size;
				}
				activestyle.attributes.at(STYLE_ATTRIBUTE::TEXT_CHARACTER_SIZE) = std::move(new_char_size);
			}
			activestyle.pending_text_apply = false;
			pendingparentredraw = true;
		}
		void ApplyBackground(GUIStyle& activestyle) {
			if (activestyle.pending_bg_apply) {
				int x = 3;
			}
			solid_background.setFillColor(std::get<sf::Color>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_FILL_COLOR)));
			solid_background.setOutlineColor(std::get<sf::Color>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_OUTLINE_COLOR)));
			solid_background.setOutlineThickness(std::get<double>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_OUTLINE_THICKNESS)));
			texture_background.setTextureRect(std::get<sf::IntRect>(activestyle.attributes.at(STYLE_ATTRIBUTE::BG_TEXTURE_RECT)));
			texture_background.setTexture(RequestVisualResource<sf::Texture>());
			activestyle.pending_bg_apply = false;
			pendingparentredraw = true;
		}
		void ApplyPosition() {
			solid_background.setPosition(elementlocalposition);
			texture_background.setPosition(elementlocalposition);
			text.setPosition(elementlocalposition);
			pendingpositionapply = false;
			pendingparentredraw = true;
		}
		void ReleasePrevStyleResources() {
			auto& style = GetStyle(previousstate);
			auto& bgresname = std::get<std::string>(style.GetAttribute(STYLE_ATTRIBUTE::BG_TEXTURE_NAME));
			if (!bgresname.empty()) {
				texture.reset();
				if (texturemgr) texturemgr->RequestResourceDealloc(bgresname);
			}
			auto& fontresname = std::get<std::string>(style.GetAttribute(STYLE_ATTRIBUTE::TEXT_FONT_NAME));
			if (!fontresname.empty()) {
				font.reset();
				if (fontmgr) fontmgr->RequestResourceDealloc(fontresname);
			}
		}
		void ApplyState(GUIStyle& activestyle, const sf::FloatRect& eltboundingbox) {
			ReleasePrevStyleResources();
			ApplyBackground(activestyle);
			ApplyText(activestyle, eltboundingbox);
			pendingstateapply = false;
			pendingparentredraw = true;
		}
	public:
		GUIVisual(Manager_Texture* tmgr, Manager_Font* fmgr, const GUIStateStyles& styles = GUIStateStyles{}) :texturemgr(tmgr), fontmgr(fmgr),statestyles(styles) {
			pendingstateapply = true;
		}
		const bool& Update(const sf::FloatRect& eltrect) {

			auto& activestyle = GetStyle(activestate);
			if (pendingstateapply) ApplyState(activestyle, eltrect);
			if (pendingpositionapply) ApplyPosition();
			if (pendingsizeapply) ApplySize();
			//apply individual, non state changes made by the user.
			if (activestyle.PendingBGApply()) ApplyBackground(activestyle);
			if (activestyle.PendingTextApply()) ApplyText(activestyle, eltrect);
			return pendingparentredraw;
		}
		void QueuePosition(const sf::Vector2f& position) {
			elementlocalposition = position;
			pendingpositionapply = true;
		}
		const sf::Vector2f& GetElementSize() const { return elementsize; }
		const sf::Vector2f& GetElementPosition() const { return elementlocalposition; }
		void QueueSize(const sf::Vector2f& size) {
			elementsize = size;
			pendingsizeapply = true;
		}
		void QueueState(const GUIState& state) {
			previousstate = activestate;
			activestate = state;
			pendingstateapply = true;
		}
		void Draw(sf::RenderTarget& target, const bool& toparent) {
			target.draw(solid_background);
			if(texture.get()) target.draw(texture_background);
			if(!std::get<bool>(statestyles.at(static_cast<int>(activestate)).GetAttribute(STYLE_ATTRIBUTE::TEXT_HIDDEN))) target.draw(text);
			if(toparent) pendingparentredraw = false;
		}
		GUIStyle& GetStyle(const GUIState& state) {return statestyles.at(static_cast<int>(state));}
		void ChangeStyle(const GUIState& state, GUIStyle& style) {
			GetStyle(state) = style;
			if (activestate == state) QueueState(state);
		}
		const bool& PendingParentRedraw() const { return pendingparentredraw; }
		const bool& PendingSizeApply() const { return pendingsizeapply; }
		void QueueParentRedraw() { pendingparentredraw = true; }//ENCAPSULATE
	};
}
#endif