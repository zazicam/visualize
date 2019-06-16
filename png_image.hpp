#include <string>

struct rgb {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class Image {
	int width;
	int height;
	rgb *buffer;
public:
	Image(int w, int h, rgb color);
	void set_pixel(int x, int y, rgb color); 
	void circle(int x, int y, int r, rgb color); 
	int save(const std::string& filename);
	~Image();
};

