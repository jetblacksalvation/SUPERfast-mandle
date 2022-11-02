// mandle2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <complex>
#include <SFML/Graphics.hpp>

using _Complex = std::complex<double>;

_Complex ret_mandel(_Complex val1, _Complex val2) {
	
	return pow(val1,2) +val2;
};

void mapvals(int xstart, int ystart, double scale, sf::Image& image) {
	//initialize s_buffer with the number of iterations per pixel
	_Complex z = { 0,0 };
	int iters = 0;
	for (int x = xstart; x < xstart+800 ; x++) {
		for (int y = ystart; y < ystart+800 ; y++) {
			z = { 0,0 };
			for (iters = 0; iters < 1000 and abs(z) < 2; iters++) {
				z = ret_mandel(z, { (double(x) / 800) / scale, (double(y) / 800) / scale });

			}
			
			image.setPixel(x - xstart, y - ystart, sf::Color(iters * 6, iters * 8, iters * 16));
		}
	}
	puts("done");
	return;
	
}



int main()
{
	sf::Event event;
	sf::RenderWindow window({ 800,800 }, "mandelbrot");


	sf::Image image;
	image.create(window.getSize().x, window.getSize().y, sf::Color::Black);
	mapvals(-400, -400, .1, image);
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
