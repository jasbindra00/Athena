#include "Map.h"
#include "FileReader.h"
#include "Log.h"
#include "EnumConverter.h"


Map::Map(Manager_Texture* mgr,sf::RenderWindow* winptr):texturemgr(mgr), renderwindow(winptr){
	LoadMap("MyMap.txt");

}
using KeyProcessing::ToUpperString;

MapTile* Map::GetMapTile(const unsigned int& row, const unsigned int& col){
	if (row >= mapdimension.y) return nullptr;
	if (col >= mapdimension.x) return nullptr;
	return &maptiles[row][col];
}

bool Map::ReadStandardTiles(const std::string& tilefile) {
	FileReader file;
	if (!file.LoadFile(tilefile)) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the standard tile file of name " + tilefile);
		return false;
	}
	file.NextLine();
	for (int i = 0; i < 2; ++i) {
		auto strdimensions = KeyProcessing::ExtractAttributesToStream(file.GetWord());
		auto attrtype = ToUpperString(strdimensions.GetWord());
		if (attrtype == "TILEDIMENSIONX") strdimensions >> tiledimension.x;
		else if (attrtype == "TILEDIMENSIONY") strdimensions >> tiledimension.y;
	}
	if (tiledimension.x <=0 || tiledimension.y <=0 ) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Invalid tile dimensions in tile file of name " + tilefile); return false; }
	while (!file.EndOfFile()) {
		file.NextLine();
		if (file.GetWord() != "TILE") { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify tile declaration on line number " + file.GetLineNumberString() + " in tile of filename " + tilefile); continue; }
		char tileid; {
			auto str = file.GetWord();
			if (str.size() > 1) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to read TILEID on line number " + file.GetLineNumberString() + " in tile file of name " + tilefile + ". ID must be a single character"); continue; }
			tileid = str[0];
			auto tileidexists = standardtiles.find(tileid);
			if (tileidexists != standardtiles.end()) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Tile with ID " + str + " from tile file of name + " + tilefile + " already exists"); continue; }
		}
		auto standardtile = std::make_unique<StandardTile>();
		while (!file.GetLineStream().eof()) {
			auto attributes = KeyProcessing::ExtractAttributesToStream(file.GetWord());
			auto attributetype = attributes.GetWord();
			if (attributetype == std::string{ "FRICTIONX" }) attributes >> standardtile->friction.x;
			else if (attributetype == std::string{ "FRICTIONY" }) attributes >> standardtile->friction.y;
			else if (attributetype == std::string{ "TOPLEFTX" }) attributes >> standardtile->texturerect.top >> standardtile->texturerect.left;
			else if (attributetype == std::string{ "TOPLEFTY" }) attributes >> standardtile->texturerect.top >> standardtile->texturerect.top;
			else if (attributetype == std::string{ "TEXTURESIZEX" }) attributes >> standardtile->texturerect.width;
			else if (attributetype == std::string{ "TEXTURESIZEY" })attributes >> standardtile->texturerect.height;
			else if (attributetype == std::string{ "SPRITESHEET" }) attributes >> standardtile->atlasmapname;
			
			else LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify attribute type of name " + attributetype + " in tile file of name " + tilefile);
		}
		if (standardtile->texturerect.width <= 0 || standardtile->texturerect.height <= 0) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Invalid tile texture rect dimensions in tile file of name " + tilefile);
		else{ standardtile->texture = texturemgr->RequestResource(standardtile->atlasmapname);
			if (standardtile->texture != nullptr) {
				standardtile->tilesprite.setTexture(*standardtile->texture);
				standardtile->tilesprite.setTextureRect(standardtile->texturerect);
				standardtile->tilesprite.setScale(tiledimension.x / standardtile->texturerect.width, tiledimension.y / standardtile->texturerect.height);
				standardtiles[tileid] = std::move(standardtile);
				continue;
			}
		}
		standardtile.reset();
	}
	return true;
	}

void Map::LoadMap(const std::string& mapfile) {
	class myc;

	FileReader file;
	if (!file.LoadFile(mapfile)) {LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open map file of name " + mapfile);  return;}
	std::string extension{ " in map file of name " + mapfile };
	auto linestream = static_cast<Attributes*>(&file.GetLineStream());
	//read map attributes here.
	file.NextLine();
	while (!linestream->eof()) {
		auto keystream = KeyProcessing::ExtractAttributesToStream(file.GetWord());
		std::string attributetype = keystream.GetWord();
		if (attributetype == "MAPNAME") keystream >> mapname;
		else if (attributetype == "MAPTILEDIMENSIONX") keystream >> mapdimension.x;
		else if (attributetype == "MAPTILEDIMENSIONY") keystream >> mapdimension.y;
		else if (attributetype == "MAXLAYERS") keystream >> maxlayers;
		else if (attributetype == "STANDARDTILEFILE") keystream >> tilefile;
		else {
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify map attribute type of name " + attributetype + extension); return;
		}
	}
	if (!ReadStandardTiles(tilefile)) return;
	while (!file.EndOfFile()) {
		file.NextLine();
		if (file.ReturnLine().empty()) continue;
		auto keystream = KeyProcessing::ExtractAttributesToStream(linestream->GetWord());
		std::string configuration;
		if (keystream.GetWord() != "START_CONFIGURATION") {
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify START_CONFIGURATION in map file of name " + mapfile + " on line " + file.GetLineNumberString());
			break;
		}
		auto configurationame = keystream.GetWord();
		int configrownum{ 0 };
		while (configrownum < mapdimension.y) {
			file.NextLine();
			if (file.ReturnLine() == KeyProcessing::ConstructKey("END_CONFIGURATION", configurationame)) {
				//NOT ENOUGH ROWS.
				LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Found END_CONFIGURATION before " + std::to_string(mapdimension.y) + " rows were read. Defaulting the rest of the configuration...");
				while (configrownum < mapdimension.y) {
					configuration += std::string(mapdimension.x, '0') + " ";
					++configrownum;
				}
				ReadConfiguration(std::move(configurationame), std::move(configuration));
				break;
			}
			configuration += file.ReturnLine() + " ";
		}
		if (file.GetWord() != KeyProcessing::ConstructKey("END_CONFIGURATION", configurationame)) {
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Could not find END_CONFIGURATION after reading the full " + std::to_string(mapdimension.y) + " rows. Truncating the configuration...");
			ReadConfiguration(std::move(configurationame), std::move(configuration));
			file.NextLine();
			while (file.GetWord() != KeyProcessing::ConstructKey("END_CONFIGURATION", configurationame) && !file.EndOfFile()) {
				file.NextLine();
			}
			if (file.EndOfFile()) {
				LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Could not find END_CONFIGURATION for configuration of name " + configurationame + ". Map registry incomplete - exiting method...");
				return;
			}
		}
		else ReadConfiguration(std::move(configurationame), std::move(configuration));
	}
}
void Map::ReadConfiguration(const std::string& configurationtype,  const std::string& configuration){
	enum class CONFIGTYPE {
		GAME_MAP = 0, LAYER_MAP, WARP_MAP, DEADLY_MAP, NULLTYPE
	};
	CONFIGTYPE type; {
		EnumConverter<CONFIGTYPE> converter([](const std::string& type) {
			if (type == "GAME_MAP") return CONFIGTYPE::GAME_MAP;
			else if (type == "LAYER_MAP") return CONFIGTYPE::LAYER_MAP;
			else if (type == "WARP_MAP") return CONFIGTYPE::WARP_MAP;
			else if (type == "DEADLY_MAP") return CONFIGTYPE::DEADLY_MAP;
			return CONFIGTYPE::NULLTYPE;
			});
		type = converter(configurationtype);
	}
	if (type == CONFIGTYPE::NULLTYPE) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify the configuration map type of name " + configurationtype);
		return;
	}
	auto ConvertToIndexNotation = [](const char& c1, const char& c2)->std::string {
		return std::string{ "[" + std::to_string(c1) + "]" + "[" + std::to_string(c2) + "]" };
	};
	int configurationrow{ 0 };
	std::string fixedconfiguration;
	Attributes stream(configuration);
	auto row = stream.GetWord();
	while (!stream.eof()) {
		if (row.size() > mapdimension.x) row.substr(0, mapdimension.x); //truncate row
		else if (row.size() < mapdimension.x)  row.append(std::string(mapdimension.x - row.size(), '0'));
		fixedconfiguration.append(std::move(row)) + " ";
		++configurationrow;
		row = stream.GetWord();
	}
	configurationrow = 0;
	Attributes configurationstream(fixedconfiguration);
	while (!configurationstream.eof()) {
		s
	}
	










	if (configurationtype == "GAME_MAP") {
		while (!configurationstream.eof()) {
			auto configurationrow = configurationstream.GetWord();
			maptiles.push_back(std::vector<MapTile>{});
			for (int i = 0; i < configurationrow.size(); ++i) {
				MapTile tile;
				auto standardtileexists = standardtiles.find(configurationrow[i]);
				if (standardtileexists == standardtiles.end()) {
					tile.standardtile = GetErrorTile();
					LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to find tile of TILEID char of " + std::to_string(configurationrow[i]) + "at index " + ConvertToIndexNotation(configrow, i));
				}
				else tile.standardtile = standardtileexists->second.get();
				tile.position = sf::Vector2f{ static_cast<float>(i * tiledimension.x), static_cast<float>(configrow * tiledimension.y) };
				maptiles.back().push_back(std::move(tile));
			}
		}
	}
	else if (configurationtype == "LAYER_MAP") {
		while (!configurationstream.eof()) {
			auto configurationrow = configurationstream.GetWord();
			for (int i = 0; i < configurationrow.size(); ++i) {
				auto tile = GetMapTile(configrow, i);
				if (!KeyProcessing::IsNumber(configurationrow[i])) {
					LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify the layer number in LAYER_MAP configuration at index " + ConvertToIndexNotation(configrow, i) + ". Defaulting to 0...");
					tile->layer = 0;
				}
				auto layernum = std::stoi(std::string{ configurationrow[i] });
				if (layernum > maxlayers) {
					LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Layer number in LAYER_MAP configuration at index " + ConvertToIndexNotation(configrow, i) + " exceeds specified maximum layer capacity of " + std::to_string(maxlayers) + ". Defaulting to 0...");
					layernum = 0;
				}
				tile->layer = layernum;
			}
		}
	}
	}




MapTile* Map::GetTile(const float& x, const float& y, const unsigned int& elevation) const
{
	return nullptr;
}

sf::Vector2i Map::GetTileSize() const
{
	return {};

}

void Map::Update(const float& dT)
{

}

void Map::Draw(){
// 	auto& x = standardtiles.at('2');
// 	x->tilesprite.setPosition(sf::Vector2f{ 200,200 });
// 	renderwindow->draw(x->tilesprite);
}
