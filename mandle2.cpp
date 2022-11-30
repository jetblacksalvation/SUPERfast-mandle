// mandle2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <complex>
#include<thread>
#include <SFML/Graphics.hpp>

using _Complex = std::complex<double>;
using _Threads = std::vector<std::thread>;

_Complex ret_mandel(_Complex val1, _Complex val2) {
	
	return (val1*val1) + val2;
};
void _th_calc(int xstart,int skip, int ystart,int pixels, double scale, sf::Image& image) {
	//initialize s_buffer with the number of iterations per pixel
	_Complex z = { 0,0 };
	int iters = 0;
	for (int x = xstart ; x < xstart + pixels; x++) {
		for (int y = ystart; y < ystart + 800; y++) {
			z = { 0,0 };
			for (iters = 0; iters < 1000 and abs(z) < 2; iters++) {
				z = ret_mandel(z, { (double(x) / 800) / scale, (double(y) / 800) / scale });

			}
			image.setPixel(x -xstart +skip, y - ystart, sf::Color(iters * 6, iters * 8, iters * 16));
		}
	}
	puts("done");
	return;

}
_Threads thread_Draw(int divisions, int x_start, int y_start, double scale, sf::Image& window) {//returns a vector of threads
	_Threads threads;
	int partitions = 800 / divisions;
	std::cout << partitions << " is divisions\n";
	for (int x = 0; x < divisions; x++) {
		//call calculator function
		threads.emplace_back(_th_calc, x_start +(partitions)*x, (partitions)*x, y_start, partitions, scale, std::ref(window));//emplace need a referance wrapper for somereason 
	}
	return threads;
};

int main()
{
	sf::Event event;
	sf::RenderWindow window({ 800,800 }, "mandelbrot");


	sf::Image image;
	image.create(window.getSize().x, window.getSize().y, sf::Color::Black);
	//mapvals(-400, -400, .1, image);
	
	for (auto& th : thread_Draw(4, -400, -400, .1, image)) {
		if (th.joinable()) {
			th.join(); //wait for threads to exit before continuing. without this it crashes.
		}
		th.~thread();
	}

	sf::Texture texture;
	texture.loadFromImage(image);
	sf::Sprite screen(texture);



	while (window.isOpen())
	{


		while (window.pollEvent(event)) {
			if (event.type == event.Closed) {
				window.close();
				exit(0);
			}


			
			
		}
		window.draw(screen);
		window.display();
	}
}
