//
// Created by scros on 11/27/24.
//

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP
#include <functional>
#include <optional>
#include <unordered_map>
#include <tuple>
#include <stdexcept>

template <class TState>
class StateMachine
{
public:
    using transition_t = std::function<void()>;
    using action_t = std::function<void()>;

private:
    struct StateData
    {
        std::optional<action_t> action;
        std::unordered_map<TState, transition_t> transitions;
    };

public:
    void addState(const TState& state)
    {
        bool result;
        std::tie(std::ignore, result) = m_states.try_emplace(state);
        if (!result)
            throw std::invalid_argument("State already registered");

        if (!m_currentState.has_value())
            m_currentState.emplace(state);
    }

    void addTransition(const TState& startState, const TState& finalState, const std::function<void()>& lambda)
    {
        const auto it = m_states.find(startState);
        if (it == m_states.end())
            throw std::invalid_argument("Unknown state");

        bool result;
        std::tie(std::ignore, result) = it->second.transitions.try_emplace(finalState, lambda);
        if (!result)
            throw std::invalid_argument("This transition already exist");
    }

    void addAction(const TState& state, const std::function<void()>& lambda)
    {
        const auto it = m_states.find(state);
        if (it == m_states.end())
            throw std::invalid_argument("Unknown state");

        if (it->second.action.has_value())
            throw std::invalid_argument("Action already exist for this state");
        it->second.action.emplace(lambda);
    }

    void transitionTo(const TState& state)
    {
        if (!m_currentState.has_value())
            throw std::logic_error("No state registered");

        auto stateData = m_states[*m_currentState];
        const auto it = stateData.transitions.find(state);
        if (it == stateData.transitions.end())
            throw std::invalid_argument("No transition available");

        std::invoke(it->second);
        m_currentState = state;
    }

    void update()
    {
        if (!m_currentState.has_value())
            throw std::logic_error("No state registered");

        auto stateData = m_states[*m_currentState];
        if (!stateData.action.has_value())
            throw std::logic_error("No action for the current state");

        std::invoke(*stateData.action);
    }

private:
    std::optional<TState> m_currentState;
    std::unordered_map<TState, StateData> m_states;
};

#endif //STATE_MACHINE_HPP
