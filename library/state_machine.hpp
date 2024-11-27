//
// Created by scros on 11/27/24.
//

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP
#include <functional>
#include <unordered_set>

template <class TState>
class StateMachine
{
public:
    void addState(const TState& state);
    void addTransition(const TState& startState, const TState& finalState, const std::function<void()>& lambda);
    void addAction(const TState& state, const  std::function<void()>& lambda);
    void transitionTo(const TState& state);
    void update();

private:
    std::unordered_set<TState> states;
    std::unordered_map<std::pair<TState, TState>, >
};

#endif //STATE_MACHINE_HPP
