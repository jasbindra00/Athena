#include "Map.h"
#include "EnumConverter.h"
#include "StreamAttributes.h"
#include "MapTile.h"
#include "Utility.h"
using namespace MapData;
using KeyProcessing::Key;
using KeyProcessing::Keys;
using namespace MapData::ConfigurationData;
void Map::ReadConfiguration(const ConfigurationType& type, const int& num, const std::string& configurationblock){
	ConfigurationReader reader = configurationreaders[type];
	Attributes configurationstream(configurationblock);
	std::string errorstring;
	std::string alteredstring;
	int rownumber{ -1 };
	while (configurationstream.NextWord()) {
		++rownumber;
		std::string configurationrow(configurationstream.ReturnWord());
		if (configurationrow.empty()) {
			std::cout<<std::endl;
		}
		auto changerecord = FixConfigurationRow(type, configurationrow);
		if (!changerecord.empty()) {
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Map configuration " + std::to_string(Utility::ConvertToUnderlyingType(type)) + " row number " + std::to_string(rownumber) + " was changed in the following ways : " + changerecord);
		}
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
	using namespace TileData;
	FileReader file;
	if (!file.LoadFile(tilefile)) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the standard tile file of name " + tilefile);
		return false;
	}
	std::string DidNotReadTile{ " DID NOT READ TILE..." };
	std::string TileFileStr{ " in tile file of name " + tilefile };
	file.NextLine();
	{	std::vector<Key> tiledimensionkeys = KeyProcessing::SortKeys("TILEDIMENSIONX TILEDIMENSIONY", file.ReturnLine(), false);
		try {
			tiledimension.x = std::stoi(tiledimensionkeys[0].second);
			tiledimension.y = std::stoi(tiledimensionkeys[1].second);
			if (tiledimension.x <= 0 || tiledimension.y <= 0) throw std::exception();
		}
		catch (const std::exception& exception) {
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Invalid tile dimensions" + TileFileStr + ". Exiting map read..");
			return false;
		}
	}
	auto tileattributes = static_cast<Attributes*>(&file.GetLineStream());
	std::string line;
	while (file.NextLine().GetFileStream()) {
		auto statictileobj = std::make_unique<StaticTile>(texturemgr);
		char tileid = '0';
		Keys tilekeys = KeyProcessing::SortKeys("TILEID TEXTURESIZEX TEXTURESIZEY FRICTIONX FRICTIONY TOPLEFTX TOPLEFTY  SPRITESHEET",file.ReturnLine(), true);
		try {
			auto& tileidstr = tilekeys[0].second;
			if (tileidstr.size() != 1) throw CustomException("Invalid TILEID argument - TILEID must be a single char"); //must be a char
			else if (tileidstr[0] == '0') throw CustomException("Invalid TILEID argument - '0' is a reserved TILEID");
			else if (statictiles.find(tileidstr[0]) != statictiles.end()) throw CustomException("TILEID '" + tileidstr + " already exists.");
			tileid = tileidstr[0];
		}
		catch (const CustomException& exception) {
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, exception.what() + TileFileStr + " on line number " + file.GetLineNumberString() + DidNotReadTile);
			return;
		}
		try { statictileobj->InitTile(tilekeys); }
		catch (const CustomException& exception) {
			Attributes errorstream(exception.what());
			while (errorstream.NextWord()) {
				std::string errortype = errorstream.ReturnWord();
				if (errortype == "TEXTURE" ) {
					statictileobj->SetTexture("ERRORTILE");
					LOG::Log(LOCATION::MAP, LOGTYPE::ERROR,__FUNCTION__, "Invalid texture initialisation of TILEID " + std::to_string(tileid) + TileFileStr + ".ERROR TEXTURE SET...");
					statictileobj->GetTextureRect().width = 32;
					statictileobj->GetTextureRect().height = 32;
					statictileobj->SetTexture("ERRORTILE");
				}
			}
		}
		//scale the sprite texture to match the specified tile pixel dimensions.
		sf::Sprite& tilesprite = statictileobj->GetTileSprite();
		sf::IntRect texturerect(statictileobj->GetTextureRect());
		texturerect.top *= tiledimension.y;
		texturerect.left *= tiledimension.x;
		tilesprite.setTextureRect(std::move(texturerect));
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
	Attributes linestream(file.ReturnLine());
	Keys foundkeys = KeyProcessing::SortKeys("TILEFILE MAPDIMENSIONX MAPDIMENSIONY", file.ReturnLine(), false);
	try {
		if (foundkeys.size() != 3) throw CustomException("Unable to initialise map - Missing {TILEFILE,file} / {MAPDIMENSIONX,x} / {MAPDIMENSION,y} keys. EXITING MAP READ...");
		if (!ReadStaticTiles(foundkeys[2].second)) throw CustomException("Unable to read static tile file of name " + foundkeys[2].second);
		try {
			mapdimension.x = std::stoi(foundkeys[1].second);
			mapdimension.y = std::stoi(foundkeys[2].second);
			if (mapdimension.x <= 0 || mapdimension.y <= 0) throw std::exception();
		}
		catch (const CustomException& e) { throw CustomException("Invalid map dimension arguments"); }
	}
	catch (const CustomException& exception) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, errorstart + exception.what() + ". EXITING MAP READ...");
		return;
	}
	file.NextLine();
	if (file.PeekWord() != "{START_CONFIGURATION,GAME_MAP} {LAYER,0}") {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, errorstart + "First configuration should be {START_CONFIGURATION,GAME_MAP} )
	}

		
		catch (const CustomException& exception) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, errorstart + std::string{ exception.what() } + ". Exiting map file read..."); return; }	
		auto& filestream = file.GetFileStream();
		file.PutBackLine();
		while (file.NextLine().GetFileStream()) {
			int layernum{ 0 };
			auto foundkeys = KeyProcessing::FindKeys("START_CONFIGURATION LAYER", file.ReturnLine());
			if (foundkeys[0].first) {

			}
			if (foundkeys[1].first) {

			}







			{
				
				auto foundkeys = KeyProcessing::FindKeys("START_CONFIGURATION LAYER", file.ReturnLine());
				if (foundkeys[0].first) {

				}
				if(foundkeys[1].first)

				std::string configurationtypestr;
				auto foundkey = KeyProcessing::FindKey("LAYER", keys);
				if (foundkey.first == true) {
					const auto& layernumstr = std::get<2>(*foundkey.second);
					if (KeyProcessing::IsOnlyNumeric(layernumstr)) {
						layernum = std::stoi(layernumstr);
					}
				}
				auto foundconfigstartkey = KeyProcessing::FindKey("START_CONFIGURATION", keys);
				if (foundkey.first == true) {
					configurationtypestr = std::get<2>(*foundkey.second);
					configtype = configurationconverter(configurationtypestr);
				}
			}
			if (configtype != ConfigurationType::NULLTYPE) {
				std::string configurationblock((mapdimension.x * mapdimension.y) + mapdimension.y, '0');
				filestream.read(configurationblock.data(), std::streamsize(mapdimension.x * mapdimension.y) + mapdimension.y);
				if (file.NextLine().ReturnLine() == "END_CONFIGURATION") {
					ReadConfiguration(configtype, 3, configurationblock);
					continue;
				}
			}
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "INVALID CONFIGURATION SYNTAX / DIMENSIONS. TRYING TO SEEK TO NEXT CONFIGURATION...");
			filestream.clear();
			filestream.seekg(configstartpos);
			file.SeekToLineKey("{START_CONFIGURATION,x}", false, true, false); //go to the next start configuration
		}
}
std::string Map::FixConfigurationRow(const ConfigurationType& type, std::string& configurationrow){
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
			if (c < '0' || c > '3') {
				c = '0';
				altered = true;
			}
		}
	}
	else if (type == ConfigurationType::DEADLY || type == ConfigurationType::TELEPORT) {
		for (auto& c : configurationrow) {
			if (c != '0' || c != '1') {
				c = '0';
				altered = true;
			}
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

