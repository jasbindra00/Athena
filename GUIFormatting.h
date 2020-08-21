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
#include "Manager_Texture.h"
#include "ManagedResources.h"
#include "Manager_Font.h"
#include "GUIElement.h"
#include "EventQueue.h"
#include <array>


class GUIElement;
class GUIInterface;
class GUILabel;
class GUITextfield;
class GUICheckbox;

namespace GUIFormatting {
	using GUIData::GUIStateData::GUIState;
	namespace BackgroundRectData
	{

		static enum class Background_Attributes {
			FILL_COLOR, OUTLINE_COLOR, OUTLINE_THICKNESS
		};
		template<Background_Attributes T>
		using DEDUCE_BACKGROUND_ARG_TYPE = std::conditional_t<std::is_same_v<typename std::decay_t<T>, Background_Attributes::FILL_COLOR> || std::is_same_v<typename std::decay_t<T>, Background_Attributes::OUTLINE_COLOR>, sf::Color, unsigned int>;
		static enum class Texture_Attributes {
			TEXTURE_NAME, TEXTURE_RECT
		};
		template<Texture_Attributes T>
		using DEDUCE_TEXTURE_ARG_TYPE = std::conditional_t < std::is_same_v<typename std::decay_t<T>, Texture_Attributes::TEXTURE_NAME>, std::string, sf::IntRect>;
		EnumConverter<Background_Attributes> bgattributeconverter([](const std::string& str)->Background_Attributes {
			if (str == "FILL_COLOR") return Background_Attributes::FILL_COLOR;
			else if (str == "OUTLINE_COLOR") return Background_Attributes::OUTLINE_COLOR;
			else if (str == "OUTLINE_THICKNESS") return Background_Attributes::OUTLINE_THICKNESS;
			});
		EnumConverter<Texture_Attributes> textureattributeconverter([](const std::string& str)->Texture_Attributes {
			if (str == "TEXTURE_NAME") return Texture_Attributes::TEXTURE_NAME;
			else if (str == "TEXTURE_RECT") return Texture_Attributes::TEXTURE_NAME;
			});
		using namespace BackgroundRectData;

		template<typename DERIVED, typename ATTRTYPE>
		struct BackgroundRectProperties {
		public:
			template<ATTRTYPE TYPE, typename T>
			void SetAttribute(const T& arg) {
				static_cast<DERIVED&>(*this).GetAttribute<TYPE>() = arg;
			}
		private:
			std::array<std::pair<Background_Attributes, bool>, 3> background_queue;
			sf::Color outline_color;
			sf::Color fill_color;
			unsigned int outline_thickness;
			template<Background_Attributes ATTRTYPE>
			auto& GetAttribute() {
				return static_cast<DERIVED*>(&this).GetAttribute<ATTRTYPE>();
			}
		};
		struct SBGProperties :public BackgroundRectProperties<SBGProperties, Background_Attributes> {
			template<Background_Attributes ATTRTYPE>
			auto& GetAttribute() {
				switch (ATTRTYPE) {
				case Background_Attributes::FILL_COLOR: {return fill_color; }
				case Background_Attributes::OUTLINE_COLOR: {return outline_color; }
				case Background_Attributes::OUTLINE_THICKNESS: {return outline_thickness; }
				}
			}
		};
		struct TBGProperties :public BackgroundRectProperties<TBGProperties, Texture_Attributes> {
			template<Texture_Attributes ATTRTYPE>
			void SetTextureAttribute(const DEDUCE_TEXTURE_ARG_TYPE<ATTRTYPE>& arg) {
				GetAttribute<ATTRTYPE>() = arg;
			}
			template<Texture_Attributes ATTRTYPE>
			auto& GetAttribute() {
				switch (ATTRTYPE) {
				case Texture_Attributes::TEXTURE_NAME: {return texture_name; }
				case Texture_Attributes::TEXTURE_RECT: {return texture_rect; }
				}
			}
			std::array<std::pair<Texture_Attributes, bool>, 2> texture_queue;
			std::string texture_name;
			sf::IntRect texture_rect;

		};
	}

	using namespace BackgroundRectData;
	static const unsigned int maxcharactersize = 30;
	struct Background {
	private:
		SBGProperties sbgproperties;
		TBGProperties tbgproperties;
		template<typename T>
		T& GetBGproperty() {
			if constexpr (std::is_same_v<typename std::decay_t<T>, SBGProperties>) return sbgproperties;
			else if (constexpr std::is_same_v<typename std::decay_t<T>, TBGProperties>) return tbgproperties;
		}
	public:
		
		template<typename PROPERTY_TYPE, typename = typename std::enable_if_t<std::is_same_v<typename std::decay_t<PROPERTY_TYPE>, SBGProperties> || std::is_same_v<typename std::decay_t<PROPERTY_TYPE>, TBGProperties>>, typename ATTRTYPE, typename = typename std::enable_if_t < std::is_same_v<typename std::decay_t<ATTRTYPE>, Background_Attributes> || std::is_same_v<typename std::decay_t<ATTRTYPE>, Texture_Attributes>>>
		void SetBGPropertyAttribute() {
			auto& bgproperty = GetBGproperty<PROPERTY_TYPE>();
			
			

		}




		void ReadIn(const BACKGROUND_ATTRIBUTE& attributetype, KeyProcessing::Keys& keys) {
			switch (attributetype) {
			case BACKGROUND_ATTRIBUTE::SBG_FILL_COLOR: {

			}
			}
		}




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
			pendingbackground = true;
		}
	};
	struct Text {
		friend class GUIVisual;
	private:
		sf::Vector2f localpositionproportion;
		sf::Vector2f originproportion{ 0,0 };
		sf::Color textcolor{ sf::Color::Color(255,255,255,255 )};
		std::string fontname{ "arial.ttf" };
		std::string customtext;
		bool texthidden = false;
		bool pendingtext = false;
		unsigned int charactersize{ maxcharactersize };
	public:
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
				KeyProcessing::FillMissingKey(KeyProcessing::KeyPair{ attributetype, "" }, keys);
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
				else if (attributetype == "CUSTOM_TEXT_STRING") {
					customtext = key->second;
					std::replace(customtext.begin(), customtext.end(), '+', ' ');
				}
			}
			pendingtext = true;
		}
	};
	template<typename T>
	using IS_TEXT = std::is_same<typename std::decay_t<T>, Text>;
	template<typename T>
	using IS_BACKGROUND = std::is_same<typename std::decay_t<T>, Background>;
	struct GUIStyle {
		GUIStyle() {
		}
		Text text;
		Background background;
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
			background.sbg_color = sf::Color::Color(255, 255, 255, 0);
		}
	};
	using GUIStateStyles = std::unordered_map<GUIState, GUIStyle>;
	class GUIVisual {
	private:
		GUIStateStyles statestyles;
		sf::Vector2f elementsize;
		sf::Vector2f position;
		sf::RectangleShape sbg;
		sf::RectangleShape tbg;
		sf::Text text;
		std::shared_ptr<sf::Font> font{ nullptr };
		std::shared_ptr<sf::Texture> tbg_texture{ nullptr };
		GUIState previousstate;
		GUIState activestate;

		mutable bool pendingstyle;
		mutable bool pendingredraw; //set to false after it has been rendered to its parent.
		mutable bool pendingposition;
		mutable bool pendingsize;

		template<typename T, typename = typename ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<T>>
		std::shared_ptr<T>& GetResource() {
			if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Font>) return font;
			else if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Texture>) return tbg_texture;
		}

		template<typename T, typename = typename ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<T>>
		bool RequestVisualResource() {
			auto& activestyle = GetActiveStyle();
			std::string* resname;
			if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Texture>) resname = &activestyle.background.tbg_name;
			else if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Font>) resname = &activestyle.text.fontname;
			if (resname->empty()) return false;
			auto newres = GetGUIManager()->GetContext()->GetResourceManager<T>()->RequestResource(*resname);
			if (newres == nullptr) {
				resname->clear();
				return false;
			}
			GetResource<T>() = std::move(newres);
			return true;
		}
		void ReleasePrevStyleResources() {
			auto& style = statestyles[previousstate];
			if (!style.background.tbg_name.empty()) {
				tbg_texture.reset();
				if (texturemgr) texturemgr->RequestResourceDealloc(style.background.tbg_name);
			}
			if (!style.text.fontname.empty()) {
				font.reset();
				if (fontmgr) fontmgr->RequestResourceDealloc(style.text.fontname);
			}
		}
		Manager_Texture* texturemgr;
		Manager_Font* fontmgr;
	protected:
		void QueuePosition(const sf::Vector2f& pos) {
			position = pos;
			pendingposition = true;
			QueueRedraw();
		}
		void QueueEltSize(const sf::Vector2f& s) {
			elementsize = s;
			pendingsize = true;
			QueueRedraw();
		}
		void QueueText(std::string str) {
			statestyles[activestate].text.customtext = str;
			pendingtext = true;
		}
		void QueueRedraw() const { pendingredraw = true; }
		void ResetRedraw() const { pendingredraw = false; }
		void QueueStyle() {//queues a pending style application
			pendingstyle = true;
			QueueRedraw();
		}
		inline void ApplyStyle(const sf::FloatRect& eltrect) {
			ReleasePrevStyleResources();
			//apply dynamically allocated resources to visuals
			if (RequestVisualResource<sf::Texture>()) { //request the required resources for the font and tbg.
				tbg.setTexture(tbg_texture.get());
				sf::Color tmp(255, 255, 255, 255);
				tbg.setFillColor(std::move(tmp));
			}
			else {//if unsucessful texture request, we don't want the default sfml error texture to show.
				sf::Color tmp(255, 255, 255, 0);
				tbg.setFillColor(std::move(tmp));
			}
			if (RequestVisualResource<sf::Font>()) text.setFont(*font);
			auto& style = statestyles[activestate];
			sbg.setFillColor(style.background.sbg_color);
			sbg.setOutlineColor(style.background.outlinecolor);
			sbg.setOutlineThickness(style.background.outlinethickness);
			style.background.pendingbackground = false;
			text.setFillColor(style.text.textcolor);
			if(!style.text.texthidden) style.text.pendingtext = false;
			
			text.style.text.textcolor
			style.text
			pendingstyle = false;
			
			pendingtext = true;
			//ApplyText(eltrect);
		}
		inline void ApplyText(const sf::FloatRect& eltlocalboundingbox) {
			auto& activestyletext = statestyles[activestate].text;
			auto str = activestyletext.customtext;
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
			text.setString(str);

			sf::Vector2f textsize;
			sf::Vector2f localorigin;
			sf::Vector2f textposition;
			auto CalculateAndApply = [&textsize, &localorigin, &textposition, &eltlocalboundingbox, &activestyletext](sf::Text& text, const unsigned int& charactersize) {
				text.setCharacterSize(activestyletext.charactersize);
				textsize = sf::Vector2f{ text.getLocalBounds().width, text.getLocalBounds().height }; //size of the bounding box.
				localorigin = { activestyletext.originproportion.x * textsize.x, activestyletext.originproportion.y * textsize.y }; //local origin as a proportion of the size.
				textposition = sf::Vector2f{ eltlocalboundingbox.left, eltlocalboundingbox.top };//plus the top left position of the element its being drawn relative to
				textposition += sf::Vector2f{ activestyletext.localpositionproportion.x * eltlocalboundingbox.width, activestyletext.localpositionproportion.y * eltlocalboundingbox.height };//plus the position of the actual text itself as a proportion of the parent element size
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
			CalculateAndApply(text, activestyletext.charactersize);
			if (!TextFits()) {
				//must find the maximum charactersize, while maintaining this position, which allows us to fit in our element.
				unsigned int newcharsize = activestyletext.charactersize;
				while (newcharsize > 1) {
					CalculateAndApply(text, newcharsize);
					if (TextFits()) break;
					--newcharsize;
				}
			}
			pendingtext = false;
			QueueRedraw();
		}
		inline void ApplyBackground() {
			auto& background = statestyles.at(activestate).background;
			BACKGROUND_ATTRIBUTE attribute;
			while (background.pendingchanges.PollEvent(attribute)) {

			}






















			while(background.pendingchanges.PollEvent())
			sbg.setFillColor(background.sbg_color);
			sbg.setOutlineColor(background.outlinecolor);
			if (RequestVisualResource<sf::Texture>()) { //request the required resources for the tbg.
				tbg.setTexture(tbg_texture.get());
				sf::Color tmp(255, 255, 255, 255);
				tbg.setFillColor(std::move(tmp));
			}
			background.
			background.
			sbg.set
			activestyle.background.
		}
		inline void ApplySize() {
			sbg.setSize(elementsize);
			tbg.setSize(elementsize);
			pendingsize = false;
		}
		inline void ApplyPosition() { //move entire elt visual.
			sbg.setPosition(position);
			tbg.setPosition(position);
			text.setPosition(position); //origin maintained.
			pendingposition = false;
		}
	public:
		GUIVisual(const GUIState& activestate) :activestate(activestate) {
			statestyles[GUIState::NEUTRAL] = GUIStyle{};
			statestyles[GUIState::CLICKED] = GUIStyle{};
			statestyles[GUIState::FOCUSED] = GUIStyle{};
		}
		virtual void Draw(sf::RenderTarget& target) const {
			target.draw(sbg);
			target.draw(tbg);
			if (!statestyles.at(activestate).text.texthidden) target.draw(text);
		}
		void OnVisualCreate(Manager_Texture* tmgr, Manager_Font* fmgr) {
			texturemgr = tmgr;
			fontmgr = fmgr;
			tbg.setFillColor(sf::Color::Transparent);
			text.setFillColor(sf::Color::Transparent);
			text.setPosition(sf::Vector2f{ 0,0 });
		}
		void ChangeStyle(const GUIStyle& style, const GUIState& state) {
			statestyles[state] = style;
			if (state == activestate) QueueStyle();
		}
		template<typename State, typename Property>
		void QueueState(const GUIState& state) {
			previousstate = activestate;
			activestate = state;
			QueueStyle();
		}

		template<typename T, typename  = typename std::enable_if_t<IS_BACKGROUND<T>::value || IS_TEXT<T>::value>>
		T& GetStateStyleProperty() const {
			auto& activestate = statestyles.at(activestate);
			if constexpr (IS_BACKGROUND<T>) return activestate.background;
			else if constexpr (IS_TEXT<T>) return activestate.text;
		}
	void ApplyChanges(const sf::FloatRect& eltboundingbox) {
		if (pendingsize) ApplySize();
		if (pendingposition) ApplyPosition();
		if (pendingstyle) ApplyStyle(eltboundingbox);
		if(statestyles.at(activestate).background.pendingbackground) ApplyBackground();
		if (statestyles.at(activestate).text.pendingtext) ApplyText(eltboundingbox);
	}
		const GUIStyle& GetStyle(const GUIState& state) const { return statestyles.at(state); }
		const sf::Vector2f& GetElementSize() const { return elementsize; }
		const sf::Vector2f& GetLocalPosition() const { return position; }
		inline std::string GetTextStr() const { return text.getString(); }
		virtual ~GUIVisual() {
			ReleaseStyleResources();
		}
	};
}
#endif