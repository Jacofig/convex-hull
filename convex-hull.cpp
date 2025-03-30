#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <ctime>
using namespace std;
template <typename T>
struct DynamicArray {
    T* data;
    int size;
    int capacity;

    DynamicArray(int initialCapacity = 10) : size(0), capacity(initialCapacity) {
        data = new T[capacity];
    }

    void push_back(const T& value) {
        if (size == capacity) {
            capacity *= 2;
            T* newData = new T[capacity];
            for (int i = 0; i < size; ++i) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
        }
        data[size++] = value;
    }

    ~DynamicArray() {
        delete[] data;
    }

    T& operator[](int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this == &other) return *this;
        delete[] data;

        data = other.data;
        size = other.size;
        capacity = other.capacity;

        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
        return *this;
    }

    std::string toString() const {
        std::ostringstream oss;

        for (int i = 0; i < size; ++i) {

            oss << data[i] << " ";
        }

        return oss.str();
    }
};
struct Point {
    float x, y;
    int index;
};

std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "index: " << point.index << "(" << point.x << ", " << point.y << ")";
    return os;
}

Point p0;

Point secondToLast(const DynamicArray<Point>& points) {
    if (points.size < 2) {
        throw std::logic_error("Too few points to determine second to last");
    }
    return points[points.size - 2];
}

void swap(Point& p1, Point& p2) {
    Point temp = p1;
    p1 = p2;
    p2 = temp;
}

double distSq(Point p1, Point p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

bool comparePoints(const Point& p1, const Point& p2) {

    //double crossProduct = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
    double crossProduct = (-1) * ((p1.y - p0.y) * (p2.x - p0.x) - (p2.y - p0.y) * (p1.x - p0.x));

    if (crossProduct > 0) return true;
    if (crossProduct < 0) return false;

    return distSq(p0, p1) < distSq(p0, p2);
}

double computeCrossProduct(const Point& p1, const Point& p2) {
    return (-1) * ((p1.y - p0.y) * (p2.x - p0.x) - (p2.y - p0.y) * (p1.x - p0.x));
}

int partition(Point* points, int low, int high) {
    Point pivot = points[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {

        if (comparePoints(points[j], pivot)) {
            i++;
            swap(points[i], points[j]);
        }
    }
    swap(points[i + 1], points[high]);
    return i + 1;
}

void quicksort(Point* points, int low, int high) {
    if (low < high) {
        int pi = partition(points, low, high);
        quicksort(points, low, pi - 1);
        quicksort(points, pi + 1, high);
    }
}
clock_t t1, t2, t3, t4;
DynamicArray<int> grahamScan(Point** points, int n) {
    DynamicArray<Point> hull;
    DynamicArray<int> hullIndexes;

    if (n < 3) return hullIndexes;

    int min = 0;
    for (int i = 1; i < n; i++) {
        if (((*points)[i].y < (*points)[min].y) || ((*points)[i].y == (*points)[min].y && (*points)[i].x < (*points)[min].x)) {
            min = i;
        }
    }
    swap((*points)[0], (*points)[min]);
    p0 = (*points)[0];

    t1 = clock();
    quicksort(*points, 1, n - 1);
    t2 = clock();
    bool inOneLine = true;
    for (int i = 2; i < n; i++) {
        double crossProduct = computeCrossProduct((*points)[i], (*points)[1]);
        if (crossProduct != 0) {
            inOneLine = false;
            break;
        }
    }

    if (inOneLine) {

        hullIndexes.push_back((*points)[0].index);
        hullIndexes.push_back((*points)[n - 1].index);
        return hullIndexes;
    }

    int m = 1;
    for (int i = 1; i < n; i++) {
        while (i < n - 1 && computeCrossProduct((*points)[i + 1], (*points)[i]) == 0) {
            i++;
        }
        (*points)[m] = (*points)[i];
        m++;
    }

    if (m < 3) return hullIndexes;

    DynamicArray<Point> S;
    //3
    S.push_back((*points)[0]);
    S.push_back((*points)[1]);

    t3 = clock();
    //4
    for (int i = 2; i < m; i++) {
        while (S.size > 1) {
            Point last = S[S.size - 1];
            Point secondToLast = S[S.size - 2];
            double crossProduct = (last.x - secondToLast.x) * ((*points)[i].y - secondToLast.y) -
                (last.y - secondToLast.y) * ((*points)[i].x - secondToLast.x);
            if (crossProduct > 0) break;
            S.size--;
        }
        S.push_back((*points)[i]);
    }
    t4 = clock();
    for (int i = 0; i < S.size; i++) {
        hullIndexes.push_back(S[i].index);
    }
    /*for (int i = 0; i < hullIndexes.size; i++)
    {
        cout << S[i].x << " " << S[i].y << endl;
    }*/
    return hullIndexes;
}


Point* readFile(const std::string& filename, int& n) {
    std::ifstream file(filename);
    file >> n;
    Point* points = new Point[n];
    for (int i = 0; i < n; i++) {
        file >> points[i].x >> points[i].y;
        points[i].index = i;
    }
    return points;
}

int main1(string fname) {
    int n;
    string filename = fname;
    Point* points = readFile(filename, n);
    DynamicArray<int> test = grahamScan(&points, n);
    double sortTime = (t2 - t1) / (double)CLOCKS_PER_SEC;
    double mainLoopTime = (t4 - t3) / (double)CLOCKS_PER_SEC;
    std::cout << "Number of points: " << test.size << endl;
    std::cout << "Indexes: " << test.toString() << std::endl;
    std::cout << "Time of sorting: " << sortTime << "s " << std::endl;
    std::cout << "Time of main loop: " << mainLoopTime << "s " << std::endl;

    delete[] points;
    return 0;
}


int main() {
    main1("points1.txt");
    std::cout << "////////////////////////////////////////////////////" << endl;
    main1("points2.txt");
    std::cout << "////////////////////////////////////////////////////" << endl;
    main1("points3.txt");
    std::cout << "////////////////////////////////////////////////////" << endl;
    main1("points4.txt");
    std::cout << "////////////////////////////////////////////////////" << endl;
    main1("points5.txt");
    std::cout << "////////////////////////////////////////////////////" << endl;
    return 0;
}