#include "Map.h"
#include "EnumConverter.h"
#include "StreamAttributes.h"
#include "MapTile.h"

using namespace MapData;
using namespace MapData::ConfigurationData;

void Map::ReadConfiguration(const ConfigurationType& type, const std::string& configurationblock) {
	ConfigurationReader reader = configurationreaders[type];
	Attributes configurationstream(configurationblock);
	std::string errorstring;
	std::string alteredstring;
	int rownumber{ -1 };
	while (!configurationstream.eof()) {
		++rownumber;
		std::string configurationrow(configurationstream.GetWord());
		if (configurationrow.empty()) {
			std::cout<<std::endl;
		}
		auto changerecord = FixConfigurationRow(type, configurationrow);
		if (!changerecord.empty()) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Map configuration row number was changed in the following ways : " + changerecord);
		{//ensuring that all the rows match the specified map dimension.
			if (configurationrow.size() != mapdimension.x) {
				std::string altered;
				if (configurationrow.size() < mapdimension.x) {
					std::string fill(mapdimension.x - configurationrow.size(), '0');
					configurationrow.append(std::move(fill));
				}
				else {
					configurationrow = configurationrow.substr(0, mapdimension.x);
					alteredstring.append(std::to_string(rownumber) + " "); //log.
				}
			}
			try { reader(std::move(configurationrow), rownumber); }
			catch (const CustomException& exception) {
				auto rowerror = static_cast<std::string>(exception.what());
				errorstring.append(rowerror);
			}
		}
	}
	if (!alteredstring.empty()) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Map CONFIGURATION ERROR : The following configuration indices were truncated/extended due to a mismatch between the size and the specified map X dimension : " + alteredstring);
	else if (!errorstring.empty()) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, std::move(errorstring));
}
bool Map::ReadStaticTiles(const std::string& tilefile){
	using KeyProcessing::ToUpperString;
	using KeyProcessing::ExtractAttributesToStream;
	using namespace TileData;
	FileReader file;
	if (!file.LoadFile(tilefile)) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the standard tile file of name " + tilefile);
		return false;
	}
	std::string DidNotReadTile{ " DID NOT READ TILE..." };
	std::string TileFileStr{ " in tile file of name " + tilefile };
	file.NextLine();
	for (int i = 0; i < 2; ++i) { //we must read the tile pixel dimensions before anything else.
		auto dimensionstream = KeyProcessing::ExtractAttributesToStream(file.GetWord());
		auto attrtype = ToUpperString(dimensionstream.GetWord());
		if (attrtype == "TILEDIMENSIONX") dimensionstream >> tiledimension.x;
		else if (attrtype == "TILEDIMENSIONY") dimensionstream >> tiledimension.y;
	}
	if (tiledimension.x <= 0 || tiledimension.y <= 0) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Invalid tile dimensions in tile file of name " + tilefile); return false; }
	auto tileattributes = static_cast<Attributes*>(&file.GetLineStream());
	std::string line;
	while (file.NextLine().GetFileStream()) {
		auto statictileobj = std::make_unique<StaticTile>(texturemgr);
		char tileid = '0';
		Attributes tileidstream(ExtractAttributesToStream(tileattributes->GetWord()));
		try {//checking plausiblity of the TILEID.
			if (ToUpperString(tileidstream.GetWord()) != "TILEID") throw CustomException{ "Unable to find the TILEID for standard tile on line " + file.GetLineNumberString() + "." };
			else {
				std::string tileidstr = tileidstream.GetWord();
				if (tileidstr.size() > 1) throw CustomException{ "The char typedef for a static tile should be a single character on " + file.GetLineNumberString() + "." };
				else if (tileidstr[0] == '0') throw CustomException{ "Invalid tile char typedef - '0' is a reserved char." };
				else if (statictiles.find(tileidstr[0]) != statictiles.end()) throw CustomException{ "A static tile with TILEID '" + tileidstr + "' already exists." };
				tileid = tileidstr[0];
			}
		}
		catch (const CustomException& exception) { //any problems with the tile id will skip the tile completely
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, exception.what() + TileFileStr + DidNotReadTile);
			statictileobj.reset();
			continue;
		}
		try { *tileattributes >> statictileobj.get(); } //read the remainder of the attributes into the object.
		catch (const CustomException& exception) {
			if (!std::string{ exception.what() }.empty()) {
				Attributes errorstream(exception.what());
				while (!errorstream.eof()) {
					std::string errortype = errorstream.GetWord();
					if (errortype == "SPRITESHEET") LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify spritesheet for tile of ID " + std::string{ tileid } + TileFileStr + ". ERROR TEXTURE SET...");
					else if (errortype == "TEXTURERECT") {
						LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Tile of ID " + std::string{ tileid } + TileFileStr + " has invalid texture dimensions." + DidNotReadTile);
						statictileobj->GetTextureRect().width = 32; // set to error texture dimensions
						statictileobj->GetTextureRect().height = 32;
					}
				}
			}
		}
		//scale the sprite texture to match the specified tile pixel dimensions.
		sf::Sprite& tilesprite = statictileobj->GetTileSprite();
		tilesprite.setTextureRect(statictileobj->GetTextureRect());
		tilesprite.setScale(tiledimension.x / tilesprite.getTextureRect().width, tiledimension.y / tilesprite.getTextureRect().height); //scale the texture down to tile pixel dimension
		statictiles[tileid] = std::move(statictileobj);
	}
	return true;
}
void Map::LoadMap(const std::string& mapfile) {
	std::string errorstart = "MAP NAME : " + mapfile + ". ";
	using KeyProcessing::ToUpperString;
	FileReader file;
	if (!file.LoadFile(mapfile)) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the mapfile of name " + mapfile);
		return;
	}
	file.NextLine();
	try {bool tilefile = false, mapdimensionx = false, mapdimensiony = false;
			Attributes linestream(file.ReturnLine());
			auto x = file.ReturnLine();
			while (!linestream.eof()) {
				Attributes keystream(KeyProcessing::ExtractAttributesToStream(linestream.GetWord()));
				std::string keytype = keystream.GetWord();
				if (keytype == "TILEFILE") {
					if (ReadStaticTiles(keystream.PeekWord())) tilefile = true;
					else throw CustomException("Unable to open the tilefile of name " + keystream.GetWord());
				}
				else if (keytype == "MAPDIMENSIONX" || keytype == "MAPDIMENSIONY") {
					std::string dimensionstr = keystream.GetWord();
					if (KeyProcessing::IsOnlyNumeric(keystream.PeekWord())) {
						if (std::stoi(dimensionstr) > 0) {
							if (keytype == "MAPDIMENSIONX") { mapdimension.x = std::stoi(dimensionstr); mapdimensionx = true; }
							else if (keytype == "MAPDIMENSIONY") { mapdimension.y = std::stoi(dimensionstr); mapdimensiony = true; }
						}
						else throw CustomException("Map dimensions are not positive.");
					}
					else throw CustomException("Map dimensions are not integers.");
				}
			}
			if (!(tilefile && mapdimensionx && mapdimensiony)) throw CustomException("All three {TILEFILE,file} {MAPDIMENSIONX,x} {MAPDIMENSIONY,y} keys must be present on the first line before map read can initiate.");
			file.NextLine();
			if (file.ReturnLine() != "{START_CONFIGURATION,GAME_MAP}") throw CustomException("Ensure that the GAME_MAP configuration is the first argument configuration.");
		}
		catch (const CustomException& exception) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, errorstart + std::string{ exception.what() } + ". Exiting map file read..."); return; }	
		auto& filestream = file.GetFileStream();
		while (!file.EndOfFile()) {
			Attributes linestream(KeyProcessing::ExtractAttributesToStream(file.ReturnLine()));
			if (linestream.GetWord() == "START_CONFIGURATION") {
				int configstartpos = filestream.tellg();
				std::string configstringtype = linestream.GetWord();
				ConfigurationType configtype = configurationconverter(configstringtype);
				if (configtype != ConfigurationType::NULLTYPE) {
					std::string configurationblock((mapdimension.x * mapdimension.y) + mapdimension.y, '0');
					filestream.readsome(configurationblock.data(), std::streamsize(mapdimension.x * mapdimension.y) + mapdimension.y);

					linestream = KeyProcessing::ExtractAttributesToStream(file.NextLine().ReturnLine());
					if (linestream.GetWord() != "END_CONFIGURATION") {
						LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to find the {END_CONFIGURATION,x} key for configuration of type " + configstringtype);
						filestream.clear();
						filestream.seekg(configstartpos);
						file.SeekToLineKey("{START_CONFIGURATION,x}", false, true, false); //go to the next start configuration
					}
					ReadConfiguration(configtype, configurationblock);
				}
			}
			file.NextLine();
		}
		
	}
std::string Map::FixConfigurationRow(const ConfigurationType& type, std::string& configurationrow)
{
	std::string recordstring;
	if (configurationrow.size() != mapdimension.x) {
		if (configurationrow.size() > mapdimension.x) { configurationrow = configurationrow.substr(0, mapdimension.x); recordstring += "TRUNCATED "; }
		else if (configurationrow.size() < mapdimension.x) { configurationrow += std::string(mapdimension.x - configurationrow.size(), '0');  recordstring += "EXPANDED "; }
	}
	bool altered = false;
	if (type == ConfigurationType::GAME) {
		for (auto& c : configurationrow) {
			bool exists = false;
			for (auto& statictile : statictiles) {
				if (c == statictile.first) {
					exists = true;
					break;
				}
			}
			if (!exists) {
				c = '0';
				altered = true;
			}
		}
	}
	else if (type == ConfigurationType::LAYER) {
		for (auto& c : configurationrow) {
			if (c < '0' || c > '3') c = '0';
			altered = true; 
		}
	}
	else if (type == ConfigurationType::DEADLY || type == ConfigurationType::TELEPORT) {
		for (auto& c : configurationrow) {
			if (c != '0' || c != '1') { c = '0'; altered = true; }
		}
	}
	if (altered) recordstring += "ADJUSTED ";
	return recordstring;
}

Map::Map(Manager_Texture* mgr, sf::RenderWindow* render) :texturemgr(mgr),window(render){
	statictiles['0'] = std::make_unique<TileData::StaticTile>(texturemgr);
	configurationreaders[ConfigurationType::GAME] = [this](const std::string& configurationrow, const int& rownum = 0)->bool {
		std::string errorstring;
		maptiles.push_back(std::vector<MapTile>());
		for (int i = 0; i < configurationrow.size(); ++i) {
			MapTile tile;
			const auto& tileid = configurationrow[i];
			auto statictile = statictiles.find(tileid);
			if (statictile != statictiles.end()) tile.statictileid = tileid;
			else { tile.statictileid = '0';	errorstring += std::string{ "[" + std::to_string(rownum) + "][" + std::to_string(i) + "] " }; }
			maptiles.back().push_back(std::move(tile));
		}
		if(!errorstring.empty()) throw CustomException("Map GAME_CONFIGURATION ERROR : The TILE ID in following configuration indices were not registered : " + std::move(errorstring));
	};
	configurationreaders[ConfigurationType::LAYER] = [this](const std::string& configurationrow, const int& rownum )->bool {
		std::string errorstring;
			for (int i = 0; i < configurationrow.size(); ++i) {
				const auto& layernum = configurationrow[i];
				auto& tile = maptiles[rownum][i];
				if (layernum >= '0' && layernum <= '3') { tile.layer = layernum; continue; }
				tile.layer = '0';
				errorstring += std::to_string(i) + " ";
			}
			if(!errorstring.empty()) throw CustomException("LAYER_MAP has a minimum layer arg of 0 and a  maximum layer arg of 3. The following tiles in the layer configuration were defaulted to 0 : " + std::move(errorstring));
	};
	LoadMap("MyMap.txt");
}

void Map::Draw(){
	int y = 0;
	for (auto& row : maptiles)
	{
		int x = 0;
		for (auto& col : row)
		{
			auto& statictile = statictiles.at(col.statictileid);
			statictile->GetTileSprite().setPosition(sf::Vector2f{ static_cast<float>(x * 32), static_cast<float>(y * 32) });
			window->draw(statictile->GetTileSprite());
			++x;
			
		}
		++y;
	}
}

