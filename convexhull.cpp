#include <bits/stdc++.h>
#include "mpi.h"
#include <cstddef>
#include <chrono>

using namespace std::chrono;
using namespace std;
struct POINT {
    int x;
    int y;
};
int lengths[2] = {1, 1};
double t_communication, start1, end1;
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

// chia theo bo vi xu ly
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

// chia theo x de sort
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
            start1 = MPI_Wtime();
            MPI_Recv(max_min_temp.data(), 2, POINTS_STRUCT, i, i, MPI_COMM_WORLD, &status);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (relation_point(max_min[0], max_min_temp[0]) < 0) {
                max_min[0] = max_min_temp[0];
            }
            if (relation_point(max_min[1], max_min_temp[1]) > 0) {
                max_min[1] = max_min_temp[1];
            }
        }
        for (int i = 1; i < numpro; i++) {
            start1 = MPI_Wtime();
            MPI_Send(max_min.data(), 2, POINTS_STRUCT, i, 0, MPI_COMM_WORLD);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
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
            start1 = MPI_Wtime();
            MPI_Recv(&len_temp, 1, MPI_INT, i, 100, MPI_COMM_WORLD, &status);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;

            sorted_array_temp.resize(len_temp);
            start1 = MPI_Wtime();
            MPI_Recv(sorted_array_temp.data(), len_temp, POINTS_STRUCT, i, 1, MPI_COMM_WORLD, &status);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (sorted_array_temp.size() != 0)
                sorted_array.insert(sorted_array.end(), sorted_array_temp.begin(), sorted_array_temp.end());
        }
        len_temp = sorted_array.size();
        for (int i = 1; i < numpro; i++) {
            start1 = MPI_Wtime();
            MPI_Send(&len_temp, 1, MPI_INT, i, 100, MPI_COMM_WORLD);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            start1 = MPI_Wtime();
            MPI_Send(sorted_array.data(), sorted_array.size(), POINTS_STRUCT, i, i, MPI_COMM_WORLD);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
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

//tim tiep tuyen
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
    vector<POINT> y_temp;
    if (y.size() == 1) return y[0];
    if (y.size() == 2) {
        if (relation(x, y[1], y[0]) < 0) return y[1];
        else return y[0];
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

//tim tiep tuyen chung
vector<POINT> findUpCommonTangent(vector<POINT> left, vector<POINT> right) {
    if (right.size() == 1) {
        POINT face_left = findUpTangent(right[0], left);
        return {face_left, right[0]};
    } else if (right.size() == 2) {
        POINT face_left = findUpTangent(right[0], left);
        if (relation(face_left, right[0], right[1]) > 0) {
            face_left = findUpTangent(right[1], left);
            return {face_left, right[1]};
        }
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
    if (right.size() == 1) {
        POINT face_left = findDownTangent(right[0], left);
        return {face_left, right[0]};
    } else if (right.size() == 2) {
        POINT face_left = findDownTangent(right[0], left);
        if (relation(face_left, right[0], right[1]) < 0) {
            face_left = findDownTangent(right[1], left);
            return {face_left, right[1]};
        }
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
    srand(time(NULL));
    vector<POINT> points;
    POINT temp;
    ofstream data;
    ofstream data11;
//    argv[1]
//    data11.open("data.txt");
//    for (int i = 0; i < 100000; i++) {
//        temp.x = rand() % 100000000;
//        temp.y = rand() % 100000000;
//        points.push_back(temp);
//        data11 << temp.x << " " << temp.y << "\n";
//    }
//    data11.close();
    ifstream infile(argv[1]);
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
    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Hello class from processor %s, rank %d out of %d processors\n", processor_name, pid, numpro);
    vector<POINT> max_min, up, down;
    double start = MPI_Wtime();
//    chia up, down
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
            start1 = MPI_Wtime();
            MPI_Recv(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (len != 0) {
                temp_point.resize(len);
                start1 = MPI_Wtime();
                MPI_Recv(temp_point.data(), len, POINTS_STRUCT, i, 1, MPI_COMM_WORLD, &status);
                end1 = MPI_Wtime();
                t_communication += end1 - start1;
                up.insert(up.end(), temp_point.begin(), temp_point.end());
            }
            start1 = MPI_Wtime();
            MPI_Recv(&len, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (len != 0) {
                temp_point.resize(len);
                start1 = MPI_Wtime();
                MPI_Recv(temp_point.data(), len, POINTS_STRUCT, i, 3, MPI_COMM_WORLD, &status);
                end1 = MPI_Wtime();
                t_communication += end1 - start1;
                down.insert(down.end(), temp_point.begin(), temp_point.end());
            }
        }
        for (int i = 1; i < numpro; i++) {
            len = up.size();
            start1 = MPI_Wtime();
            MPI_Send(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (len != 0) {
                start1 = MPI_Wtime();
                MPI_Send(up.data(), up.size(), POINTS_STRUCT, i, 1, MPI_COMM_WORLD);
                end1 = MPI_Wtime();
                t_communication += end1 - start1;
            }
            len = down.size();
            start1 = MPI_Wtime();
            MPI_Send(&len, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (len != 0) {
                start1 = MPI_Wtime();
                MPI_Send(down.data(), down.size(), POINTS_STRUCT, i, 3, MPI_COMM_WORLD);
                end1 = MPI_Wtime();
                t_communication += end1 - start1;
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

// sap xep theo quick sort
    down = sort_points(down, numpro, pid, max_min);
    up = sort_points(up, numpro, pid, max_min);

//    chia point de xu li, tu dong them point max, min
    vector<POINT> down_array, down_array_temp, convex_down;
    down_array_temp = devided_vector(down, numpro, pid);
    down_array.push_back(max_min[1]);
    down_array.insert(down_array.end(), down_array_temp.begin(), down_array_temp.end());
    down_array.push_back(max_min[0]);

//  tim down convexhull tung bo vxl
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
            index_down.push_back(i);
        }
    }
    for (int i = 0; i < index_down.size(); i++) {
        down_array[i] = down_array[index_down[i]];
    }
    down_array.resize(index_down.size());

//    ket hop
    if (pid == 0) {
        vector<POINT> temp_dowm_array;
        vector<vector<POINT>> list_convex_down;// tap cac convexdown o tung vxl
        if (numpro != 1) down_array.pop_back();
        data.open("down_data.txt");
        data << "0 \n";
        for (int i = 0; i < down_array.size(); i++) {
            data << down_array[i].x << " " << down_array[i].y << "\n";
        }
//        list_convex_down.push_back(down_array);
        for (int i = 1; i < numpro; i++) {
            start1 = MPI_Wtime();
            MPI_Recv(&len, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &status);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (len != 0) {
                temp_dowm_array.resize(len);
                start1 = MPI_Wtime();
                MPI_Recv(temp_dowm_array.data(), len, POINTS_STRUCT, i, 5, MPI_COMM_WORLD, &status);
                end1 = MPI_Wtime();
                t_communication += end1 - start1;
                if (i == numpro - 1) {
                    temp_dowm_array.erase(temp_dowm_array.begin());
                } else {
                    temp_dowm_array.pop_back();
                    temp_dowm_array.erase(temp_dowm_array.begin());
                }
                data << i << " \n";
                for (int i = 0; i < temp_dowm_array.size(); i++) {
                    data << temp_dowm_array[i].x << " " << temp_dowm_array[i].y << "\n";
                }
                list_convex_down.push_back(temp_dowm_array);
            }
        }
        data.close();
        vector<POINT> left_right, convexdown_temp_thang;//chi so left right convex down tung bo vxl
        convex_down = down_array;
        for (int i = 0; i < list_convex_down.size(); i++) {

            left_right = findDownCommonTangent(convex_down, list_convex_down[i]);
            convexdown_temp_thang.resize(0);
            convexdown_temp_thang = convex_down;
            convex_down.resize(0);
            for (int j = 0; j < convexdown_temp_thang.size(); j++) {
                if (relation_point(convexdown_temp_thang[j], left_right[0]) <= 0) {
                    convex_down.push_back(convexdown_temp_thang[j]);
                }
            }
            for (int j = 0; j < list_convex_down[i].size(); j++) {
                if (relation_point(list_convex_down[i][j], left_right[1]) >= 0) {
                    convex_down.push_back(list_convex_down[i][j]);
                }
            }
        }
    } else {
        len = down_array.size();
        MPI_Send(&len, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
        if (len != 0) {
            MPI_Send(down_array.data(), down_array.size(), POINTS_STRUCT, 0, 5, MPI_COMM_WORLD);
        }
    }

    vector<POINT> up_array, up_array_temp, convex_up;
    up_array_temp = devided_vector(up, numpro, pid);
    up_array.push_back(max_min[1]);
    up_array.insert(up_array.end(), up_array_temp.begin(), up_array_temp.end());
    up_array.push_back(max_min[0]);
    vector<int> index_up(1, 0);
    if (up_array.size() > 2) {
        int max_size = up_array.size();
        POINT point_temp_temp;
        for (int i = 1; i < max_size; i++) {
            point_temp_temp = up_array[i];
            while (index_up.size() > 1 &&
                   relation(up_array[index_up[index_up.size() - 2]],
                            up_array[index_up[index_up.size() - 1]], point_temp_temp) >
                   0) {
                index_up.pop_back();
            }
            index_up.push_back(i);
        }
    }
    for (int i = 0; i < index_up.size(); i++) {
        up_array[i] = up_array[index_up[i]];
    }
    up_array.resize(index_up.size());
    if (pid == 0) {
        vector<POINT> temp_up_array;
        vector<vector<POINT>> list_convex_up;
        if (numpro != 1) up_array.pop_back();
        data.open("up_data.txt");
        data << "0 \n";
        for (int i = 0; i < up_array.size(); i++) {
            data << up_array[i].x << " " << up_array[i].y << "\n";
        }
        for (int i = 1; i < numpro; i++) {
            start1 = MPI_Wtime();
            MPI_Recv(&len, 1, MPI_INT, i, 6, MPI_COMM_WORLD, &status);
            end1 = MPI_Wtime();
            t_communication += end1 - start1;
            if (len != 0) {
                temp_up_array.resize(len);
                start1 = MPI_Wtime();
                MPI_Recv(temp_up_array.data(), len, POINTS_STRUCT, i, 7, MPI_COMM_WORLD, &status);
                end1 = MPI_Wtime();
                t_communication += end1 - start1;
                if (i == numpro - 1) {
                    temp_up_array.erase(temp_up_array.begin());
                } else {
                    temp_up_array.pop_back();
                    temp_up_array.erase(temp_up_array.begin());
                }
                data << i << " \n";
                for (int i = 0; i < temp_up_array.size(); i++) {
                    data << temp_up_array[i].x << " " << temp_up_array[i].y << "\n";
                }
                list_convex_up.push_back(temp_up_array);
            }
        }
        vector<POINT> left_right, convexup_temp_thang;
        convex_up = up_array;
        for (int i = 0; i < list_convex_up.size(); i++) {
            left_right = findUpCommonTangent(convex_up, list_convex_up[i]);
            convexup_temp_thang.resize(0);
            convexup_temp_thang = convex_up;
            convex_up.resize(0);
            for (int j = 0; j < convexup_temp_thang.size(); j++) {
                if (relation_point(convexup_temp_thang[j], left_right[0]) <= 0) {
                    convex_up.push_back(convexup_temp_thang[j]);
                }
            }
            for (int j = 0; j < list_convex_up[i].size(); j++) {
                if (relation_point(list_convex_up[i][j], left_right[1]) >= 0) {
                    convex_up.push_back(list_convex_up[i][j]);
                }
            }
        }
        ofstream data1;
        data1.open("result.txt");
        for (int i = 0; i < convex_down.size(); i++) {
            temp.x = convex_down[i].x;
            temp.y = convex_down[i].y;
            data1 << temp.x << " " << temp.y << "\n";
        }
        for (int i = convex_up.size() - 2; i >= 0; i--) {
            temp.x = convex_up[i].x;
            temp.y = convex_up[i].y;
            data1 << temp.x << " " << temp.y << "\n";
        }
        data1.close();
        std::cout << "The process " << processor_name << " took " << t_communication << " seconds to comunication."
                  << std::endl;
        double end = MPI_Wtime();
        std::cout << "The process " << processor_name << " took " << end - start << " seconds to run." << std::endl;
    } else {
        len = up_array.size();
        MPI_Send(&len, 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
        if (len != 0) {
            MPI_Send(up_array.data(), up_array.size(), POINTS_STRUCT, 0, 7, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
}