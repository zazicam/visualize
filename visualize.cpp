#include "png_image.hpp"

#include <iostream>
#include <vector>

struct Point {
	double x;
	double y;
	int cluster;
};

rgb color_by_index(int i) {
	if(i==0) return rgb{255,0,0};
	if(i==1) return rgb{0,255,0};
	if(i==2) return rgb{0,0,255};
	if(i==3) return rgb{128,0,0};
	if(i==4) return rgb{0,128,0};
	if(i==5) return rgb{0,0,128};
	if(i==6) return rgb{128,128,0};
	if(i==7) return rgb{0,128,128};
	if(i==8) return rgb{128,0,128};
	return rgb{128,128,128};
}

int main(int argc, char *argv[])
{
	// Make sure that the output filename argument has been provided
	if (argc != 2) {
		fprintf(stderr, "Please specify output .png file\n");
		exit(1);
	}

	// read data
	std::vector<Point> points;	
	char sep; // separator
	Point point;
	while(std::cin>>point.x>>sep>>point.y>>sep>>point.cluster) {
		points.push_back(point);
	}

	// calculate range of data
	Point p = points[0]; 
	double min_x = p.x , min_y = p.y, max_x = p.x, max_y = p.y;
	for(const auto& p : points) {
		if(p.x < min_x) min_x = p.x;
		if(p.y < min_y) min_y = p.y;
		if(p.x > max_x) max_x = p.x;
		if(p.y > max_y) max_y = p.y;
	}
	
	// calculate size of image
	double dx = max_x - min_x;
	double dy = max_y - min_y;

	int h, w;
	if(dx>dy) {
		w = 1000;
		h = static_cast<int>(w * dy/dx);
	}
	else {
		h = 1000;
		w = static_cast<int>(h * dx/dy);
	}

	int field = 50;
	double scale_x = (w-2*field)/dx, scale_y = (h-2*field)/dy;

	// draw and save image
	Image image(w, h, rgb{255,255,255});

	int r = 10; // 'points' radius
	for(const auto& p : points) {

		int x = field + (p.x - min_x) * scale_x;
		int y = field + (p.y - min_y) * scale_y;
		image.circle(x, y, r, color_by_index(p.cluster));
	}

	image.save(argv[1]);

	return 0;
}

