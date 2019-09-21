#include <stdio.h>
#include <stdlib.h>
#include "AreaContainer.h"

AreaContainer AreaContainer::s_areaContainer;

AreaContainer::AreaContainer()
{
	clearAll();
	m_scale = 1;
}

AreaContainer* AreaContainer::getInstance()
{
	return &s_areaContainer;
}

void AreaContainer::load(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("cannot open %s\n", filename);
		return;
	}

	clearAll();
	int ret = 0;
	do {
		RECTANGLE rect;
		ret = fscanf(fp, "%d, %d, %d, %d, %d, %d, %d, %d"
			, &rect.p[0].x, &rect.p[0].y, &rect.p[1].x, &rect.p[1].y, &rect.p[2].x, &rect.p[2].y, &rect.p[3].x, &rect.p[3].y);
		if (ret > 0) m_rects.push_back(rect);
	} while (ret > 0);

}

void AreaContainer::save(const char *filename)
{
	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("cannot open %s\n", filename);
		return;
	}

	for (auto rect : m_rects) {
		fprintf(fp, "%d, %d, %d, %d, %d, %d, %d, %d\n", rect.p[0].x, rect.p[0].y, rect.p[1].x, rect.p[1].y, rect.p[2].x, rect.p[2].y, rect.p[3].x, rect.p[3].y);
	}

	fclose(fp);
}

void AreaContainer::addPoint(const POINT &p)
{
	m_drawingRect.p[m_drawingPointIndex].x = p.x * m_scale;
	m_drawingRect.p[m_drawingPointIndex].y = p.y * m_scale;
	m_drawingPointIndex++;
	if (m_drawingPointIndex == 4) {
		m_rects.push_back(m_drawingRect);
		clearCurrent();
	}

}

void AreaContainer::revertPoint()
{
	if (m_drawingPointIndex > 0) {
		m_drawingPointIndex--;
	} else if (m_rects.size() > 0) {
		m_rects.pop_back();
	}
}

void AreaContainer::clearCurrent()
{
	m_drawingPointIndex = 0;
	for (int i = 0; i < 4; i++) m_drawingRect.p[i].x = -1;
}

void AreaContainer::clearAll()
{
	m_rects.clear();
	clearCurrent();
}

void AreaContainer::remove(int index)
{
	if (index < 0) return;
	if (index < m_rects.size()) {
		m_rects.erase(m_rects.begin() + index);
	} else {
		printf("error at remove\n");
	}
}

void AreaContainer::getRectangle(std::vector<RECTANGLE> &rects, RECTANGLE &drawingRect, int &drawingPointIndex)
{
	rects = m_rects;
	drawingRect = m_drawingRect;
	drawingPointIndex = m_drawingPointIndex;

	// adjust scale
	for (auto &rect : rects) {
		for (int i = 0; i < 4; i++) {
			rect.p[i].x /= m_scale;
			rect.p[i].y /= m_scale;
		}
	}
	for (int i = 0; i < 4; i++) {
		drawingRect.p[i].x /= m_scale;
		drawingRect.p[i].y /= m_scale;
	}

}

int AreaContainer::getRectNum()
{
	return m_rects.size();
}


void AreaContainer::setScale(int scale)
{
	m_scale = scale;
}

