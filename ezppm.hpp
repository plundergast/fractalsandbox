//ezppm.hpp - Jamie Pendergast
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

namespace ezppm {
	struct Color {
		int r, g, b;

		Color& operator + (Color& rhs);
		Color& operator - (Color& rhs);
		Color& operator * (Color& rhs);
		Color& operator / (Color& rhs);
		Color ();
		Color (int r, int g, int b);
		void remap ();
	};

	namespace Colors {
		static const Color
			Black (0,0,0),
			White(255,255,255),
			Grey(128,128,128),
			Red (255, 0, 0),
			Green (0, 255, 0),
			Blue (0, 0, 255),
			Yellow (255, 255, 0),
			Teal (0, 255, 255),
			Purple (255, 0, 255),
			Orange (128, 128, 0),
			Rose (230, 0, 120),
			Cyan (0, 128, 255);	
	};

	class draw_cmd {
	public:
		virtual Color operator()(int x, int y) = 0;
	};

	class Image {
	private:
		Color* colors_;
		int width_;
		int height_;
	public:
		// get raw color data
		Color* raw () const;

		// convert 2d coord to pixel index
		inline int index (int x, int y);

		// get width and height
		int width () const;
		int height () const;

		// resize image, note: this deletes the old array
		void size (int width, int height);

		// draws a pixel
		void put (Color color, int x, int y);
		
		//fills from x0, y0 to x1, y1
		void fill (Color color, int x0, int y0, int x1, int y1);

		void fill_expr (draw_cmd& cmd, int x0, int y0, int x1, int y1);

		// wraps ints inbetween 0 and width or height
		void wrap (int& x, int& y);

		// sample color at pixel coords
		Color sample (int x, int y);

		//write the color data to file, returns if successful
		bool write (std::string name);

		Image (int width, int height, Color fill = Colors::Black);
		Image (Image&& rhs);

		//soon
		Image (const std::string& directory) = delete;
		~Image ();

		//disallows
		Image () = delete;
		Image& operator = (Image& rhs) = default;
		Image (const Image&) = delete;
	};
}