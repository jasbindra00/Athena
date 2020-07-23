#include "LOG.h"
std::unordered_map<LOGTYPE, LocationLogs> LOG::logs{};
sf::Clock LOG::time = sf::Clock{};