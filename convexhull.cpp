#include "mpi.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;
typedef struct {
    int x;
    int y;
} POINT;

vector<POINT> points;


void genPoint(int number) {
    POINT temp;
    ofstream data;
    data.open("data.txt");
    for (int i = 0; i < number; i++) {
        temp.x = rand() % 1000 - rand() % 1000;
        temp.y = rand() % 1000 - rand() % 1000;
        points.push_back(temp);
        data << temp.x << " " << temp.y << "\n";
    }
    data.close();
}

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

int main(int argc, char *argv[]) {
    int number = 100;
//    genPoint(number);
    int pid, numpro;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &numpro);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int namelen;
    MPI_Get_processor_name(processor_name, &namelen);
    printf("Hello world from processor %s , rank %d out of %d processors\n", processor_name, pid, numpro);
    MPI_Status status;

    int lengths[2] = {1, 1};
    MPI_Aint offsets[2] = {offsetof(POINT, x), offsetof(POINT, y)};
    MPI_Datatype types[2] = {MPI_INT, MPI_INT};
    MPI_Datatype POINTS_STRUCT;
    MPI_Type_create_struct(2, lengths, offsets, types, &POINTS_STRUCT);
    MPI_Type_commit(&POINTS_STRUCT);

    if (pid == 0) {
        vector<POINT> test;
        test = {{1, 2},
                {1, 3}};
        MPI_Send(test.data(), 2, POINTS_STRUCT, 1, 0, MPI_COMM_WORLD);

    } else {
//        test.resize(4);
        vector<POINT> test1;
        test1.resize(2);
        MPI_Recv(test1.data(), 2, POINTS_STRUCT, 0, 0, MPI_COMM_WORLD, &status);
//        for (int i = 0; i < 3; i++) {
        printf("%d  %d\n", test1[0].x, test1[0].y);
        printf("%d  %d\n", test1[1].x, test1[1].y);
//        }
    }


    MPI_Finalize();
    return 0;
}