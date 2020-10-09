#include "GUIFormattingData.h"
#include "Utility.h"
using namespace GUIFormattingData;

using Utility::ConvertToUnderlyingType;
namespace GUIFormattingData {

}

GUIFormattingData::GUIStyle::GUIStyle()
{
	sf::Color defaultc(255, 255, 255, 255);
	attributes[STYLE_ATTRIBUTE::BG_FILL_COLOR] = defaultc;
	attributes[STYLE_ATTRIBUTE::BG_OUTLINE_COLOR] = defaultc;
	attributes[STYLE_ATTRIBUTE::TEXT_FILL_COLOR] = std::move(defaultc);
	attributes[STYLE_ATTRIBUTE::BG_TEXTURE_NAME] = std::string{ "" };
	attributes[STYLE_ATTRIBUTE::TEXT_FONT_NAME] = std::string{};
	attributes[STYLE_ATTRIBUTE::BG_TEXTURE_RECT] = sf::IntRect{ 0,0,0,0 };
	attributes[STYLE_ATTRIBUTE::TEXT_STRING] = std::string{};
	attributes[STYLE_ATTRIBUTE::BG_OUTLINE_THICKNESS] = 1.0;
	attributes[STYLE_ATTRIBUTE::TEXT_CHARACTER_SIZE] = 30.0;
	attributes[STYLE_ATTRIBUTE::TEXT_POSITION] = sf::Vector2f{ 0.5,0.5 };
	attributes[STYLE_ATTRIBUTE::TEXT_ORIGIN] = sf::Vector2f{ 0.5,0.5 };
	attributes[STYLE_ATTRIBUTE::TEXT_HIDDEN] = false;
}

STYLE_ATTRIBUTE GUIFormattingData::GUIStyle::ReadIn(const STYLE_ATTRIBUTE& attribute, const AttributeVariant& val)
{
	/*
	-Responsible for : Reading in a STYLE_ATTRIBUTE of a user value.
	*/
	//Check if the input attribute enum is listed.
	auto attrstr = static_cast<std::string>(magic_enum::enum_name(attribute));
		//Unlisted type.
	if (attrstr.empty()) return STYLE_ATTRIBUTE::NULLTYPE;
	//Check if the input and required types match by comparing the indicies.
	auto& attribute_variant = attributes.at(attribute);
	if (val.index() != attribute_variant.index()) return STYLE_ATTRIBUTE::NULLTYPE;
	attribute_variant = val;
	//A change has been applied; we need to log this.
	(GetPropertyType(attribute) == PROPERTY_TYPE::BG) ? pending_bg_apply = true : pending_text_apply = true;
	return attribute;
}
const GUIFormattingData::PROPERTY_TYPE GUIFormattingData::GUIStyle::GetPropertyType(const STYLE_ATTRIBUTE& type) //RESPONSIBLE FOR : Checking the property that an input STYLE_ATTRIBUTE enumerate belongs to
{
	auto ind = magic_enum::enum_index(type);
	if (ind >= BG_RANGE.first && ind <= BG_RANGE.second) return PROPERTY_TYPE::BG;
	else if (ind >= TEXT_RANGE.first && ind <= TEXT_RANGE.second) return PROPERTY_TYPE::TEXT;
	return PROPERTY_TYPE::NULLTYPE;
}

const bool GUIStyle::PollChange(STYLE_ATTRIBUTE& attr){
	return pending_changes.Poll(attr);
}

const STYLE_ATTRIBUTE GUIFormattingData::GUIStyle::ReadIn(const KeyProcessing::Keys& keys)
{
	//Find the STYLE_ATTRIBUTE key, eg {STYLE_ATTRIBUTE,TEXT_POSITION}
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
	auto SetResult = [&result, &successful](AttributeVariant&& arg)->void {
		result = std::move(arg);
		successful = true;
	};
	if (type == STYLE_ATTRIBUTE::BG_FILL_COLOR || type == STYLE_ATTRIBUTE::BG_OUTLINE_COLOR || type == STYLE_ATTRIBUTE::TEXT_FILL_COLOR) {
		std::pair<bool, sf::Color> res = Utility::Conversions::ProcessColor(keys, std::get<sf::Color>(attributes.at(type)));
		if (res.first) SetResult(std::move(res.second));
	}
	else if (type == STYLE_ATTRIBUTE::TEXT_POSITION || type == STYLE_ATTRIBUTE::TEXT_ORIGIN) {
	
		sf::Vector2f defaultvec = std::get<sf::Vector2f>(GetAttribute(type));
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
		sf::IntRect defaultintrect = std::get<sf::IntRect>(GetAttribute(type));
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
		attributes.at(Utility::ConvertToUnderlyingType(type)) = std::move(result);
		pending_changes.Insert(type);
		(GetPropertyType(type) == PROPERTY_TYPE::BG) ? pending_bg_apply = true : pending_text_apply = true;
		return type;
	}
	return STYLE_ATTRIBUTE::NULLTYPE;
}

void GUIVisual::ApplySize()
{
	solid_background.setSize(element_size);
	texture_background.setSize(element_size);
	pending_size_apply = false;
	pending_parent_redraw = true;
}

void GUIFormattingData::GUIVisual::ApplyBackground(GUIStyle& activestyle)
{
	if (activestyle.pending_bg_apply) {
		int x = 3;
	}
	solid_background.setFillColor(std::get<sf::Color>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_FILL_COLOR)));
	solid_background.setOutlineColor(std::get<sf::Color>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_OUTLINE_COLOR)));
	solid_background.setOutlineThickness(std::get<double>(activestyle.GetAttribute(STYLE_ATTRIBUTE::BG_OUTLINE_THICKNESS)));
	texture_background.setTextureRect(std::get<sf::IntRect>(activestyle.attributes.at(STYLE_ATTRIBUTE::BG_TEXTURE_RECT)));
	texture_background.setTexture(RequestVisualResource<sf::Texture>());
	activestyle.pending_bg_apply = false;
	pending_parent_redraw = true;
}

void GUIFormattingData::GUIVisual::ApplyState(GUIStyle& activestyle, const sf::FloatRect& eltboundingbox)
{
	ReleasePrevStyleResources();
	ApplyBackground(activestyle);
	ApplyText(activestyle, eltboundingbox);
	pending_state_apply = false;
	pending_parent_redraw = true;
}

const GUIFormattingData::GUIStateStyles& GUIFormattingData::GUIVisual::GUIVisual()
{

}

void GUIFormattingData::GUIVisual::Update()
{
	auto& active_style = state_styles.at(*active_state);
	if (pending_state_apply) ApplyState(activestyle, eltrect);
	if (activestyle.PendingBGApply()) ApplyBackground(activestyle);
	if (activestyle.PendingTextApply()) ApplyText(activestyle, eltrect);
	if (pending_size_apply) ApplySize();
	if (pending_position_apply) ApplyPosition();
	//apply individual, non state changes made by the user.
	
}
]/*
 -What we ideally need to do is be able to allow the user call a callable of their choice.
 -Then upon the draw function execution, we call this callable, with any custom draw targets that the input
 lambda has not absorbed.
 -The concept of a custom drawable vector is absolutely useless; we are unable to specify any future positions, which
 would negate the use of culling.
 */
void GUIFormattingData::GUIVisual::Draw(sf::RenderTarget& target, const bool& toparent)
{
	target.draw(solid_background);
	if (texture.get()) target.draw(texture_background);
	if (!std::get<bool>(state_styles.at(static_cast<int>(active_state)).GetAttribute(STYLE_ATTRIBUTE::TEXT_HIDDEN))) target.draw(text);
	for (auto& drawable : drawables) {
		target.draw(*drawable);
	}
	if (toparent) pending_parent_redraw = false;
}

/*
-If we change the style of the active state, then we must reflect the change instantly through the request of a visual redraw.

*/
void GUIFormattingData::GUIVisual::ChangeStyle(const GUIData::GUIState& state, GUIStyle&& style)
{
	GetStyle(state) = std::move(style);
	if (*active_state == state) SetState(state);
}
void GUIFormattingData::GUIVisual::SetState(const GUIData::GUIState& state)
{
	previous_state = active_state;
	active_state = state;
	pending_state_apply = true;
}
void GUIFormattingData::GUIVisual::OnCreate(GUIStateStyles&& styles, GUIData::GUIState* element_state)
{
	state_styles = std::move(styles);
	active_state = std::move(element_state);
	previous_state = *active_state;
}
void GUIFormattingData::GUIVisual::RequestVisualResource(const STYLE_ATTRIBUTE& attribute)
{
	

}


/*
-We should be able to release the style resources of a given text or background individually.

*/
void GUIVisual::ReleasePrevStyleResources()
{
	auto& style = GetStyle(previous_state);
	auto& bgresname = std::get<std::string>(style.GetAttribute(STYLE_ATTRIBUTE::BG_TEXTURE_NAME));
	if (!bgresname.empty()) {
		texture.reset();
		if (texturemgr) texturemgr->RequestResourceDeallocation(bgresname);
	}
	auto& fontresname = std::get<std::string>(style.GetAttribute(STYLE_ATTRIBUTE::TEXT_FONT_NAME));
	if (!fontresname.empty()) {
		font.reset();
		if (fontmgr) fontmgr->RequestResourceDeallocation(fontresname);
	}
}
void GUIVisual::ApplyPosition()
{
	solid_background.setPosition(element_local_position);
	texture_background.setPosition(element_local_position);
	text.setPosition(element_local_position);
	pending_position_apply = false;
	pending_parent_redraw = true;
}

