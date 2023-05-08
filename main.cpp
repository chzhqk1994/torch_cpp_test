#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace cv::dnn;

const float INPUT_WIDTH = 640;
const float INPUT_HEIGHT = 640;
const int YOLO_NUM_DETECTION = 25200;  // 640 x 640 x 3 기준 25200 개의 grid cell 을 만든다
const int YOLO_DEIMENSION = 85;  // [ X Y W H | Confidence | Class score of 80 classes ]

const float SCORE_THRESHOLD = 0.001;
const float CONFIDENCE_THRESHOLD = 0.001;
const float NMS_THRESHOLD = 0.45;

const float FONT_SCALE = 0.7;
const int FONT_FACE = FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;

Scalar BLACK = Scalar(0, 0, 0);
Scalar RED = Scalar(0, 0, 255);
Scalar GREEN = Scalar(0, 255, 0);
Scalar BLUE = Scalar(255, 0, 0);

vector<Mat> inference(Mat input_image, Net model) {
    Mat blob;

    // Net에 입력되는 형태는 blob 형태로 변경 해줘야 한다
    // blob : opencv 에서 Mat 타입의 4차원 텐서 (NCHW)
    blobFromImage(input_image, blob, 1./255., Size(INPUT_WIDTH, INPUT_HEIGHT), Scalar(), true, false);

    // model 에 blob 형태의 데이터를 넣어줌
    model.setInput(blob);

    // blob 을 받은 model 을 실행 시켜줌(forward propagation)
    vector<Mat> outputs;
    model.forward(outputs, model.getUnconnectedOutLayersNames());

    return outputs;
}

Mat draw_result(Mat image, vector<Mat>& detect_result, const vector<string>& class_lists) {
    vector<int> class_ids;
    vector<float> confidences;
    vector<Rect> bboxes;

    // resizing factor
    float x_scale = image.cols / INPUT_WIDTH;
    float y_scale = image.rows / INPUT_HEIGHT;

    // detect_result 의 각 항목들에 접근하기 위한 포인터, 2차원 벡터의 [0][0] 위치로 초기화
    float *data = (float *)detect_result[0].data;  // ???

    for (int i = 0; i < YOLO_NUM_DETECTION; i++) {
        // class 의 확률 = confidence * class 분류 확률??
        float confidence = data[4];
        if (confidence >= CONFIDENCE_THRESHOLD) {
            float *classes_scores = data + 5;  // 각 row 의 0번째 기준 + 5 부터 모든 class 에 대한 confidence 값들을 가져와서 float 배열을 만듦

            // 1 x 80 Mat 를 만들고 80개의 클래스에 대한 scores 를 저장
            Mat scores(1, class_lists.size(), CV_32FC1, classes_scores);

            Point class_id;
            double max_class_score;
            minMaxLoc(scores, 0, &max_class_score, NULL, &class_id);

            // double max_class_score1 = *max_element(classes_scores, classes_scores + 80);
            // printf("%f\n", max_class_score);

            if (max_class_score > SCORE_THRESHOLD) {
                class_ids.push_back(class_id.x);
                confidences.push_back(confidence);

                // make and save bbod info
                float cx = data[0];
                float cy = data[1];
                float width = data[2];
                float height = data[3];

                // 좌, 상, 너비 높이
                int left = int((cx - 0.5 * width) * x_scale);
                int top = int((cx + 0.5 * width) * x_scale);
                width = int(width * x_scale);
                height = int(height * x_scale);
                // save valid bbox
                bboxes.push_back(Rect(left, top, width, height));
            }
        }
    }

    // Do NMS process and draw bbox
    vector<int> indicies;
    NMSBoxes(bboxes, confidences, CONFIDENCE_THRESHOLD, NMS_THRESHOLD, indicies);
    printf("Num of valid bboxes : %d\n", indicies.size());
    for (int i = 0; i < indicies.size(); i++) {
        int idx = indicies[i];
        Rect bbox = bboxes[i];
        int left = bbox.x;
        int top = bbox.y;
        int width = bbox.width;
        int height = bbox.height;

        rectangle(image, Point(left, top), Point(left + width, top + height), RED, THICKNESS * 3);
        string label = format("%.2f", confidences[i]);
        label = class_lists[class_ids[idx]] + ": " + label;
    }
    rectangle(image, Point(10, 10), Point(20, 20), RED, THICKNESS*2);

    return image;
}

int main() {
    vector<string> class_list;
    String string;
    fstream ifs("coco_labels.txt");
    while (getline(ifs, string))
        class_list.push_back(string);

    String image_path = "/home/qisens/cpp_project/torch_cpp_test/test_image.jpg";
    Mat image = imread(image_path);

    // Load model
    Net model;
    model = readNet("/home/qisens/cpp_project/torch_cpp_test/yolov5s.onnx");

    vector<Mat> detections;
    detections = inference(image, model);  // inference image

    Mat output_image = draw_result(image.clone(), detections, class_list);
    imshow("output", output_image);
    waitKey(0);

    return 0;
}