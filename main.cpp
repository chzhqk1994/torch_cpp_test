#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace cv::dnn;

const float INPUT_WIDTH = 640;
const float INPUT_HEIGHT = 640;

const float SCORE_THRESHOLD = 0.5;
const float CONFIDENCE_THRESHOLD = 0.45;
const float NMS_THRESHOLD = 0.45;

const float FONT_SCALE = 0.7;
const int FONT_FACE = FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;

Scalar BLACK = Scalar(0, 0, 0);
Scalar RED = Scalar(0, 0, 255);
Scalar GREEN = Scalar(0, 255, 0);
Scalar BLUE = Scalar(255, 0, 0);

vector<Mat> model_input(Mat input_image, Net model) {
    Mat blob;

    // Net에 입력되는 형태는 blob 형태로 변경 해줘야 한다
    // blob : opencv 에서 Mat 타입의 4차원 텐서 (NCHW)
    blobFromImage(input_image, blob, 1/255, Size(INPUT_WIDTH, INPUT_HEIGHT), Scalar(), true, false);

    // model 에 blob 형태의 데이터를 넣어줌
    model.setInput(blob);

    // blob 을 받은 model 을 실행 시켜줌(forward propagation)
    vector<Mat> outputs;
    model.forward(outputs, model.getUnconnectedOutLayersNames());

    return outputs;
}

int main(int, char **) {
    vector<string> class_list;
    String string;
    fstream ifs("coco_labels.txt");
    while (getline(ifs, string))
        class_list.push_back(string);

    String image_path = "lenna.png";
    Mat image = imread(image_path);

    // Load model
    Net model;
    model = readNet("yolov5s.onnx");

    vector<Mat> detections;
    detections = model_input(image, model);  // inference


    return 0;
}
