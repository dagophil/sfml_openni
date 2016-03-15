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
#include "screens/highscore_screen.hxx"

namespace kin
{

class HDMGame : public Widget
{
public:

    HDMGame();

    void hover_field(int x, int y);

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

    int hov_x_;
    int hov_y_;
    Event::ScreenID current_screen_;

};

HDMGame::HDMGame()
    :
      handle_close_(detail::do_nothing0),
      hov_x_(-1),
      hov_y_(-1)
{
    handle_hover_ = [&](DiffType x, DiffType y)
    {
        DiffType w = opts.mouse_->get_absolute_rectangle().GetWidth();
        DiffType h = opts.mouse_->get_absolute_rectangle().GetHeight();
        DiffType xx = x-0.45*w;
        DiffType yy = y-0.17*h;
        opts.mouse_->overwrite_render_rectangle({xx, yy, xx+w, yy+h});
    };

    // Add the background image.
    auto bg_im = std::make_shared<ImageWidget>("images/bg_grass.jpg");
    add_widget(bg_im);

    // Create the container.
    container_ = std::make_shared<Widget>();
    add_widget(container_);

    // Register the listener.
    listener_ = std::make_shared<Listener>();
    listener_->handle_notify_ = [&](Event const & event)
    {
        notify(event);
    };
    event_manager.register_listener(listener_);

    // Load the splash screen.
    opts.mouse_->show();
    load_screen(Event::HighscoreScreen);
}

void HDMGame::hover_field(int x, int y)
{
    hov_x_ = x;
    hov_y_ = y;
}

void HDMGame::update_impl(float elapsed_time)
{
    Event hov(Event::FieldHover);
    hov.field_hover_.x_ = hov_x_;
    hov.field_hover_.y_ = hov_y_;
    event_manager.post(hov);

    Event tick(Event::Tick);
    tick.tick_.elapsed_time_ = elapsed_time;
    event_manager.post(tick);
    hov_x_ = -1;
    hov_y_ = -1;
}

/**
 * @brief Helper function to create a screen of type T.
 */
template <typename T>
std::shared_ptr<Widget> create_screen(){
    return std::make_shared<T>();
}

/**
 * @brief std::function type of the create_screen function.
 */
typedef std::function<std::shared_ptr<Widget>()> ScreenFunction;

void HDMGame::load_screen(Event::ScreenID id)
{
    current_screen_ = id;

    // Remove the current widgets.
    container_->clear_widgets();
    event_manager.clear_delayed_calls();

    // Fill the map with the screen functions.
    std::map<Event::ScreenID, ScreenFunction> m {
        {Event::SplashScreen, ScreenFunction(create_screen<SplashScreen>)},
        {Event::MainMenuScreen, ScreenFunction(create_screen<MainMenuScreen>)},
        {Event::ManualScreen, ScreenFunction(create_screen<ManualScreen>)},
        {Event::GameScreen, ScreenFunction(create_screen<GameScreen>)},
        {Event::HighscoreScreen, ScreenFunction(create_screen<HighscoreScreen>)}
    };

    // Get the correct screen function to create the screen and add it to the widget container.
    auto it = m.find(id);
    if (it != m.end())
    {
        auto const & f = it->second;
        auto screen = f();
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
        load_screen(event.change_screen_.screen_id_);
    }
    else if (event.type_ == Event::Close)
    {
        handle_close_();
    }
}



}

#endif
