#ifndef GUIFORMATTING_H
#define GUIFORMATTING_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <variant>
#include <string>
#include <functional>
#include <array>
#include <cstdint>
#include "MagicEnum.h"
#include "CustomQueue.h"
#include "GUIData.h"
#include "Manager_Font.h"
#include "Manager_Texture.h"
class Manager_GUI;
/*
-Each component should have an apply function.
-When the queue reaches this iteration, it will then simply all Apply
*/
namespace GUIFormattingData {
	class StyleComponentBase
	{
	protected:
		AbstractStyle* parent_style;
		std::function<void(void)> applier;
		inline void AlertParentOfChange() { parent_style->Alert(this); }
	public:
		StyleComponentBase(AbstractStyle* p, std::function<void(void)> i_applier)
			:parent_style(p), applier(std::move(i_applier))
		{
		}
		inline void ApplyComponent() { applier(); }
		virtual ~StyleComponentBase() = 0;
	};



	/*
	-Keep an applier within a given style component
	-Template this style component
	-We do not need to have the applier as a template, since we are not going to use the lambda as a function
	-We are going to use the lambda as a capture and therefore do not have to have any inputs
	*/


	/*

	-Every component should be initialised in that it should know how to apply itself.
	-
	*/
	using Applier = std::function<void(void)>;
	template<typename ComponentType>
	class StyleComponent : StyleComponentBase
	{
	private:
		ComponentType component;
		Applier applier;
	public:
		StyleComponent(AbstractStyle* parent, std::function<void(void)> applier)
			:StyleComponentBase(std::move(type), parent, applier)
		{
			/*
			
			*/
		}
		inline void SetComponent(ComponentType&& inp)
		{
			component = std::forward<ComponentType>(inp);
			AlertParentOfChange();
		}
		inline const ComponentType& GetComponent() const noexcept { return component; }
	};

	struct AbstractStyle
	{
		AbstractStyle(GUIVisual* p)
		{
		}
		virtual void Alert(StyleComponentBase* component) = 0;
		virtual ~AbstractStyle(){}
	};
	
	template<typename Property>
	struct BaseStyle : public AbstractStyle
	{
		AbstractProperty* parent;
		StyleComponent<sf::Vector2f> position;
		StyleComponent<float> rotation;
		StyleComponent<sf::Vector2f> scale;
		StyleComponent<sf::Vector2f> origin;

		StyleComponent<sf::Color> fill_color;
		StyleComponent<sf::Color> outline_color;
		//StyleComponent<std::string, Property> resource_name;


		/*
		-What will a string be applied to?
		*/
		BaseStyle(Property& p)
			:position([this->position, &p](){p->setPosition(position.GetComponent()); }),
			rotation([this->rotation, &p](){p->setRotation(rotation.GetComponent()); }),
			scale([this->scale, &p](){p->setScale(scale.GetComponent()); }),
			origin([this->origin](){p->setOrigin(origin.GetComponent()); }),
			fill_color([this->fill_color](){p->setFillColor(fill_color.GetComponent()); }),
			outline_color([this->outline_color](){p->setOutlineColor(outline_color.GetComponent()); })
		{	
		}
		virtual ~BaseStyle<Property>() = 0;

	private:
		void Alert(StyleComponentBase* component) override { parent->Alert(component); }
	};

	struct TextStyle : public BaseStyle<sf::Text>
	{
		StyleComponent<uint8_t> character_size;
		StyleComponent<uint32_t> text_style;
		StyleComponent<uint32_t> line_spacing;
		StyleComponent<uint32_t> letter_spacing;
		TextStyle(sf::Text& text)
			:
			character_size([this->character_size, &text](){text.setCharacterSize(character_size); }),
			text_style([this->text_style, &text](){text.setStyle(text_style); }),
			line_spacing([this->line_spacing, &text](){ text.setLineSpacing(line_spacing); }),
			letter_spacing([this->letter_spacing, &text](){text.setLetterSpacing(letter_spacing); })
		{
		}
		virtual ~TextStyle(){}
	};
	struct BackgroundStyle : public BaseStyle<sf::RectangleShape>
	{
		StyleComponent<sf::IntRect, sf::RectangleShape> texture_rect;
		StyleComponent<sf::Vector2f, sf::RectangleShape> size;

		BackgroundStyle(sf::RectangleShape& p)
			:BaseStyle<sf::RectangleShape>(p),
			texture_rect(this, [&p, this]() {p.setTextureRect(texture_rect.GetComponent()); }),
			size(this, [&p, this]() {p.setSize(size.GetComponent()); })
		{

		}
		virtual ~BackgroundStyle()
		{
		}
	};

	class AbstractProperty
	{
	protected:
		CustomQueue<StyleComponentBase*> pending_changes;
	private:
	public:
		void Alert(StyleComponentBase* component) 
		{
		}
		virtual ~AbstractProperty() = 0;
	};

	/*
	-When a given substyle changes, it should alert the Visual Property 
	*/
	template<typename Property,typename Style>
	class VisualProperty : public AbstractProperty
	{
	private:
		Property visual_property;
		GUIData::GUIState active_state;
		
		std::array<Style, 3> state_styles;
	protected:
	public:
		VisualProperty()
		{
		}
		void SetState(GUIData::GUIState&& state)
		{
			active_state = std::move(state);
			//Reset the queue here.
			//Insert everything from the input state.
		}
		void ProcessChanges()
{

	
	};































	class GUIVisual //Not exposed to user.
	{
	protected:
		CustomQueue<Attribute_Type> pending_text_changes;
		CustomQueue<Attribute_Type> pending_background_changes;
	public:
		void ChangeState()
		{
			/*
				-Clear all queues.
				-Add every single element to each queue.
			*/
		}
		template<typename Property>
		void Alert(Attribute_Type type)
		{
			if constexpr (std::is_same_v<typename std::decay_t<Property>, sf::Text>) pending_text_changes.Insert(type);
			else if constexpr (std::is_same_v<typename std::decay_t<Property>, sf::RectangleShape>) pending_background_changes.Insert(type);
		}

		void ProcessPendingChanges()
		{
			/*
			
			
			*/
		}
	};
	class GUIVisual
	{
	private:
		std::unordered_map<Attribute_Type, AttributeApplier> applier_map;
		static StyleComponents CreateAttributes(const std::vector<Attribute_Type>& vec, BaseStyle& m_property)
		{
		}
		VisualProperty<sf::Text> text;
		VisualProperty<sf::RectangleShape> background;
	public:
		GUIVisual()
			:applier_map({ Attribute_Type::Character_Size, [](BaseStyle& v) {} })
		{
			
		text.state_components.at(0).second.
		}
	};







// 
// 
// 	using GUIStateStyles = std::array<GUIStyle, 3>;
// 	class GUIVisual 
// 	{
// 	private:
// 		GUIVisualProperty<sf::Text> text;
// 		GUIVisualProperty<sf::RectangleShape> background;
// 
// 		void ProcessQueue()
// 		void ProcessPendingChanges()
// 		{	
// 		}
// 		void CalibrateText();
// 		void ApplyText();
// 		void ApplyBackground();
// 		void ChangeState(const GUIData::GUIState& state);
// 		void Update(const float& dT)
// 		{
// 		}
// 
// 		sf::Text text;
// 		sf::RectangleShape background;
// 
// 		std::array<GUIStyle, 3> state_styles;
// 		GUIData::GUIState active_state;
// 
// 		Manager_Texture* texture_mgr;
// 		Manager_Font* font_mgr;
// 
// 		inline GUIStyle& GetStyle(const GUIData::GUIState& state) { return state_styles.at(static_cast<int>(state)); }
// 		void ApplyText()
// 		{
// 
// 			BaseStyle<sf::Text, Manager_Font, TextStyle>(GetStyle(active_state).text_style).ApplyAttributes(text, font_mgr);
// 		}
// 		void ApplyBackground()
// 		{
// 			auto x = GetStyle(active_state);
// 		}
// 		std::array<GUIStyle, 3> state_styles;
// 
// 		sf::RectangleShape background;
// 		sf::Text text;
// 
// 		sf::FloatRect element_local_AABB;
// 		bool pending_parent_redraw;
// 
// 		GUIData::GUIState active_state;
// 		GUIStyle active_style;
// 
// 		Manager_Font* font_mgr;
// 		Manager_Texture* texture_mgr;
// 
// 		inline GUIStyle& GetStyle(const GUIData::GUIState& state) { return state_styles.at(Utility::ConvertToUnderlyingType(state)); }
// 		void Update()
// 		{
// 			if (active_style.ApplyPendingAttributes<TextStyle>(text)) pending_parent_redraw = true;
// 			if(active_style.ApplyPendingAttributes<BackgroundStyle>(background)) pending_parent_redraw = true;
// 		}
// 		bool pending_state_apply;
// 
// 		void ApplySize();
// 		void ApplyText(GUIStyle& activestyle, const sf::FloatRect& eltlocalboundingbox)
// 		{
// 			sf::Vector2f textsize;
// 			sf::Vector2f localorigin;
// 			sf::Vector2f textposition;
// 			const auto& origin_proportion = std::get<sf::Vector2f>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_ORIGIN));
// 			const auto& position_proportion = std::get<sf::Vector2f>(activestyle.GetAttribute(STYLE_ATTRIBUTE::TEXT_POSITION));
// 			auto CalculateAndApply = 
// 				[&textsize, &localorigin, &textposition, &eltlocalboundingbox, &origin_proportion, &position_proportion](sf::Text& text, const unsigned int& character_size) {
// 				text.setCharacterSize(character_size);
// 				textsize = sf::Vector2f{ text.getLocalBounds().width, text.getLocalBounds().height }; //size of the bounding box.
// 				localorigin = { origin_proportion.x * textsize.x, origin_proportion.y * textsize.y }; //local origin as a proportion of the size.
// 				textposition = sf::Vector2f{ eltlocalboundingbox.left, eltlocalboundingbox.top };//plus the top left position of the element its being drawn relative to
// 				textposition += sf::Vector2f{ position_proportion.x * eltlocalboundingbox.width, position_proportion.y * eltlocalboundingbox.height };//plus the position of the actual text itself as a proportion of the parent element size
// 				textposition -= sf::Vector2f{ text.getLocalBounds().left, text.getLocalBounds().top };//minus the default padding that sfml inserts with sf::Text::getLocalBounds()
// 				text.setOrigin(localorigin);
// 				text.setPosition(textposition);
// 			};
// 			auto TextFits = [&eltlocalboundingbox, &textposition, &localorigin, &textsize]()->bool {
// 				sf::Vector2f texttopleft{ textposition - localorigin };
// 				if (texttopleft.x < eltlocalboundingbox.left) return false;
// 				if (texttopleft.x + textsize.x > eltlocalboundingbox.left + eltlocalboundingbox.width) return false;
// 				if (texttopleft.y < eltlocalboundingbox.top) return false;
// 				if (texttopleft.y + textsize.y > eltlocalboundingbox.top + eltlocalboundingbox.height) return false;
// 				return true;
// 			};
// 			std::cout << "hello world" << std::endl;
// 			auto new_char_size = static_cast<double>(text.getCharacterSize());
// 			CalculateAndApply(text, new_char_size);
// 			if (!TextFits()) {
// 				//must find the maximum charactersize, while maintaining this position, which allows us to fit in our element.
// 				while (new_char_size > 1) {
// 					CalculateAndApply(text, new_char_size);
// 					if (TextFits()) break;
// 					--new_char_size;
// 				}
// 				activestyle.attributes.at(STYLE_ATTRIBUTE::TEXT_CHARACTER_SIZE) = std::move(new_char_size);
// 				activestyle.attributes.at(STYLE_ATTRIBUTE::TEXT_CHARACTER_SIZE) = std::move(new_char_size);
// 			}
// 			activestyle.pending_text_apply = false;
// 			pending_parent_redraw = true;
// 		}
// 		void ApplyBackground(GUIStyle& activestyle);
// 		void ApplyPosition();
// 		void ReleasePrevStyleResources();
// 		void ApplyState(GUIStyle& activestyle, const sf::FloatRect& eltboundingbox);
// 	public:
// 		GUIVisual(Manager_Texture* tmgr, Manager_Font* fmgr) 
// 			:texture_mgr(tmgr),
// 			font_mgr(fmgr),
// 			pending_state_apply(true)
// 		{
// 		}
// 		void OnCreate(GUIStateStyles&& styles, GUIData::GUIState* element_state);
// 		inline CustomQueue<Attribute_Type>& GetAttributeQueue() noexcept { return pending_style_changes; }
// 		void Update();
// 		inline void SetPosition(sf::Vector2f&& position) {
// 			element_local_AABB.left = std::move(position.x);
// 			element_local_AABB.top = std::move(position.y);
// 			
// 
// 			element_local_position = position;
// 			pending_position_apply = true;
// 		}
// 		inline const sf::Vector2f& GetElementSize() const { return { element_local_AABB.width, element_local_AABB.height }; }
// 		inline const sf::Vector2f& GetElementPosition() const { return { element_local_AABB.left, element_local_AABB.top }; }
// 		inline void SetSize(const sf::Vector2f& size) {
// 			element_size = size;
// 			pending_size_apply = true;
// 		}
// 		void SetState(const GUIData::GUIState& state);
// 		void Draw(sf::RenderTarget& target, const bool& toparent);
// 
// 		inline GUIStyle& GetStyle(const GUIData::GUIState& state) {return *state_styles.at(Utility::ConvertToUnderlyingType(state));}
// 		void ChangeStyle(const GUIData::GUIState& state, GUIStyle&& style);
// 		inline const bool& PendingParentRedraw() const { return pending_parent_redraw; }
// 		inline const bool& PendingSizeApply() const { return pending_size_apply; }
// 		inline void QueueParentRedraw() { pending_parent_redraw = true; }//ENCAPSULATE
// 		inline std::vector<sf::Drawable*>& GetUserDrawables() { return drawables; }
// 	};
}
#endif


