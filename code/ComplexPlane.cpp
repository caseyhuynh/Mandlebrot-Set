#include "ComplexPlane.h"

using namespace std;
using namespace sf;


ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
	m_pixel_size = { pixelWidth, pixelHeight };

	m_aspectRatio = pixelHeight / (pixelWidth * 1.0);
	m_plane_center = { 0,0 };
	m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };
	m_zoomCount = 0;
	m_State = State::CALCULATING;

	m_vArray.setPrimitiveType(Points);
	m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(m_vArray);
}

void ComplexPlane::updateRender() {
	int pixelHeight = m_pixel_size.y;
	int pixelWidth = m_pixel_size.x;

	if (m_State == State::CALCULATING) {
		int numThreads = (thread::hardware_concurrency() == 0) ? 4 : thread::hardware_concurrency(); // Number of threads
		vector<thread> threads;
		int rowsPerThread = pixelHeight / numThreads;

		// Lambda function for thread tasks
		auto computeChunk = [this, pixelWidth](int startRow, int endRow) {
			for (int i = startRow; i < endRow; ++i) {
				for (int j = 0; j < pixelWidth; ++j) {
					Vector2i local = { j, i };
					Vector2f coords = mapPixelToCoords(local);
					size_t count = countIterations(coords);
					Uint8 r, g, b;
					iterationsToRGB(count, r, g, b);
					m_vArray[j + i * pixelWidth].position = { static_cast<float>(j), static_cast<float>(i) };
					m_vArray[j + i * pixelWidth].color = { r, g, b };
				}
			}
			};

		// Launch threads for each chunk
		for (int t = 0; t < numThreads; ++t) {
			int startRow = t * rowsPerThread;
			int endRow = (t == numThreads - 1) ? pixelHeight : startRow + rowsPerThread;
			threads.emplace_back(computeChunk, startRow, endRow);
		}

		// Wait for threads to finish
		for (auto& thread : threads) {
			thread.join();
		}

		m_State = State::DISPLAYING; // Set state after computation
	}
}


void ComplexPlane::zoomIn()
{
	m_zoomCount++;
	float xSize = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
	float ySize = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
	m_plane_size = { xSize, ySize };
	m_State = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
	m_zoomCount--;
	float xSize = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
	float ySize = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));
	m_plane_size = { xSize, ySize };
	m_State = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
	m_plane_center = mapPixelToCoords(mousePixel);
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
	stringstream ss;
	ss << "Mandlebrot Set" << "\n";
	ss << "Cursor: (" << fixed << m_mouseLocation.x << ", " << fixed << m_mouseLocation.y << ")\n";
	ss << "Center: (" << fixed << m_plane_center.x << ", " << fixed << m_plane_center.y << ")\n";
	ss << "Left Click zoom in" << "\n";
	ss << "Right click zoom out" << "\n";

	text.setString(ss.str());
	text.setCharacterSize(20);
	FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	text.setPosition(150, 75);

}

size_t ComplexPlane::countIterations(Vector2f coord)
{
	complex<double> z(coord.x, coord.y);
	complex<double> c = z;
	size_t i = 0;
	while (abs(z) < 2.0 && i < 64)
	{
		z = z * z + c;

		i++;
	}

	return i;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	if (count == MAX_ITER)
	{
		r = 0; g = 0; b = 0; // black
	}
	else if (0 <= count && count <= 4)
	{
		r = 30; g = 0; b = 255; // purple
	}
	else if (5 <= count && count <= 8) // 
	{
		r = 234; g = 0; b = 255; // pink
	}
	else if (9 <= count && count <= 12)
	{
		r = 255; g = 0; b = 106; // reddish pink
	}
	else if (13 <= count && count <= 16) //turquoise
	{
		r = 0; g = 255; b = 251;
	}
	else if (17 <= count && count <= 20) // Green
	{
		r = 0; g = 255; b = 0;
	}
	else if (21 <= count && count <= 24)
	{
		r = 0; g = 255; b = 136;
	}
	else if (25 <= count && count <= 28)
	{
		r = 64; g = 255; b = 0;
	}
	else if (26 <= count && count <= 32)
	{
		r = 255; g = 187; b = 0;
	}
	else if (33 <= count && count <= 36)
	{
		r = 0; g = 255; b = 0;
	}
	else if (37 <= count && count <= 40)
	{
		r = 255; g = 145; b = 0;
	}
	else if (41 <= count && count <= 44)
	{
		r = 255; g = 55; b = 0;

	}
	else if (45 <= count && count <= 48) //Yellow 
	{
		r = 255; g = 255; b = 0;
	}
	else if (49 <= count && count <= 52) //red
	{
		r = 255; g = 0; b = 64;
	}
	else if (53 <= count && count <= 56)
	{
		r = 255; g = 255; b = 0;
	}
	else if (57 <= count && count <= 60)
	{
		r = 255; g = 0; b = 157;
	}
	else if (61 <= count && count <= 63)
	{
		r = 255; g = 0; b = 0;
	}
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
	float x = ((mousePixel.x * 1.0) / 1920) * (m_plane_size.x) + (m_plane_center.x - m_plane_size.x / 2.0);
	float y = ((mousePixel.y * 1.0) / -1080) * (-m_plane_size.y) + (m_plane_center.y - m_plane_size.y / 2.0);

	return Vector2f(x, y);
}
