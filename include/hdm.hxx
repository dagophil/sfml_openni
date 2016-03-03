#ifndef HDM_HXX
#define HDM_HXX

#include <map>

#include <SFML/Graphics.hpp>

#include "widgets.hxx"
#include "events.hxx"
#include "options.hxx"
#include "screens/manual_screen.hxx"
#include "screens/main_menu_screen.hxx"
#include "screens/splash_screen.hxx"
#include "screens/game_screen.hxx"

namespace kin
{

class HDMGame : public Widget
{
public:

    HDMGame(size_t width, size_t height);

    std::function<void()> handle_close_;

protected:

    /**
     * @brief Post the tick event.
     */
    void update_impl(float elapsed_time);

private:

    void notify(Event const & event); // callback for events
    void load_screen(Event::ScreenID id); // the load function for the screens

    std::shared_ptr<Widget> container_; // the main container
    std::shared_ptr<Listener> listener_; // the main listener

};

HDMGame::HDMGame(
        size_t width,
        size_t height
)   :
      Widget(0, 0, width, height, 0),
      handle_close_(detail::do_nothing0)
{
    handle_hover_ = [&](DiffType x, DiffType y)
    {
        auto old_x = opts.mouse_->rect_.Left;
        auto old_y = opts.mouse_->rect_.Top;
        auto w = opts.mouse_->rect_.GetWidth();
        auto h = opts.mouse_->rect_.GetHeight();
        opts.mouse_->rect_.Offset(x-old_x - 0.45*w, y-old_y - 0.17*h);
    };

    // Add the background image.
    add_widget(std::make_shared<ImageWidget>(
            "images/bg_grass.jpg",
            0, 0,
            width, height,
            0
    ));

    // Create the container.
    container_ = std::make_shared<Widget>(0, 0, width, height, 0);
    add_widget(container_);

    // Register the listener.
    listener_ = std::make_shared<Listener>();
    listener_->handle_notify_ = [&](Event const & event)
    {
        notify(event);
    };
    event_manager.register_listener(listener_);

    // Load the splash screen.
    load_screen(Event::SplashScreen);
}

void HDMGame::update_impl(float elapsed_time)
{
    event_manager.post(Event(Event::Tick));
}

/**
 * @brief Helper function to create a screen of type T.
 */
template <typename T>
std::shared_ptr<Widget> create_screen(
        HDMGame::DiffType x,
        HDMGame::DiffType y,
        HDMGame::DiffType width,
        HDMGame::DiffType height,
        int z_index
){
    return std::make_shared<T>(x, y, width, height, z_index);
}

/**
 * @brief std::function type of the create_screen function.
 */
typedef std::function<std::shared_ptr<Widget>(
        HDMGame::DiffType,
        HDMGame::DiffType,
        HDMGame::DiffType,
        HDMGame::DiffType,
        int
)> ScreenFunction;

void HDMGame::load_screen(Event::ScreenID id)
{
    // Remove the current widgets.
    container_->clear_widgets();

    // Fill the map with the screen functions.
    std::map<Event::ScreenID, ScreenFunction> m {
        {Event::SplashScreen, ScreenFunction(create_screen<SplashScreen>)},
        {Event::MainMenuScreen, ScreenFunction(create_screen<MainMenuScreen>)},
        {Event::ManualScreen, ScreenFunction(create_screen<ManualScreen>)},
        {Event::GameScreen, ScreenFunction(create_screen<GameScreen>)}
    };

    // Get the correct screen function to create the screen and add it to the widget container.
    auto it = m.find(id);
    if (it != m.end())
    {
        auto const & f = it->second;
        auto screen = f(
                    0, 0,
                    container_->rect_.GetWidth(),
                    container_->rect_.GetHeight(),
                    1
        );
        container_->add_widget(screen);
    }
    else
    {
        throw std::runtime_error("Unknown screen.");
    }
}

void HDMGame::notify(Event const & event)
{
    if (event.type_ == Event::ChangeScreen)
    {
        load_screen(event.change_screen_.screen_id);
    }
    else if (event.type_ == Event::Close)
    {
        handle_close_();
    }
}



}

#endif
