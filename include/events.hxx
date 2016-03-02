#ifndef EVENTS_HXX
#define EVENTS_HXX

#include <memory>
#include <vector>
#include <queue>

#include "utility.hxx"

namespace kin
{

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
        GameScreen
    };

    struct ChangeScreenEvent
    {
        ScreenID screen_id;
    };

    enum EventType
    {
        ChangeScreen,
        Tick,
        Close
    };

    Event(EventType type)
        :
          type_(type)
    {}

    EventType const type_;

    union
    {
        ChangeScreenEvent change_screen_;
    };

};

class Listener
{
public:

    Listener()
        :
          handle_notify_(detail::do_nothing1<Event>)
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

    void post(Event event)
    {
        if (event.type_ == Event::Tick)
        {
            while (!queue_.empty())
            {
                for (auto l : listeners_)
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

private:

    std::vector<ListenerPointer> listeners_;
    std::queue<Event> queue_;

};

}

#endif
