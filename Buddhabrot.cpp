#include <iostream>
#include <complex>
#include <random>
#include <chrono>
#include "ezppm.hpp"

using namespace std;

class Expression : public ezppm::draw_cmd
{
public:

	void Generate (short**& heatmap, short& mostOrbits, short iterations)
	{
		typedef complex<float> Complex;
		Complex Minimum (-2, -2);
		Complex Maximum (2 , 2);

		mt19937 random;
		random.seed (chrono::high_resolution_clock::now ().time_since_epoch ().count ());

		uniform_real_distribution<float> RealDist (Minimum.real (), Maximum.real ());
		uniform_real_distribution<float> ImagDist (Minimum.imag (), Maximum.imag());

		for (int sample = 0; sample < 2048 * 2048; ++sample)
		{

			Complex C{ RealDist (random), ImagDist (random) };
			

			if(C.real() * C.real() + C.imag() * C.imag() < 4.0f)
			{
				Complex Z (C);
				for (int iteration = 0; iteration < iterations; ++iteration)
				{

					Z = Z * Z + C;
					int x = static_cast<int>((Z.real () - Minimum.real ()) * (height / (Maximum.real () - Minimum.real ())));
					int y = static_cast<int>((Z.imag () - Minimum.imag ()) * (width / (Maximum.imag () - Minimum.imag ())));

					if (Z.real () >= Minimum.real () && Z.real () <= Maximum.real () && Z.imag () >= Minimum.imag () && Z.imag () <= Maximum.imag ())
					{

						if (iteration <= iterations - 1)
						{
							++heatmap[x][y];
							if (heatmap[x][y] > mostOrbits)
							{
								mostOrbits = heatmap[x][y];
							}
						}
					}
				}
			}
		}
	}

	float GetAverage (short**& buddhabrotHeatmap, int x, int y, short most)
	{
		float ratio{ static_cast<float>(buddhabrotHeatmap[x][y]) / most };

		if (x > 0 && x < width - 1 && y > 0 && y < height - 1)
		{
			ratio += static_cast<float>(buddhabrotHeatmap[x + 1][y]) / most; ratio *= 1.4;
			ratio += static_cast<float>(buddhabrotHeatmap[x - 1][y]) / most; ratio *= 1.4;
			ratio += static_cast<float>(buddhabrotHeatmap[x][y + 1]) / most; ratio *= 1.4;
			ratio += static_cast<float>(buddhabrotHeatmap[x][y - 1]) / most; ratio *= 1.4;

			ratio += static_cast<float>(buddhabrotHeatmap[x + 1][y + 1]) / most; ratio *= 1.31;
			ratio += static_cast<float>(buddhabrotHeatmap[x - 1][y + 1]) / most; ratio *= 1.31;
			ratio += static_cast<float>(buddhabrotHeatmap[x - 1][y + 1]) / most; ratio *= 1.31;
			ratio += static_cast<float>(buddhabrotHeatmap[x - 1][y - 1]) / most; ratio *= 1.31;

			ratio += static_cast<float>(buddhabrotHeatmap[x + 1][y - 1]) / most; ratio *= 1.31;
			ratio += static_cast<float>(buddhabrotHeatmap[x - 1][y - 1]) / most; ratio *= 1.31;
			ratio += static_cast<float>(buddhabrotHeatmap[x - 1][y + 1]) / most; ratio *= 1.31;
			ratio += static_cast<float>(buddhabrotHeatmap[x - 1][y - 1]) / most; ratio *= 1.31;
			ratio /= 13;
		}
		return ratio;
	}

	ezppm::Color operator()(int x, int y)
	{
		float red = GetAverage (buddhabrotHeatmapRed, x, y, mostOrbitsRed) * 255.0f;
		float green = GetAverage (buddhabrotHeatmapGreen, x, y, mostOrbitsGreen) * 255.0f;
		float blue = GetAverage (buddhabrotHeatmapBlue, x, y, mostOrbitsBlue) * 255.0f;
		
	
		return ezppm::Color (red, green, blue);
	}

	Expression (int Width, int Height) :
		width{ Width },
		height{ Height }
	{
		buddhabrotHeatmapRed = new short*[height];
		buddhabrotHeatmapGreen = new short*[height];
		buddhabrotHeatmapBlue = new short*[height];
		for (int y = 0; y < height; ++y)
		{
			buddhabrotHeatmapRed[y] = new short[width] {0};
			buddhabrotHeatmapGreen[y] = new short[width] {0};
			buddhabrotHeatmapBlue[y] = new short[width] {0};
		}
	}
	
	~Expression ()
	{
		for (int y = 0; y < height; ++y)
		{
			delete[] buddhabrotHeatmapRed[y];
			delete[] buddhabrotHeatmapGreen[y];
			delete[] buddhabrotHeatmapBlue[y];
		}

		delete[] buddhabrotHeatmapRed;
		delete[] buddhabrotHeatmapGreen;
		delete[] buddhabrotHeatmapBlue;
	}

	short mostOrbitsRed;
	short mostOrbitsGreen;
	short mostOrbitsBlue;
	int width, height;
	short** buddhabrotHeatmapRed;
	short** buddhabrotHeatmapGreen;
	short** buddhabrotHeatmapBlue;
};


int main ()
{
	ezppm::Image image (2048, 2048);
	Expression buddhabrot (2048, 2048);
	
	buddhabrot.Generate (buddhabrot.buddhabrotHeatmapRed, buddhabrot.mostOrbitsRed, 25);
	buddhabrot.Generate (buddhabrot.buddhabrotHeatmapGreen, buddhabrot.mostOrbitsGreen, 100);
	buddhabrot.Generate (buddhabrot.buddhabrotHeatmapBlue, buddhabrot.mostOrbitsBlue, 40);


	image.fill_expr (buddhabrot, 0, 0, 2048, 2048);
	image.write ("Buddhabrot");
	return 0;
}