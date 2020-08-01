#include "Map.h"
#include "EnumConverter.h"
#include "StreamAttributes.h"
#include "MapTile.h"


void Map::ReadConfiguration(const std::string& configurationtype, const std::string& configuration) {
	using namespace TileData;
	auto ConstructIndex = [](const int& y, const int& x)->std::string {return "[" + std::to_string(y) + "][" + std::to_string(x) + "] "; };
	Attributes configurationstream(configuration);
	int configrownumber{ -1 };
	//error strings.
	std::string altered;
	std::string invalidtileid;
	std::string invalidlayer;
	std::string invalidteleportdeadly;
	while (!configurationstream.eof()) {
		++configrownumber;
		std::string configrow = configurationstream.GetWord();
		if (configrow.size() != mapdimension.x) {
			std::string altered;
			if (configrow.size() < mapdimension.x) {
				std::string fill(mapdimension.x - configrow.size(), '0');
				configrow.append(std::move(fill));
			}
			else {
				configrow = configrow.substr(0, mapdimension.x);
				altered.append(ConstructIndex(configrownumber, 0));
			}
			for (int k = 0; k < configrow.size(); ++k) {
				auto& val = configrow[k];
				using KeyProcessing::ToUpperString;
				if (ToUpperString(configurationtype) == "GAME_MAP") {
					MapTile tile;
					auto foundstatic = statictiles.find(val);
					if (foundstatic != statictiles.end()) {
						tile.statictileid = val;
						maptiles[configrownumber - 1].push_back(std::move(tile));
						continue;
					}
					tile.statictileid = '0';
					maptiles[configrownumber - 1].push_back(std::move(tile));
					invalidtileid.append(ConstructIndex(configrownumber, k));
				}
				else {
					auto& foundtile = maptiles[configrownumber][k];
					if (ToUpperString(configurationtype) == "LAYER_MAP") {
						auto& foundtile = maptiles[configrownumber][k];
						if (val > '0' && val < '3') {
							foundtile.layer = val;
							continue;
						}
						invalidlayer.append(ConstructIndex(configrownumber, k));
					}
					else if (val == '0' || val == '1') {
						if (ToUpperString(configurationtype) == "TELEPORT_MAP") foundtile.teleport = val;
						else if (ToUpperString(configurationtype) == "DEADLY_MAP")foundtile.deadly = val;
						continue;
					}
					invalidteleportdeadly.append(ConstructIndex(configrownumber, k));
				}
			}
		}

		if (!invalidlayer.empty()) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "LAYER_MAP has a minimum layer arg of 0 and a  maximum layer arg of 3. The following tiles in the layer configuration were defaulted to 0 : " + invalidlayer);
		if (!invalidteleportdeadly.empty()) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "TELEPORT_MAP / DEADLY_MAP args must be binary to represent true or false. The following tiles in the teleport and or deadly configuration were defaulted to 0 : " + invalidteleportdeadly);
		if (!invalidtileid.empty()) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "GAME_MAP configuration had unregistered TILEID's. Ensure that you register the tile before creating a GAME_MAP configuration. The following tiles were defaulted to error tiles : " + invalidtileid);

	}
}

bool Map::ReadStandardTiles(const std::string& tilefile){
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
	file.NextLine();
	auto tileattributes = static_cast<Attributes*>(&file.GetLineStream());
	while (!file.EndOfFile()) {
		file.NextLine();
		auto statictileobj = std::make_unique<StaticTile>(texturemgr);
		char tileid = '0';
		Attributes tileidstream(ExtractAttributesToStream(tileattributes->GetWord()));
		try {//checking plausiblity of the TILEID.
			if (ToUpperString(tileidstream.GetWord()) != "TILEID") throw CustomException{ "Unable to find the TILEID for standard tile on line " + file.GetLineNumberString() + "." };
			else {
				std::string tileidstr = tileidstream.GetWord();
				if (tileidstr.size() > 1) throw CustomException{ "The char typedef for a static tile should be a single character on " + file.GetLineNumberString() + "." };
				else if (tileidstr[0] == '0') throw CustomException{ "Invalid tile char typedef - '0' is a reserved char." };
				else if (statictiles.find(tileidstr[0]) != statictiles.end()) {
					throw CustomException{ "A static tile with TILEID '" + tileidstr + "' already exists." };
				}
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

void Map::LoadMap(const std::string& mapfile){
	using KeyProcessing::ToUpperString;
	std::string mapstr = " in map file of name " + mapfile;
	FileReader file;
	if (!file.LoadFile(mapfile)) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open map file of name " + mapfile);
		return;
	}
	file.NextLine();
	try {
		Attributes keystream = KeyProcessing::ExtractAttributesToStream(file.GetWord()); //
		if (ToUpperString(keystream.GetWord()) != "TILEFILE") throw CustomException("Unable to read the TILEFILE map attribute " + mapstr);
		if (!ReadStandardTiles(keystream.PeekWord())) throw CustomException("Unable to read the tile file of name " + keystream.GetWord() + mapstr);
		for (int i = 0; i < 2; ++i) {//read map dimensions
			keystream = KeyProcessing::ExtractAttributesToStream(file.GetWord());
			std::string attributetype = keystream.GetWord();
			if (attributetype == "MAPDIMENSIONX") keystream >> mapdimension.x;
			else if (attributetype == "MAPDIMENSIONY") keystream >> mapdimension.y;
		}
		if (mapdimension.x <= 0 || mapdimension.y <= 0) throw CustomException("Invalid map dimensions " + mapstr + ". Ensure that map dimensions are positive integers. Exiting map read...");
	}
	catch (const CustomException& exception) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, std::string{ exception.what() } + ". Exiting map read...");
		return;
	}
	for (int i = 0; i < mapdimension.y; ++i) {
		maptiles.push_back(std::vector<TileData::MapTile>());
	}
	file.NextLine();
	auto& filestream = file.GetFileStream();
	while (!file.EndOfFile()) {
		Attributes keystream = KeyProcessing::ExtractAttributesToStream(file.GetWord());
		if (keystream.GetWord() != "START_CONFIGURATION") { //we should be at the start of a config.
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to find the start of the current configuration " + mapstr + " - skipping configuration...");
			if (!file.SeekToLineKey("{START_CONFIGURATION,x}", false, true, false)) { //no other start configuration exists
				LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to find any subsequent configurations " + mapstr + ". Exiting map read...");
				return;
			}
			keystream = KeyProcessing::ExtractAttributesToStream(file.GetWord());
			keystream.GetWord(); //seekg to next word
		}
		int configstart = filestream.tellg(); //incase we need to backtrack because of user error.
		std::string configurationtype = keystream.GetWord();
		std::string configurationblock((mapdimension.x * mapdimension.y) + mapdimension.y, '0');
		filestream.readsome(configurationblock.data(), std::streamsize(mapdimension.x * mapdimension.y) + mapdimension.y); //read the chunk of data that which SHOULD be the configuration
		auto y = file.NextLine();
		try {if (file.ReturnLine() != KeyProcessing::ConstructKey("END_CONFIGURATION", configurationtype)) { //if by the end of reading the chunk, we are not at an end config
				//then there exists a dichotomy; the user may have inserted an over or undersized configuration, in which case we may have over or undershot another configuration
				filestream.clear();
				filestream.seekg(configstart); //go back to where the configuration started.
				int configurationrow{ 0 };
				std::pair<bool, std::string> iskey(false, "");
				while (!file.EndOfFile() && !iskey.first) { //read line by line, until we reach a key.
					iskey = KeyProcessing::CheckKeySyntax(file.NextLine());
					++configurationrow;
				}
				std::string errorstring;
				int alterrows = mapdimension.y - (configurationrow - 1); //measuring the length of the configuration.
				if (alterrows != 0) errorstring += "CHANGE_CONFIG "; //if config size doesn't match specified dimensions, config size := wrong. -ve:= truncate n rows. +ve := expand n rows.
				if (file.EndOfFile()) errorstring += "MISSING_END_CONFIGURATION "; //if at end of file, then this means we are missing the end config key.
				if (iskey.first) {
					std::string configmark = Attributes(iskey.second).GetWord();
					if (configmark == "START_CONFIGURATION" || configmark != "END_CONFIGURATION") {
						if (configmark != "END_CONFIGURATION" && configmark != "START_CONFIGURATION") { //invalid key.
							LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify entry in line number " + file.GetLineNumberString() + " " + mapstr);
						}
						errorstring += "MISSING_END_CONFIGURATION "; //if we are at a new start config, then we must be missing an end_config key.
						throw CustomException(std::move(errorstring));
					}
				}
				if (!errorstring.empty()) {
					CustomException exception(std::move(errorstring));
					exception.SetNum(alterrows);
					file.NextLine();
					throw std::move(exception);
				}
			}
			ReadConfiguration(std::move(configurationtype), std::move(configurationblock));
			file.NextLine();
		}
		catch (const CustomException& exception) {
			Attributes errorstream(static_cast<std::string>(exception.what()));
			std::string errortype = errorstream.GetWord();
			while (!errorstream.eof()) {
				if (errortype == "MISSING_END_CONFIGURATION") LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Missing {END_CONFIGURATION,x} key on line number " + file.GetLineNumberString() + mapstr);
				if (errortype == "CHANGE_CONFIG") {
					auto nrowchange = exception.GetNum();
					if (nrowchange > 0) {
						LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Configuration is " + std::to_string(nrowchange) + " rows too small " + mapstr + ". Defaulting the remaining required rows to 0...");
						//at this point, the mapdimensionmax - nrowchange row and onwards will be filled with junk values, by leakage from another configuration.
						auto pos = ((mapdimension.y - nrowchange) * (mapdimension.x + 1));
						std::string defaultrow(mapdimension.x, '0'); defaultrow.push_back('\n');
						configurationblock.erase(configurationblock.begin() + pos, configurationblock.end());
						for (int k = 0; k < nrowchange; ++k) {
							configurationblock.append(defaultrow);
						}
					}
					else if (nrowchange < 0) LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Configuration is " + std::to_string(nrowchange) + " rows too big " + mapstr + ". Ignoring the remaining rows...");
				}
				errortype = errorstream.GetWord();
			}
			ReadConfiguration(std::move(configurationtype), std::move(configurationblock));
		}
	}
}
Map::Map(Manager_Texture* mgr) :texturemgr(mgr){
	statictiles['0'] = std::make_unique<TileData::StaticTile>(texturemgr);
	LoadMap("MyMap.txt");
}

