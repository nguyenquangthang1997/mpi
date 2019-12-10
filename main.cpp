#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;

typedef  struct {
    int* data;
    int length;
} Vector;

typedef struct ArrayList {
    int* data;
    int length;
    int size;
} ArrayList_t;
#define DEFAULT_SIZE 64

ArrayList_t* listCreate() {
    ArrayList_t *list = (ArrayList_t*) calloc(1, sizeof(ArrayList_t));
    if (!list)
        return NULL;
    list->size = DEFAULT_SIZE;
    list->length = 0;
    if (!(list->data = (int*) calloc(sizeof(int), list->size))) {
        free(list);
        return NULL;
    }
    return list;
}

void listDestroy(ArrayList_t *list) {
    free(list->data);
    free(list);
}

static int _listExpand(ArrayList_t *list, int max) {
    if (max < list->size)
        return 0;
    int new_size = list->size  * 1.5;
    if (new_size < max)
        new_size = max;

    int *t;
    if (!(t = realloc(list->data, new_size * sizeof(int))))
        return -1;
    list->data = t;
    (void) memset(list->data + list->size, 0, (new_size - list->size) * sizeof(int));
    list->size = new_size;
    return 0;
}
int listPutIdx(ArrayList_t *list, int idx, int data) {
    if (_listExpand(list, idx + 1))
        return -1;
    list->data[idx] = data;
    if (list->length <= idx)
        list->length = idx + 1;
    return 0;
}
int listAppend(ArrayList_t *arr, int data) {
    return listPutIdx(arr, arr->length, data);
}

void listClear(ArrayList_t *list){
    list->length = 0;
}





typedef struct {
    int x;
    int y;
} POINT;

//p1-p2-p3 >0 re trai, <0 re phai, = 0 thi k doi
int relation(POINT point1, POINT point2, POINT point3) {
    int temp = (point3.y - point1.y) * (point2.x - point1.x) - (point2.y - point1.y) * (point3.x - point1.x);
    if (temp > 0) return 1; else if (temp < 0) return -1; else return 0;
}

POINT findUpTangent(POINT x, vector<POINT> y) {
    vector<POINT> y_temp;
    int middle = y.size() / 2;
    if (relation(x, y[middle], y[middle - 1]) == 0) {
        return y[middle];
    } else if (relation(x, y[middle], y[middle + 1]) == 0) {
        return y[middle + 1];
    } else if (relation(x, y[middle], y[middle - 1]) < 0 && relation(x, y[middle], y[middle + 1]) > 0) {
        for (int i = 0; i <= middle; i++) {
            y_temp.push_back(y[i]);
        }
        if (y_temp.size() == 2)
            return y_temp[0];
        else return findUpTangent(x, y_temp);
    } else if (relation(x, y[middle], y[middle - 1]) > 0 && relation(x, y[middle], y[middle + 1]) < 0) {
        for (int i = middle; i < y.size(); i++) {
            y_temp.push_back(y[i]);
        }
        if (y_temp.size() == 2)
            return y_temp[1];
        else return findUpTangent(x, y_temp);
    } else return y[middle];
}

POINT findDownTangent(POINT x, vector<POINT> y) {
    vector<POINT> y_temp;
    int middle = y.size() / 2;
    if (relation(x, y[middle], y[middle - 1]) == 0) {
        return y[middle];
    } else if (relation(x, y[middle], y[middle + 1]) == 0) {
        return y[middle + 1];
    } else if (relation(x, y[middle], y[middle - 1]) > 0 && relation(x, y[middle], y[middle + 1]) < 0) {
        for (int i = 0; i <= middle; i++) {
            y_temp.push_back(y[i]);
        }
        if (y_temp.size() == 2)
            return y_temp[0];
        else return findDownTangent(x, y_temp);
    } else if (relation(x, y[middle], y[middle - 1]) < 0 && relation(x, y[middle], y[middle + 1]) > 0) {
        for (int i = middle; i < y.size(); i++) {
            y_temp.push_back(y[i]);
        }
        if (y_temp.size() == 2)
            return y_temp[1];
        else return findDownTangent(x, y_temp);
    } else return y[middle];
}

//left right
vector<POINT> findUpCommonTangent(vector<POINT> left, vector<POINT> right) {
    vector<POINT> right_temp;
    int middle_right = right.size() / 2;
    POINT face_left = findUpTangent(right[middle_right], left);
    if (relation(face_left, right[middle_right], right[middle_right - 1]) < 0 &&
        relation(face_left, right[middle_right], right[middle_right + 1]) > 0) {
        for (int i = middle_right; i < right.size(); i++) {
            right_temp.push_back(right[i]);
        }
        if (right_temp.size() == 2) return {face_left, right_temp[1]};
        return findUpCommonTangent(left, right_temp);
    } else if (relation(face_left, right[middle_right], right[middle_right - 1]) > 0 &&
               relation(face_left, right[middle_right], right[middle_right + 1]) < 0) {
        for (int i = 0; i <= middle_right; i++) {
            right_temp.push_back(right[i]);
        }
        if (right_temp.size() == 2) return {face_left, right_temp[0]};
        return findUpCommonTangent(left, right_temp);
    } else if (relation(face_left, right[middle_right], right[middle_right + 1]) == 0) {
        return {face_left, right[middle_right]};
    } else if (relation(face_left, right[middle_right], right[middle_right - 1]) == 0) {
        return {face_left, right[middle_right - 1]};
    } else return {face_left, right[middle_right]};
}

vector<POINT> findDownCommonTangent(vector<POINT> left, vector<POINT> right) {
    vector<POINT> right_temp;
    int middle_right = right.size() / 2;
    POINT face_left = findDownTangent(right[middle_right], left);
    if (relation(face_left, right[middle_right], right[middle_right - 1]) > 0 &&
        relation(face_left, right[middle_right], right[middle_right + 1]) < 0) {
        for (int i = middle_right; i < right.size(); i++) {
            right_temp.push_back(right[i]);
        }
        if (right_temp.size() == 2) return {face_left, right_temp[1]};
        return findDownCommonTangent(left, right_temp);
    } else if (relation(face_left, right[middle_right], right[middle_right - 1]) < 0 &&
               relation(face_left, right[middle_right], right[middle_right + 1]) > 0) {
        for (int i = 0; i <= middle_right; i++) {
            right_temp.push_back(right[i]);
        }
        if (right_temp.size() == 2) return {face_left, right_temp[0]};
        return findDownCommonTangent(left, right_temp);
    } else if (relation(face_left, right[middle_right], right[middle_right + 1]) == 0) {
        return {face_left, right[middle_right]};
    } else if (relation(face_left, right[middle_right], right[middle_right - 1]) == 0) {
        return {face_left, right[middle_right - 1]};
    } else return {face_left, right[middle_right]};
}

vector<POINT> merge_partion(vector<POINT> left, vector<POINT> right) {
    left.push_back({1000000, 1000000});
    right.push_back({1000000, 1000000});
    int left_index = 0, right_index = 0;
    vector<POINT> result;
    for (int i = 0; i < (left.size() + right.size()); i++) {
        if (relatation_point(left[left_index], right[right_index]) > 0) {
            result.push_back(right[right_index]);
            right_index++;
        } else {
            result.push_back(left[left_index]);
            left_index++;
        }
    }
    return result;

}

int main() {
    vector<POINT> left = {{1, 1},
                          {2, 5},
                          {3, 8},
                          {4, 9},
                          {5, 7},
                          {6, 8}};
    vector<POINT> right = {{13, 2},
                           {7,  4},
                           {9,  7},
                           {10, 6},
                           {12, 4}};


    vector<POINT> a = findUpCommonTangent(left, right);
    printf("%d  %d\n", a[0].x, a[0].y);
    printf("%d  %d\n", a[1].x, a[1].y);
    vector<POINT> left1 = {{1, -1},
                           {2, -5},
                           {3, -8},
                           {4, -9},
                           {5, -7},
                           {6, -8}};
    vector<POINT> right1 = {{13, -2},
                            {7,  -4},
                            {9,  -7},
                            {10, -6},
                            {12, -4}};


    vector<POINT> a1 = findDownCommonTangent(left1, right1);
    printf("%d  %d\n", a1[0].x, a1[0].y);
    printf("%d  %d\n", a1[1].x, a1[1].y);

    return 0;
}