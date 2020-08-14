#ifndef MANAGER_STATE_H
#define MANAGER_STATE_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include "SharedContext.h"
#include "State_Base.h"
#include "Log.h"


namespace GameStateData {
	enum class GameStateType;
};
using GameStateData::GameStateType;


namespace GameStateInfo {
	using StatePtr = std::unique_ptr<State_Base>;
	using StateProducer = std::function<StatePtr(void)>;
	using StateFactory = std::unordered_map<GameStateType, StateProducer>;
	using StateQueue = std::vector<GameStateType>;
	using StateObjects = std::vector <std::pair<GameStateType, StatePtr>>;
}

using namespace GameStateInfo;
class EventManager;
class Manager_GUI;
class Manager_State
{
private:
	StateQueue insertionqueue;
	StateQueue destructionqueue;
	StateFactory statefactory;
	StateObjects statestack;
	SharedContext* context;
	Manager_GUI* guimgr;
	
	template<typename T>
	bool RegisterStateProducer(const GameStateType& state) {
		statefactory[state] = [this](void) {return std::make_unique<T>(this, guimgr); };
		return true;
	}
	void ProcessDestructions();
	void ProcessInsertions();
	
public:
	Manager_State(SharedContext* context, Manager_GUI* guimanager);
	inline void QueueDestruction(const GameStateData::GameStateType& state) { destructionqueue.emplace_back(state); }
	inline void QueueInsertion(const GameStateType& state) { insertionqueue.emplace_back(state); }
	bool StateExists(const GameStateType& state) const;
	void RemoveStateProducer(const GameStateType& state);
	void Update(const float& dT);
	void Draw();
	void SwitchState(const GameStateType& s);
	SharedContext* GetContext() const { return context; }
};
#endif