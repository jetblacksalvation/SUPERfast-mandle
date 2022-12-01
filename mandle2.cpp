// mandle2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <complex>
#include<thread>
#include <SFML/Graphics.hpp>

using _Complex = std::complex<double>;
using _Threads = std::vector<std::thread>;
_Threads global_threads;//so that i dont have to initialize/ allocate memory every time i use _Threads. 
_Complex ret_mandel(_Complex val1, _Complex val2) {
	
	return (val1*val1) + val2;
};
void _th_calc(int xstart,int skip, int ystart,int pixels, double scale, int stop, sf::Image& image) {
	//initialize s_buffer with the number of iterations per pixel
	_Complex z = { 0,0 };
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
			if (iters == stop - 1) {
				image.setPixel(x - xstart + skip, y - ystart, sf::Color::Black);
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
		thread_Draw(16, -400, -400, .1,x, image);

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
				//std::cout << " x = " << pos.x << " y = " << pos.y << std::endl;
				//system("pause");
				for (int x = 1; x < 100; x++) {
					thread_Draw(16, -400 + (pos.x-400) * scale, -400 + (pos.y - 400) * scale, scale,x, image);
					for (auto& th : global_threads) {
						if (th.joinable()) {
							th.join(); //wait for threads to exit before continuing. without this it crashes.
						}
						th.~thread();
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
