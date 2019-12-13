#include <bits/stdc++.h>
#include "mpi.h"
#include <cstddef>

using namespace std;
struct POINT {
    int x;
    int y;
};

int lengths[2] = {1, 1};
MPI_Status status;
MPI_Aint offsets[2] = {offsetof(POINT, x), offsetof(POINT, y)};
MPI_Datatype types[2] = {MPI_INT, MPI_INT};
MPI_Datatype POINTS_STRUCT;


//1 is greater, -1 is lower, 0 is same
int relation_point(POINT point1, POINT point2) {
    if (point1.x > point2.x) {
        return 1;
    } else if (point1.x < point2.x) {
        return -1;
    } else {
        if (point1.y > point2.y) return 1;
        else if (point1.y < point2.y) return -1;
        else return 0;
    }
}

//p1-p2-p3 >0 re trai, <0 re phai, = 0 thi k doi
int relation(POINT point1, POINT point2, POINT point3) {

    long long temp = (long long) (point3.y - point1.y) * (point2.x - point1.x) -
                     (long long) (point2.y - point1.y) * (point3.x - point1.x);
    if (temp > 0) return 1; else if (temp < 0) return -1; else return 0;
}

vector<POINT> quick_sort(vector<POINT> p) {
    vector<POINT> result, left, right, left_temp, right_temp;
    if (p.size() == 1) {
        return p;
    }
    if (p.size() == 2) {
        if (relation_point(p[0], p[1]) > 0) {
            result.push_back(p[1]);
            result.push_back(p[0]);
        } else {
            result.push_back(p[0]);
            result.push_back(p[1]);
        }
        return result;
    } else {
        int pivot = p.size() / 2;
        for (int i = 0; i < p.size(); i++) {
            if (relation_point(p[i], p[pivot]) > 0) {
                right_temp.push_back(p[i]);
            } else if (relation_point(p[i], p[pivot]) < 0) {
                left_temp.push_back(p[i]);
            }
        }
        if (!left_temp.empty()) {
            left = quick_sort(left_temp);
        }
        left.push_back(p[pivot]);
        if (!right_temp.empty()) {
            right = quick_sort(right_temp);
            left.insert(left.end(), right.begin(), right.end());
        }

        return left;
    }
}

vector<POINT> devided_vector(vector<POINT> p, int numpro, int pid) {
    int len_divided_vector = p.size() / numpro;

    vector<POINT> array;
    for (int i = pid * len_divided_vector;
         i < len_divided_vector * (pid + 1) ||
         (i < p.size() && p.size() > len_divided_vector * (pid + 1) && p.size() < len_divided_vector * (pid + 2));
         i++) {
        array.push_back(p[i]);
    }
    return array;
}

vector<POINT> devided_by_x_vector(vector<POINT> p, int numpro, int pid, vector<POINT> max_min) {
    int range_x = max_min[0].x - max_min[1].x;
    int len_vector_sort = range_x / numpro;
    vector<int> index_x;
    int int_temp = max_min[1].x;
    while (int_temp <= max_min[0].x) {
        index_x.push_back(int_temp);
        int_temp += len_vector_sort;
        if (int_temp > max_min[0].x) {
            index_x[index_x.size() - 1] = max_min[0].x;
        }
    }
    vector<POINT> array;
    for (int i = 0; i < p.size(); i++) {
        if (p[i].x > index_x[pid] && p[i].x < index_x[pid + 1]) {
            array.push_back(p[i]);
        }
    }
    return array;
}

vector<POINT> get_max_min_point(vector<POINT> p, int numpro, int pid) {
    MPI_Type_create_struct(2, lengths, offsets, types, &POINTS_STRUCT);
    MPI_Type_commit(&POINTS_STRUCT);

    vector<POINT> array = devided_vector(p, numpro, pid);

    if (pid == 0) {
        vector<POINT> max_min, max_min_temp;
        max_min.push_back(array[0]);
        max_min.push_back(array[0]);
        POINT max_temp;
        for (int i = 0; i < array.size() - 1; i++) {
            if (relation_point(max_min[0], array[i + 1]) < 0) max_min[0] = array[i + 1];
            if (relation_point(max_min[1], array[i + 1]) > 0) max_min[1] = array[i + 1];
        }
        max_min_temp.resize(2);
        for (int i = 1; i < numpro; i++) {
            MPI_Recv(max_min_temp.data(), 2, POINTS_STRUCT, 1, i, MPI_COMM_WORLD, &status);
            if (relation_point(max_min[0], max_min_temp[0]) < 0) {
                max_min[0] = max_min_temp[0];
            }
            if (relation_point(max_min[1], max_min_temp[1]) > 0) {
                max_min[1] = max_min_temp[1];
            }
        }
        for (int i = 1; i < numpro; i++) {
            MPI_Send(max_min.data(), 2, POINTS_STRUCT, i, 0, MPI_COMM_WORLD);
        }
        return max_min;

    } else {
        vector<POINT> max_min;
        max_min.push_back(array[0]);
        max_min.push_back(array[0]);

        for (int i = 0; i < array.size() - 1; i++) {
            if (relation_point(max_min[0], array[i + 1]) < 0) max_min[0] = array[i + 1];
            if (relation_point(max_min[1], array[i + 1]) > 0) max_min[1] = array[i + 1];
        }
        MPI_Send(max_min.data(), 2, POINTS_STRUCT, 0, pid, MPI_COMM_WORLD);

        MPI_Recv(max_min.data(), 2, POINTS_STRUCT, 0, 0, MPI_COMM_WORLD, &status);
        return max_min;
    }
}

vector<POINT> sort_points(vector<POINT> points, int numpro, int pid, vector<POINT> max_min) {
    vector<POINT> array = devided_by_x_vector(points, numpro, pid, max_min);
    vector<POINT> sorted_array = quick_sort(array);
    MPI_Type_create_struct(2, lengths, offsets, types, &POINTS_STRUCT);
    MPI_Type_commit(&POINTS_STRUCT);

    if (pid == 0) {
        int len_temp = 0;
        vector<POINT> sorted_array_temp;
        for (int i = 1; i < numpro; i++) {
            MPI_Recv(&len_temp, 1, MPI_INT, i, 100, MPI_COMM_WORLD, &status);
            sorted_array_temp.resize(len_temp);
            MPI_Recv(sorted_array_temp.data(), len_temp, POINTS_STRUCT, i, 1, MPI_COMM_WORLD, &status);
            sorted_array.insert(sorted_array.end(), sorted_array_temp.begin(), sorted_array_temp.end());
        }
        len_temp = sorted_array.size();
        for (int i = 1; i < numpro; i++) {
            MPI_Send(&len_temp, 1, MPI_INT, i, 100, MPI_COMM_WORLD);
            MPI_Send(sorted_array.data(), sorted_array.size(), POINTS_STRUCT, i, i, MPI_COMM_WORLD);
        }
        return sorted_array;
    } else {
        int len;
        len = sorted_array.size();
        MPI_Send(&len, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
        MPI_Send(sorted_array.data(), sorted_array.size(), POINTS_STRUCT, 0, 1, MPI_COMM_WORLD);

        MPI_Recv(&len, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        sorted_array.resize(len);
        MPI_Recv(sorted_array.data(), len, POINTS_STRUCT, 0, pid, MPI_COMM_WORLD, &status);

        return sorted_array;
    }
}

POINT findUpTangent(POINT x, vector<POINT> y) {
    if (y.size() == 1)
        return y[0];
    else if (y.size() == 2) {
        if (relation(x, y[1], y[0]) < 0) return y[0];
        else return y[1];
    } else {
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
}

POINT findDownTangent(POINT x, vector<POINT> y) {
    //todo check 1, 2 point
    //todo view relation_point when in down
    vector<POINT> y_temp;
    if (y.size() == 1) return y[0];
    if (y.size() == 2) {
        if (relation(x, y[1], y[0]) < 0) return y[1];
        else return y[0];
//        else return y[0];
    } else {
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
}

vector<POINT> findUpCommonTangent(vector<POINT> left, vector<POINT> right) {
    if (right.size() < 3) {
        POINT face_left = findDownTangent(right[0], left);
        return {face_left, right[0]};
    } else {
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
}

vector<POINT> findDownCommonTangent(vector<POINT> left, vector<POINT> right) {
    vector<POINT> right_temp;
    if (right.size() < 3) {
        POINT face_left = findDownTangent(right[0], left);
        return {face_left, right[0]};
    } else {
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
}


int main(int argc, char **argv) {
    vector<POINT> points;
    POINT temp;
    ofstream data;
//    ofstream data11;
//    data11.open("data.txt");
//    for (int i = 0; i < 100; i++) {
//        temp.x = rand() % 100000;
//        temp.y = rand() % 100000;
//        points.push_back(temp);
//        data11 << temp.x << " " << temp.y << "\n";
//    }
//    data11.close();

    ifstream infile("data.txt");
    int a, b;
    while (infile >> a >> b) {
        temp.x = a;
        temp.y = b;
        points.push_back(temp);
    }

    int pid, numpro;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &numpro);
    vector<POINT> max_min, up, down;
    max_min = get_max_min_point(points, numpro, pid);

    vector<POINT> array_pid = devided_vector(points, numpro, pid);
    int len;

    for (int i = 0; i < array_pid.size(); i++) {
        if (relation(max_min[1], max_min[0], array_pid[i]) >= 0) {
            up.push_back(array_pid[i]);
        } else if (relation(max_min[1], max_min[0], array_pid[i]) <= 0) {
            down.push_back(array_pid[i]);
        }
    }

    if (pid == 0) {
        vector<POINT> temp_point;
        for (int i = 1; i < numpro; i++) {
            MPI_Recv(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            if (len != 0) {
                temp_point.resize(len);
                MPI_Recv(temp_point.data(), len, POINTS_STRUCT, i, 1, MPI_COMM_WORLD, &status);
                up.insert(up.end(), temp_point.begin(), temp_point.end());
            }

            MPI_Recv(&len, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
            if (len != 0) {
                temp_point.resize(len);
                MPI_Recv(temp_point.data(), len, POINTS_STRUCT, i, 3, MPI_COMM_WORLD, &status);
                down.insert(down.end(), temp_point.begin(), temp_point.end());
            }
        }

        for (int i = 1; i < numpro; i++) {
            len = up.size();
            MPI_Send(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            if (len != 0) {
                MPI_Send(up.data(), up.size(), POINTS_STRUCT, i, 1, MPI_COMM_WORLD);
            }
            len = down.size();
            MPI_Send(&len, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            if (len != 0) {
                MPI_Send(down.data(), down.size(), POINTS_STRUCT, i, 3, MPI_COMM_WORLD);
            }
        }

    } else {
        len = up.size();
        MPI_Send(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        if (len != 0) {
            MPI_Send(up.data(), up.size(), POINTS_STRUCT, 0, 1, MPI_COMM_WORLD);
        }
        len = down.size();
        MPI_Send(&len, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        if (len != 0) {
            MPI_Send(down.data(), down.size(), POINTS_STRUCT, 0, 3, MPI_COMM_WORLD);
        }
        up.clear();
        down.clear();

        MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        if (len != 0) {
            up.resize(len);
            MPI_Recv(up.data(), len, POINTS_STRUCT, 0, 1, MPI_COMM_WORLD, &status);
        }
        MPI_Recv(&len, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        if (len != 0) {
            down.resize(len);
            MPI_Recv(down.data(), len, POINTS_STRUCT, 0, 3, MPI_COMM_WORLD, &status);
        }
    }


    down = sort_points(down, numpro, pid, max_min);
    up = sort_points(up, numpro, pid, max_min);
    vector<POINT> down_array, down_array_temp, up_array, convex_down, convex_up;
    down_array_temp = devided_vector(down, numpro, pid);
    down_array.push_back(max_min[1]);
    down_array.insert(down_array.end(), down_array_temp.begin(), down_array_temp.end());
    down_array.push_back(max_min[0]);
    data.open("result1.txt");
    for (int i = 0; i < down_array.size(); i++) {
        temp.x = down_array[i].x;
        temp.y = down_array[i].y;
        data << temp.x << " " << temp.y << "\n";

    }
    data.close();

    vector<int> index_down(1, 0);
    if (down_array.size() > 2) {
        int max_size = down_array.size();
        POINT point_temp_temp;
        for (int i = 1; i < max_size; i++) {
            point_temp_temp = down_array[i];
            while (index_down.size() > 1 &&
                   relation(down_array[index_down[index_down.size() - 2]],
                            down_array[index_down[index_down.size() - 1]], point_temp_temp) <
                   0) {
                index_down.pop_back();
            }
            printf("%d \n", i);
            index_down.push_back(i);
        }
    }
    for (int i = 0; i < index_down.size(); i++) {
        down_array[i] = down_array[index_down[i]];
    }
    down_array.resize(index_down.size());


    if (pid == 0) {
        vector<POINT> temp_dowm_array;
        vector<vector<POINT>> list_convex_down;
        list_convex_down.push_back(down_array);
//        vector<POINT> left_right;
        for (int i = 1; i < numpro; i++) {
            MPI_Recv(&len, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &status);
            if (len != 0) {
                temp_dowm_array.resize(len);
                MPI_Recv(temp_dowm_array.data(), len, POINTS_STRUCT, i, 5, MPI_COMM_WORLD, &status);
                list_convex_down.push_back(temp_dowm_array);
            }
        }
        vector<POINT> left_right;
        vector<POINT> left_right_temp;
        left_right.push_back(max_min[0]);
        for (int i = 0; i < list_convex_down.size() - 1; i++) {
            left_right_temp = findDownCommonTangent(list_convex_down[i], list_convex_down[i + 1]);
            left_right.insert(left_right.end(), left_right_temp.begin(), left_right_temp.end());
        }
        left_right.push_back(max_min[1]);

        for (int i = 0; i < list_convex_down.size(); i++) {
            if (relation_point(left_right[i * 2], left_right[i * 2 + 1]) <= 0) {
                for (int j = 0; j < list_convex_down[i].size(); i++) {
                    if (relation_point(list_convex_down[i][j], left_right[i * 2]) >= 0 &&
                        relation_point(list_convex_down[i][j], left_right[i * 2 + 1]) <= 0) {
                        convex_down.push_back(list_convex_down[i][j]);
                    }
                }
            }
        }
        ofstream data1;
        data1.open("result2.txt");
        for (int i = 0; i < convex_down.size(); i++) {
            temp.x = convex_down[i].x;
            temp.y = convex_down[i].y;
            data1 << temp.x << " " << temp.y << "\n";

        }
        data1.close();

    } else {
        len = down_array.size();
        MPI_Send(&len, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
        if (len != 0) {
            MPI_Send(down_array.data(), down_array.size(), POINTS_STRUCT, 0, 5, MPI_COMM_WORLD);
        }
    }

//    up_array = devided_vector(up, numpro, pid);
//    if (up_array.size() > 2) {
//        for (int i = 2; i < up_array.size(); i++) {
//            if (relation(up_array[i - 2], up_array[i - 1], up_array[i]) > 0) {
//                for (int j = i - 1; j < up_array.size() - 1; j++) {
//                    up_array[j] = up_array[j + 1];
//                }
//                up_array.resize(up_array.size() - 1);
//                i--;
//            }
//        }
//    }
//    if (pid == 0) {
//        vector<POINT> temp_up_array;
//        vector<vector<POINT>> list_convex_up;
//        list_convex_up.push_back(up_array);
//        for (int i = 1; i < numpro; i++) {
//            MPI_Recv(&len, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &status);
//            if (len != 0) {
//                temp_up_array.resize(len);
//                MPI_Recv(temp_up_array.data(), len, POINTS_STRUCT, i, 5, MPI_COMM_WORLD, &status);
//                list_convex_up.push_back(temp_up_array);
//            }
//        }
//        vector<POINT> left_right;
//        vector<POINT> left_right_temp;
//        left_right.push_back(max_min[0]);
//        for (int i = 0; i < list_convex_up.size() - 1; i++) {
//            left_right_temp = findUpCommonTangent(list_convex_up[i], list_convex_up[i + 1]);
//            left_right.insert(left_right.end(), left_right_temp.begin(), left_right_temp.end());
//        }
//        left_right.push_back(max_min[1]);
//
//        for (int i = 0; i < list_convex_up.size(); i++) {
//            if (relation_point(left_right[i * 2], left_right[i * 2 + 1]) <= 0) {
//                for (int j = 0; j < list_convex_up[i].size(); i++) {
//                    if (relation_point(list_convex_up[i][j], left_right[i * 2]) >= 0 &&
//                        relation_point(list_convex_up[i][j], left_right[i * 2 + 1]) <= 0) {
//                        convex_up.push_back(list_convex_up[i][j]);
//                    }
//                }
//            }
//        }
//    } else {
//        len = up_array.size();
//        MPI_Send(&len, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
//        if (len != 0) {
//            MPI_Send(up_array.data(), up_array.size(), POINTS_STRUCT, 0, 5, MPI_COMM_WORLD);
//        }
//    }
//    for (int i = 0; i < up_array.size(); i++) {
//        printf("%d upupup %d %d \n", pid, up[i].x, up[i].y);
//    }

//    ofstream data;
//    data.open("result.txt");
//    for (int i = 0; i < down_array.size(); i++) {
//        temp.x = down_array[i].x;
//        temp.y = down_array[i].y;
//        points.push_back(temp);
//        data << temp.x << " " << temp.y << "\n";
//
//    }
//    data.close();

    MPI_Finalize();
}