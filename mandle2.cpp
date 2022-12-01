

#include <iostream>
#include <complex>
#include <string.h>
#include <thread>
#include <SFML/Graphics.hpp>
#include <mutex>

//bash compile 
//g++ mandel.cpp  -lsfml-graphics -lsfml-window -lsfml-system && ./a.out
std::mutex mtx;
using __Complex = std::complex<double>;//_Commplex is already used in this implementation of c++. so i added an extra _ :P
using _Threads = std::vector<std::thread>;
_Threads global_threads;//so that i dont have to initialize/ allocate memory every time i use _Threads. 


bool escaped_pixel[800][800];


void clear_pix_arr(void) {
    for (int x = 0; x < 800; x++) {
        for (int y = 0; y < 800; y++) {
            escaped_pixel[x][y] = false;
        }
    }
}

__Complex ret_mandel(__Complex val1, __Complex val2) {

    return (val1 * val1) + val2;
};
void _th_calc(int xstart, int skip, int ystart, int pixels, double scale, int stop, sf::Image& image) {
    //initialize screen buffer with the number of iterations per pixel if i already know that the position has exited the mandelbrot set, 
    //i shouldn't calculate it, so im thinking of storing "illegal" arguements of this function in a global array( or passing that array directly)
    //and checking for if pixel args == current args, than pass on execution .
    __Complex z = { 0,0 };
    double size = image.getSize().x;
    int iters = 0;
    for (int x = xstart; x < xstart + pixels; x++) {
        for (int y = ystart; y < ystart + size; y++) {
            //std::cout<<"you should see this\n";
            //start -> pixels = range of mandelbrot handled per thread, same for 
            //escaped pixel array. Im getting read access violations because of it, I'm guessing a thread accessing and writing to 
            //escaped pixels causes a bug 
            //std::cout << escaped_pixel[x - xstart][y - ystart];// i was indexing at a negative number because i wasn't sub
            //by the offset 
            if (escaped_pixel[x - xstart][y - ystart] == true) {//should work ...
                
                
                //the pixel already get's drawn, so all i have to do is return 
            }
            else {
                z = { 0,0 };
                for (iters = 0; iters < stop and abs(z) < 2; iters++) {
                    z = ret_mandel(z, { (double(x) / (double)size) / (double)scale, (double(y) / (double)size) / (double)scale });
                    if (iters == stop - 1) {
                        mtx.lock();
                        escaped_pixel[x - xstart][y - ystart] = true;


                        image.setPixel(x - xstart + skip, y - ystart, sf::Color::Black);
                        mtx.unlock();
                        break;
                    }
                }
                if (iters == stop - 1) {

                }
                else {
                    image.setPixel(x - xstart + skip, y - ystart, sf::Color(iters * 6, iters * 8, iters * 16));
                }
                
            }

        }
    }
    return;

}
void thread_Draw(int divisions, int x_start, int y_start, double scale, int iters, sf::Image& window) {//returns a vector of threads

    int partitions = window.getSize().x / divisions;//doesnt matter much tbh
    for (int x = 0; x < divisions; x++) {
        //call calculator function
        //std::cout << "threads  = " << x << std::endl;
        global_threads.emplace_back(_th_calc, x_start + (partitions)*x, (partitions)*x, y_start, partitions, scale, iters, std::ref(window));//emplace need a referance wrapper for somereason 
    }
    return;
};

int main()
{
    clear_pix_arr();
    sf::Event event;
    sf::RenderWindow window({ 800,800 }, "mandelbrot");

    sf::Texture texture;

    sf::Image image;
    image.create(window.getSize().x, window.getSize().y, sf::Color::Black);
    texture.loadFromImage(image);
    sf::Sprite screen(texture);
    double scale = .1;
    for (int x = 1; x < 80; x++) {
        thread_Draw(12, -400, -400, .1, x, image);

        for (auto& th : global_threads) {
            if (th.joinable()) {
                th.join(); //wait for threads to exit before continuing. without this it crashes.
            }
            th.~thread();
        }

        clear_pix_arr();
        window.draw(screen);
        window.display();
    }
    sf::Vector2i pos;

    while (window.isOpen())
    {


        while (window.pollEvent(event)) {
            if (event.type == event.Closed) {
                window.close();
                exit(0);
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) { //zoom in

                    scale /= 2;

                }
                else if (event.mouseWheelScroll.delta < 0) { //zoom out, why would you ever want to?


                    scale *= 2;
                }
                image.create(window.getSize().x, window.getSize().y, sf::Color::Black);

                pos = sf::Mouse::getPosition(window);
                for (int x = 1; x < 50; x++) {
                    thread_Draw(12, -400 + (pos.x - 400) * scale, -400 + (pos.y - 400) * scale, scale, x, image);
                    for (auto& th : global_threads) {
                        if (th.joinable()) {
                            th.join(); //wait for threads to exit before continuing. without this it crashes.
                        }
                        th.~thread();
                    }
                    while (window.pollEvent(event)) {
                        //ubuntu is much more picky whenever the window doesn't respond
                        //so polling events here is fine
                        if (event.type == event.Closed) {
                            window.close();
                            exit(0);
                        }
                    }
                    clear_pix_arr();

                    texture.loadFromImage(image);
                    sf::Sprite screen(texture);

                    window.draw(screen);
                    window.display();
                }
            }


        }
        texture.loadFromImage(image);
        sf::Sprite screen(texture);

        window.draw(screen);
        window.display();
    }
}
