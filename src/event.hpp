#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>
#include <map>
#include <queue>

/**
 * @brief Event class for storing different types of events that can occur in the game.
*/
class Event {
    public:
        enum class Type {
            BirdLaunching,
            BirdLaunched,
            ObjectCountUpdated,
            ScoreUpdated,
            UpdateHUD,
            UpdateView,
            DefaultView,
        };
        explicit Event(Type type) : type_(type) {}
        Type getType() const { return type_; }
    private:
        Type type_;
};

/**
 * @brief EventQueue class is responsible for storinng and consuming events.
*/
class EventQueue {
    public:
        void push(const Event& event) {
            events_.push(event);
        }

        bool hasEvents() const {
            return !events_.empty();
        }

        Event poll() {
            Event event = events_.front();
            events_.pop();
            return event;
        }
    private:
        std::queue<Event> events_;
};

/**
 * @brief EventDispatcher class is responsible for dispatching events to the event queue. Thus, separates consumption of events from their production.
*/
class EventDispatcher {
    public:
        EventDispatcher(EventQueue& eventQueue) : eventQueue_(eventQueue) {}

        // Dispatch an event by pushing it into the event queue
        void dispatch(const Event& event) {
            eventQueue_.push(event);
        }

    private:
        EventQueue& eventQueue_;
};

#endif // EVENT_HPP