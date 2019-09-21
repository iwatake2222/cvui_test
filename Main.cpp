#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include "Common.h"
#include "AreaContainer.h"


/***** Definitions *****/
#define FILENAME    "areas.csv"
#define WINDOW_MAIN "main window"
#define WINDOW_CONTROL "control window"
static const cv::Scalar COLOR_RECT          = cv::Scalar(  0, 255,   0);
static const cv::Scalar COLOR_RECT_SELECTED = cv::Scalar(  0, 255, 255);
static const cv::Scalar COLOR_RECT_DRAWING  = cv::Scalar(160, 200, 160);

/***** Global variables *****/
static bool s_isRunning = true;
static POINT s_mouse;
static int s_selectedRectIndex = -1;
static int  s_scale = 1;
static cv::VideoCapture s_cap;
static int s_videoFramePos = 0;
static bool s_videoPositionChanged = false;


/***** Functions *****/
static void processCtrl(cv::Mat &frame)
{
	cvui::context(WINDOW_CONTROL);

	cvui::beginColumn(frame, 10, 10, -1, -1, 10);
		cvui::beginRow(-1, -1, 10);
			if (cvui::button("SAVE")) {
				AreaContainer::getInstance()->save(FILENAME);
			}
			if (cvui::button("LOAD")) {
				AreaContainer::getInstance()->load(FILENAME);
			}
		cvui::endRow();
		cvui::beginRow( -1, -1, 10);
			if (cvui::button("<")) {
				AreaContainer::getInstance()->clearCurrent();
				if (AreaContainer::getInstance()->getRectNum() > 0) {
					if (s_selectedRectIndex == -1) {
						s_selectedRectIndex = AreaContainer::getInstance()->getRectNum() - 1;
					} else {
						if (--s_selectedRectIndex == -1) s_selectedRectIndex = AreaContainer::getInstance()->getRectNum() - 1;
					}
				}
			}
			if (cvui::button(">")) {
				AreaContainer::getInstance()->clearCurrent();
				if (AreaContainer::getInstance()->getRectNum() > 0) {
					if (s_selectedRectIndex == -1) {
						s_selectedRectIndex = 0;
					} else {
						s_selectedRectIndex++;
						s_selectedRectIndex %= AreaContainer::getInstance()->getRectNum();
					}
				}
			}
			if (cvui::button("DELETE")) {
				AreaContainer::getInstance()->remove(s_selectedRectIndex);
				s_selectedRectIndex = -1;
			}
		cvui::endRow();
		cvui::beginRow(-1, -1, 10);
			if (cvui::button("CLEAR")) {
				AreaContainer::getInstance()->clearAll();
				s_selectedRectIndex = -1;
			}
		cvui::endRow();
		cvui::beginRow(-1, -1, 10);
		if (s_cap.isOpened()) {
			
			int previousVideoFramePos = s_videoFramePos;
			int totalCount = (int)s_cap.get(cv::CAP_PROP_FRAME_COUNT);
			cvui::trackbar(150, &s_videoFramePos, 0, totalCount - 1);
			if (previousVideoFramePos != s_videoFramePos) {
				s_cap.set(cv::CAP_PROP_POS_FRAMES, s_videoFramePos);
				s_videoPositionChanged = true;
			}
		}
		cvui::endRow();
		cvui::beginRow(-1, -1, 10);
			cvui::printf("(%d,%d)", s_mouse.x * s_scale, s_mouse.y * s_scale);
		cvui::endRow();
		cvui::beginRow(-1, -1, 10);
			if (s_scale == 1) {
				if (cvui::button("SCALE = 1/1")) {
					s_scale = 2;
					AreaContainer::getInstance()->setScale(s_scale);
				}
			} else {
				if (cvui::button("SCALE = 1/2")) {
					s_scale = 1;
					AreaContainer::getInstance()->setScale(s_scale);
				}
			}
		cvui::endRow();
	cvui::endColumn();

	cvui::imshow(WINDOW_CONTROL, frame);
}



static void processMain(cv::Mat &frame)
{
	cvui::context(WINDOW_MAIN);
	s_mouse.x = cvui::mouse(WINDOW_MAIN).x;
	s_mouse.y = cvui::mouse(WINDOW_MAIN).y;
	if (cvui::mouse(WINDOW_MAIN, cvui::LEFT_BUTTON, cvui::CLICK)) {
		AreaContainer::getInstance()->addPoint(s_mouse);
		s_selectedRectIndex = -1;
	} else 	if (cvui::mouse(WINDOW_MAIN, cvui::RIGHT_BUTTON, cvui::CLICK)) {
		AreaContainer::getInstance()->revertPoint();
		s_selectedRectIndex = -1;
	}

	std::vector<RECTANGLE> rects;
	RECTANGLE drawingRect;
	int drawingPointIndex;
	AreaContainer::getInstance()->getRectangle(rects, drawingRect, drawingPointIndex);
	for (int i = 0; i < rects.size(); ++i) {
		RECTANGLE r = rects[i];
		cv::Scalar color = COLOR_RECT;
		if (s_selectedRectIndex == i) color = COLOR_RECT_SELECTED;
		for (int j = 0; j < 4; j++) {
			int indexCurrent = j;
			for (int k = 0; k < 4; k++) {
				int indexNext = (j + k) % 4;
				cv::line(frame, cv::Point(r.p[indexCurrent].x, r.p[indexCurrent].y), cv::Point(r.p[indexNext].x, r.p[indexNext].y), color, 2);
			}
		}
	}

	if (drawingPointIndex != 0) {
		for (int i = 0; i < drawingPointIndex - 1; i++) {
			int indexCurrent = i;
			int indexNext = (i + 1) % drawingPointIndex;
			cv::line(frame, cv::Point(drawingRect.p[indexCurrent].x, drawingRect.p[indexCurrent].y), cv::Point(drawingRect.p[indexNext].x, drawingRect.p[indexNext].y), COLOR_RECT_DRAWING, 2);
		}
		cv::line(frame, cv::Point(drawingRect.p[drawingPointIndex - 1].x, drawingRect.p[drawingPointIndex - 1].y), cv::Point(s_mouse.x, s_mouse.y), COLOR_RECT_DRAWING, 2);
	}

	cvui::imshow(WINDOW_MAIN, frame);
}


int main(int argc, char* argv[])
{
	cv::Mat frameMainBack = cv::Mat(1080, 1920, CV_8UC3, cv::Scalar(50, 50, 50));
	if (argc == 2) {
		char *imageFileName = argv[1];
		char* ext = strrchr(imageFileName, '.');
		if (ext == NULL || *(++ext) == '\0') {
			printf("error reading %s\n", imageFileName);
		} else if ((strncmp(ext, "jpg", 3) == 0) || (strncmp(ext, "bmp", 3) == 0) || (strncmp(ext, "png", 3) == 0)
			|| (strncmp(ext, "JPG", 3) == 0) || (strncmp(ext, "BMP", 3) == 0) || (strncmp(ext, "PNG", 3) == 0)) {
			frameMainBack = cv::imread(imageFileName);
			if (frameMainBack.empty()) {
				printf("error reading %s\n", imageFileName);
			}
		} else if ((strncmp(ext, "mp4", 3) == 0) || (strncmp(ext, "avi", 3) == 0)
			|| (strncmp(ext, "MP4", 3) == 0) || (strncmp(ext, "AVI", 3) == 0)) {
			s_cap.open(imageFileName);
			if (!s_cap.isOpened()) {
				printf("error reading %s\n", imageFileName);
			} else {
				s_cap.read(frameMainBack);
			}
		}
	}

	AreaContainer::getInstance()->load(FILENAME);
	cvui::init(WINDOW_MAIN);
	cvui::init(WINDOW_CONTROL);
	
	while (s_isRunning && cv::getWindowProperty(WINDOW_MAIN, 0) >= 0 && cv::getWindowProperty(WINDOW_CONTROL, 0) >= 0) {
		if (s_videoPositionChanged) {
			s_cap.read(frameMainBack);
			s_videoPositionChanged = false;
			if (frameMainBack.empty()) {
				printf("seek error\n");
				s_videoFramePos = 0;
				s_cap.set(cv::CAP_PROP_POS_FRAMES, s_videoFramePos);
				s_cap.read(frameMainBack);
			}
		}

		cv::Mat frameMain = frameMainBack.clone();
		cv::Mat frameCtrl = cv::Mat(250, 200, CV_8UC3, cv::Scalar(50, 50, 50));

		cv::resize(frameMain, frameMain, cv::Size(frameMain.cols / s_scale, frameMain.rows / s_scale), 0, 0);

		processMain(frameMain);
		processCtrl(frameCtrl);
	
		cv::waitKey(1);
	}
	return 0;
}
