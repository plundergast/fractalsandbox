#include "ezppm.hpp"

using namespace ezppm;

Color& Color::operator + (Color& rhs) {
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}

Color& Color::operator - (Color& rhs) {
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	return *this;
}

Color& Color::operator * (Color& rhs) {
	r *= rhs.r;
	g *= rhs.g;
	b *= rhs.b;
	return *this;
}

Color& Color::operator/(Color & rhs)
{
	r /= rhs.r;
	g /= rhs.g;
	b /= rhs.b;
	return *this;
}

void Color::remap () {
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
}

Color::Color (int r, int g, int b) : r{ r }, g{ g }, b{ b }{
}

Color::Color () {
	r = g = b = 0;
}

Color* Image::raw () const {
	return colors_;
}

inline int Image::index (int x, int y)
{
	return x * width_ + y;
}

int Image::width () const {
	return width_;
}

int Image::height () const {
	return height_;
}

void Image::size (int width, int height)  {
	width_ = width;
	height_ = height;
	delete[] colors_;
	colors_ = new Color[width * height];
}

void Image::fill (Color color, int x0, int y0, int x1, int y1) {
	if (x0 > x1) std::swap (x0, x1);
	if (y0 > y1) std::swap (y0, y1);
	for (int x = x0; x < x1; ++x) {
		for (int y = y0; y < y1; ++y) {
			put (color, x, y);
		}
	}
}

void Image::fill_expr (draw_cmd& cmd, int x0, int y0, int x1, int y1)
{
	if (x0 > x1) std::swap (x0, x1);
	if (y0 > y1) std::swap (y0, y1);
	for (int x = x0; x < x1; ++x) {
		for (int y = y0; y < y1; ++y) {
			put (cmd(x,y), x, y);
		}
	}
}

void Image::put (Color color, int x, int y) {
	if (x >= 0 && y >= 0 && x <= width_ && y <= height_) {
		colors_[index(x,y)] = color;
	}
}

void Image::wrap (int& x, int& y) {
	x = x % width_;
	y = y % height_;
}

Color Image::sample (int x, int y) {
	return colors_[index(x,y)];
}

bool Image::write (std::string name) {
	std::ofstream file (name + ".ppm");
	if (!file.is_open ()) {
		return false;
	}
	file << "P3\n" << width_ << " " << height_ << "\n255\n";
	for (int y = height_; y > 0; y--) {
		for (int x = 0; x < width_; x++) {
			Color s = sample (x, y);
			file << s.r << " " << s.g << " " << s.b << '\n';
		}
	}
	file.close ();
	return true;
}

Image::Image (int width, int height, Color fill) : width_{ width }, height_{ height } {
	colors_ = new Color[width * height];
	this->fill (fill,0, 0, width, height);
}

Image::Image (Image&& rhs) {
	delete[] colors_;
	width_ = rhs.width ();
	height_ = rhs.height ();
	colors_ = rhs.raw ();
}

/*
ezppm::Image::Image (const std::string & directory) 
{
	//not working
	std::ifstream file (directory);
	if (!file.is_open ())
	{
		return;
	}

	std::string line;
	int lineNo = 0;
	bool condition = false;

	int X = 0;
	int Y = 0;
	while (file >> line)
	{
		if (lineNo == 1)
		{
			int width = 0;
			int height = 0;
			sscanf_s (line.c_str (), "%d %d", &width, &height);
			condition = true;	
			colors_ = new Color[width * height];
		}
		else if (lineNo > 3)
		{
			if (condition)
			{
				int red = 0;
				int green = 0;
				int blue = 0;
				sscanf_s (line.c_str(), "%d %d %d", &red, &green, &blue);
				colors_[index (X, Y)] = Color (red, green, blue);
				if (X >= width_)
				{
					X = 0;
					Y++;
				} 
				else 
					X++;
			}
			else
			{
				return;
			}
			
		}
		lineNo++;
	}

	file.close ();
}
*/

Image::~Image () {
	if (colors_ != nullptr) {
		delete[] colors_;
	}
}