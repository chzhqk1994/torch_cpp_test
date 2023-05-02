#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace std;


typedef struct Point {
    int x;
    int y;
} Point;

vector<Point> points;
vector<Point> findConvexHull();
double outProduct(Point p1, Point p2, Point p3);

int main() {
    freopen("input.txt", "rt", stdin);

    int N;
    vector<Point> convex;

    scanf("%d", &N);
    for (int i = 0; i < N; i++) {
        Point point;
        scanf("%d %d", &point.x, &point.y);
        points.push_back(point);
    }

    convex = findConvexHull();

   for (int i = 0; i < convex.size(); i++)
        printf("%d, %d \n", convex[i].x, convex[i].y);
    puts("");

    // get Convexhull Area
    double sum = 0;
    for (int i = 1; i < convex.size() - 1; i++)
        sum += (double)(outProduct(convex[0], convex[i], convex[i + 1]) / 2.0);

    printf("%.2f", sum);
    return 0;
}

bool cmp(Point p1, Point p2) {
    if (p1.y == p2.y) return p1.x < p2.x;
    else return p1.y < p2.y;
}

double outProduct(Point p1, Point p2, Point p3) {
    return (p2.x * p3.y - p2.x * p1.y - p1.x * p3.y) - (p3.x * p2.y - p1.x * p2.y - p3.x * p1.y);
}

bool ccw(Point p1, Point p2) {
    float value = outProduct(points[0], p1, p2);
    if (value == 0) return p1.x + p1.y < p2.x + p2.y;
    else return value > 0;
}

vector<Point> findConvexHull() {
    vector<Point> valid_points;

    sort(points.begin(), points.end(), cmp);
    sort(points.begin() + 1, points.end(), ccw);

    valid_points.push_back(points[0]);
    valid_points.push_back(points[1]);

    Point p1, p2;
    for (int i = 2; i < points.size(); i++) {
        while (valid_points.size() >= 2) {
            p2 = valid_points.back();
            valid_points.pop_back();
            p1 = valid_points.back();

            if (outProduct(p1, p2, points[i]) > 0) {
                valid_points.push_back(p2);
                break;
            }
        }
        valid_points.push_back(points[i]);
    }

    return valid_points;
}