#ifndef AREA_CONTAINER_H_
#define AREA_CONTAINER_H_

#include "Common.h"

class AreaContainer
{
private:
	static AreaContainer s_areaContainer;
	std::vector<RECTANGLE> m_rects;
	RECTANGLE m_drawingRect;
	int m_drawingPointIndex;
	int m_scale;



public:


private:
	AreaContainer();

public:
	static AreaContainer* getInstance();
	void load(const char *filename);
	void save(const char *filename);
	void addPoint(const POINT &p);
	void revertPoint();
	void clearCurrent();
	void clearAll();
	void remove(int index);
	void getRectangle(std::vector<RECTANGLE> &rects, RECTANGLE &drawingRect, int &drawingPointIndex);
	int  getRectNum();
	void setScale(int scale);
};

#endif	/* AREA_CONTAINER_H_ */
