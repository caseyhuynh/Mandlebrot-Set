#include "ComplexPlane.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <complex>



using namespace sf;
using namespace std;

int main()
{
     int pixelWidth = VideoMode::getDesktopMode().width;
     int pixelHeight = VideoMode::getDesktopMode().height;

    VideoMode vm(pixelWidth, pixelHeight);
    RenderWindow window(vm, "Mandlebrot Project", Style::Default);
    ComplexPlane balls(pixelWidth, pixelHeight);

    bool update = true;

    Text MandleText;
    Font font;
    font.loadFromFile("Fonts/arial.ttf");
    MandleText.setFont(font);
    MandleText.setFillColor(Color::White);
    

    while (window.isOpen())
    {
        // Event handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "the left button was pressed" << std::endl;
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                    Vector2i mCoords = { event.mouseButton.x, event.mouseButton.y };
                    balls.setCenter(mCoords);
                    balls.zoomIn();
                    update = true;
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    std::cout << "the right button was pressed" << std::endl;
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                    Vector2i mCoords = { event.mouseButton.x, event.mouseButton.y };
                    balls.setCenter(mCoords);
                    balls.zoomOut();
                    update = true;
                }

            }
            if (event.type == sf::Event::MouseMoved)
            {
                
                Vector2i mCoords = { event.mouseMove.x, event.mouseMove.y }; 
                balls.setMouseLocation(mCoords);
                update = true;
               
            }

            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
            }

            // Game logic updates (e.g., move characters, handle collisions)
            // ...
            if (update)
            {
                balls.updateRender();
                balls.loadText(MandleText);
                update = false;
            }

            // Rendering
            window.clear();
            // Draw game objects
            // ...
           
            window.draw(balls);
            window.draw(MandleText);
            window.display();
        }




    }
}


