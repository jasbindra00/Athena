#ifndef MANAGER_STATE_H
#define MANAGER_STATE_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include "GameStateType.h"
#include "State_Base.h"
#include "SharedContext.h"
class State_Base;
using StatePtr = std::unique_ptr<State_Base>;

using StateProducer = std::function<StatePtr(void)>;
using StateFactory = std::unordered_map<StateType, StateProducer>;
using StateQueue = std::vector<StateType>;
using StateObjects = std::vector <std::pair<StateType, StatePtr>>;


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
	void RegisterStateProducer(const StateType& state) {
		statefactory[state] = [this](void) {return std::make_unique<T>(this); };
	}
	void ProcessDestructions();
	void ProcessInsertions();
	
public:
	Manager_State(SharedContext* context);
	inline void QueueDestruction(const StateType& state) { destructionqueue.emplace_back(state); }
	inline void QueueInsertion(const StateType& state) { insertionqueue.emplace_back(state); }
	bool StateExists(const StateType& state) const;
	void RemoveStateProducer(const StateType& state);
	void Update(const float& dT);
	void Draw();
	void SwitchState(const StateType& s);
	SharedContext* GetContext() const { return context; }
};
#endif