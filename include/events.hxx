#ifndef EVENTS_HXX
#define EVENTS_HXX

#include <memory>
#include <vector>
#include <queue>
#include <functional>

#include "utility.hxx"

namespace kin
{

struct DelayedCall
{
    DelayedCall(float delay, std::function<void()> f)
        :
          delay_(delay),
          f_(f)
    {}
    float delay_;
    std::function<void()> f_;
};

class Event
{
public:

    enum ScreenID
    {
        SplashScreen,
        MainMenuScreen,
        HighscoreScreen,
        ManualScreen,
        CreditsScreen,
        GameScreen,
        OptionsScreen
    };

    struct ChangeScreenEvent
    {
        ChangeScreenEvent(ScreenID id)
            :
              screen_id_(id)
        {}
        ScreenID screen_id_;
    };

    struct TickEvent
    {
        TickEvent(float elapsed_time)
            :
              elapsed_time_(elapsed_time)
        {}
        float elapsed_time_;
    };

    struct FieldHoverEvent
    {
        int x_;
        int y_;
    };

    enum EventType
    {
        ChangeScreen,
        Tick,
        Close,
        MoleHit,
        KinectClick,
        FieldHover,
        ToggleSound
    };

    Event(EventType type)
        :
          type_(type)
    {}

    Event(ChangeScreenEvent const & other)
        :
          type_(ChangeScreen),
          change_screen_(other)
    {}

    EventType const type_;

    union
    {
        ChangeScreenEvent change_screen_;
        TickEvent tick_;
        FieldHoverEvent field_hover_;
    };

};

class Listener
{
public:

    Listener()
        :
          handle_notify_(detail::do_nothing1<Event const &>)
    {}

    void notify(Event const & event)
    {
        notify_impl(event);
        handle_notify_(event);
    }

    std::function<void(Event const &)> handle_notify_;

protected:

    virtual void notify_impl(Event const & event)
    {}

};

/**
 * @brief Receives events and posts them to the listeners.
 */
class EventManager
{
public:

    typedef std::shared_ptr<Listener> ListenerPointer;

    void register_listener(ListenerPointer listener)
    {
        listeners_.push_back(listener);
    }

//    void unregister_listener(ListenerPointer listener)
//    {
//        auto it = std::find(listeners_.begin(), listeners_.end(), listener);
//        if (it != listeners_.end())
//            listeners_.erase(it);
//    }

    void add_delayed_call(float delay, std::function<void()> f)
    {
        delayed_calls_.emplace_back(delay, f);
    }

    void post(Event event)
    {
        if (event.type_ == Event::Tick)
        {
            // Check if a delayed event should be fired.
            for (auto & e : delayed_calls_)
            {
                e.delay_ -= event.tick_.elapsed_time_;
                if (e.delay_ <= 0)
                {
                    e.f_();
                }
            }
            delayed_calls_.erase(
                std::remove_if(
                            delayed_calls_.begin(),
                            delayed_calls_.end(),
                            [](DelayedCall const & e){
                                return e.delay_ <= 0;
                            }
                ),
                delayed_calls_.end()
            );

            // Pass all events to the listeners.
            while (!queue_.empty())
            {
                auto listeners_copy = listeners_;
                for (auto l : listeners_copy)
                {
                    l->notify(queue_.front());
                }
                queue_.pop();
            }
        }
        else
        {
            queue_.push(event);
        }
    }

    void clear_delayed_calls()
    {
        delayed_calls_.clear();
    }

private:

    std::vector<ListenerPointer> listeners_;
    std::queue<Event> queue_;
    std::vector<DelayedCall> delayed_calls_;

};

EventManager event_manager; // the global event manager

}

#endif
