

#include <iostream>
#include <complex>
#include <thread>
#include <SFML/Graphics.hpp>


//bash compile 
//g++ mandel.cpp  -lsfml-graphics -lsfml-window -lsfml-system && ./a.out

using __Complex = std::complex<double>;//_Commplex is already used in this implementation of c++. so i added an extra _ :P
using _Threads = std::vector<std::thread>;
_Threads global_threads;//so that i dont have to initialize/ allocate memory every time i use _Threads. 
__Complex ret_mandel(__Complex val1, __Complex val2) {
	
	return (val1*val1) + val2;
};
void _th_calc(int xstart,int skip, int ystart,int pixels, double scale, int stop, sf::Image& image) {
	//initialize screen buffer with the number of iterations per pixel if i already know that the position has exited the mandelbrot set, 
    //i shouldn't calculate it, so im thinking of storing "illegal" arguements of this function in a global array( or passing that array directly)
    //and checking for if pixel args == current args, than pass on execution .
	__Complex z = { 0,0 };
	double size = image.getSize().x;
	int iters = 0;
	for (int x = xstart ; x < xstart + pixels; x++) {
		for (int y = ystart; y < ystart + size; y++) {
			z = { 0,0 };
			for (iters = 0; iters < stop and abs(z) < 2; iters++) {
				z = ret_mandel(z, { (double(x) / size) / scale, (double(y) / size) / scale });
				if (iters == stop - 1) {
					image.setPixel(x - xstart + skip, y - ystart, sf::Color::Black);
					break;
				}
			}
			if (iters == stop - 1) {//i could use a goto statement here, but they are EVIL. 
			}
			else{
				image.setPixel(x - xstart + skip, y - ystart, sf::Color(iters * 6, iters * 8, iters * 16));
			}
		}
	}
	return;

}
void thread_Draw(int divisions, int x_start, int y_start, double scale, int iters, sf::Image& window) {//returns a vector of threads
	
	int partitions = window.getSize().x / divisions;//doesnt matter much tbh
	for (int x = 0; x < divisions; x++) {
		//call calculator function
		global_threads.emplace_back(_th_calc, x_start +(partitions)*x, (partitions)*x, y_start, partitions, scale, iters, std::ref(window));//emplace need a referance wrapper for somereason 
	}
	return ;
};

int main()
{
	sf::Event event;
	sf::RenderWindow window({ 800,800 }, "mandelbrot");

	sf::Texture texture;

	sf::Image image;
	image.create(window.getSize().x, window.getSize().y, sf::Color::Black);
	texture.loadFromImage(image);
	sf::Sprite screen(texture);
	double scale = .1;
	for (int x = 1; x < 40; x++) {
		thread_Draw(12, -400, -400, .1,x, image);

		for (auto& th : global_threads) {
			if (th.joinable()) {
				th.join(); //wait for threads to exit before continuing. without this it crashes.
			}
			th.~thread();
		}
		

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

					scale /=2;
					
				}
				else if (event.mouseWheelScroll.delta < 0) { //zoom out, why would you ever want to?


					scale *=2;
				}
				image.create(window.getSize().x, window.getSize().y, sf::Color::Black);

				pos = sf::Mouse::getPosition(window);
				for (int x = 1; x < 50; x++) {
					thread_Draw(12, -400 + (pos.x-400) * scale, -400 + (pos.y - 400) * scale, scale,x, image);
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
