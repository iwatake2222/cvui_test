#ifndef COMMON_H_
#define COMMON_H_

#include <vector>
#include <string>

typedef struct _POINT {
	int x;
	int y;

	_POINT(int _x = 0, int _y = 0)
		: x(_x)
		, y(_y)
	{}

} POINT;


typedef struct {
	POINT p[4];
} RECTANGLE;


#endif	/* AREA_CONTAINER_H_ */


