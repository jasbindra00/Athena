#ifndef MANAGER_STATE_H
#define MANAGER_STATE_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include "SharedContext.h"



namespace GameState {
	enum class GameStateType;
};
using GameState::GameStateType;

class State_Base;
using StatePtr = std::unique_ptr<State_Base>;

using StateProducer = std::function<StatePtr(void)>;
using StateFactory = std::unordered_map<GameStateType, StateProducer>;
using StateQueue = std::vector<GameStateType>;
using StateObjects = std::vector <std::pair<GameStateType, StatePtr>>;



class EventManager;
class Manager_State
{
private:
	StateQueue insertionqueue;
	StateQueue destructionqueue;
	StateFactory statefactory;
	StateObjects statestack;
	SharedContext* context;
	
	template<typename T>
	void RegisterStateProducer(const GameStateType& state) {
		statefactory[state] = [this](void) {return std::make_unique<T>(this); };
	}
	void ProcessDestructions();
	void ProcessInsertions();
	
public:
	Manager_State(SharedContext* context);
	inline void QueueDestruction(const GameState::GameStateType& state) { destructionqueue.emplace_back(state); }
	inline void QueueInsertion(const GameStateType& state) { insertionqueue.emplace_back(state); }
	bool StateExists(const GameStateType& state) const;
	void RemoveStateProducer(const GameStateType& state);
	void Update(const float& dT);
	void Draw();
	void SwitchState(const GameStateType& s);
	SharedContext* GetContext() const { return context; }
};
#endif