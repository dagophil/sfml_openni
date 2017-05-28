#include <iostream>

#include <SFML/Graphics.hpp>

#include "widgets.hxx"

void create_widgets(kin::Widget & container)
{
    using namespace kin;

    auto grid0_ptr = std::make_shared<GridWidget>(2, 2);
    auto & grid0 = *grid0_ptr;





    auto grid1_ptr = std::make_shared<GridWidget>(3, 3);
    auto & grid1 = *grid1_ptr;

    for (size_t y = 0; y < 3; ++y)
    {
        for (size_t x = 0; x < 3; ++x)
        {
            auto im = std::make_shared<ImageWidget>("images/mario.png");
            grid1(x, y) = im;
        }
    }

//    grid1(1, 1)->set_width(30);
//    grid1(1, 1)->set_height(30);
//    grid1(1, 1)->align_x_ = CenterX;
//    grid1(1, 1)->align_y_ = CenterY;
//    grid1(1, 1)->scale_ = Fit;

    grid1(1, 1)->handle_mouse_enter_ = [](int x, int y)
    {
        std::cout << "mouse enter" << std::endl;
    };
    grid1(1, 1)->handle_mouse_leave_ = [](int x, int y)
    {
        std::cout << "mouse leave" << std::endl;
    };

    grid1.set_x_sizes(0.5f, 0.4f, 0.1f);
    grid1.set_y_sizes(0.2f, 0.6f, 0.2f);

    grid0(0, 1) = grid1_ptr;


    container.add_widget(grid0_ptr);
}

int main(int argc, char** argv)
{
    using namespace std;
    using namespace kin;

    // Window width and height.
    size_t WIDTH = 800;
    size_t HEIGHT = 600;

    // Create the main widget.
    Widget container;
    create_widgets(container);

    // Create the window.
    auto style = sf::Style::Close;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Test widgets", style);
    FPS fps_measure(1.0f);
    while (window.isOpen())
    {
        // Handle window events.
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Process the input.
        auto mouse_pos = sf::Mouse::getPosition(window);
        container.hover(mouse_pos.x, mouse_pos.y);

        // Update the widgets.
        auto fps = fps_measure.update();
        auto elapsed_time = fps_measure.elapsed_time();
        container.update(elapsed_time);

        // Draw everything.
        window.clear();
        container.render(window);
        window.display();
    }
}
