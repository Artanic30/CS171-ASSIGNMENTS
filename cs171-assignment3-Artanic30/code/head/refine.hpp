#pragma once
#include <vector>
//#include "../3rdLibs/glm/glm/glm.hpp"
#include "Eigen/Dense"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////////
// You should try to reduce the time complexity. For each time of loop subdivision (divide  //
// once), the time complexity of your program should not be lower than O(p^2+f*l) (p:number //
// of points, f: number of faces, l: number of lines). And try to explain how you reduce the//
// time complexity in you report and video.                                                 //
//                                                                                          //
// The following is a reference for you to do loop subdivision.                             //
//////////////////////////////////////////////////////////////////////////////////////////////
namespace ref {
	class point {
	public:
		Eigen::Vector3f pos;
        Eigen::Vector2f uv;	// you can ignore uv
        //one pos may have different, if you want to use exact value, you should store this
        //std::vector<Eigen::Vector2f> uv;
        //std::vector<Eigen::Vector3f> normal;
        std::vector<int> n_face;
        std::vector<int> n_point;
        std::vector<int> b_point;
        std::vector<int> n_line;
        bool is_border;
        void add_n_point(int);
        void add_b_point(int);
        point();
        ~point();
	};

	class face {
    public:
        int v[3];   //We have to use index, instead of point itself;
        //std::vector<Eigen::Vector2f> n_line;
        std::vector<int> n_line;
        Eigen::Vector3f normal;
        face();
        ~face();
	};

    class line {
    public:
        int v1;     //We have to use index, instead of point itself;
        int v2;
        std::vector<int> n_face;
        bool is_border;
        line();
        ~line();
	};
    
    // Decide whether a line is boundary: Find adj faces of one of the two points, and check whether any of them used the other point.
    // Use face as the iterator of loop to find lines
    
    void build_data_struct(
        const std::vector < Eigen::Vector3f >& out_vertices,
        std::vector < Eigen::Vector3f > out_normals,
        std::vector < int > out_v_index,
        std::vector < int > out_vn_index,
        std::vector<ref::point>& P_list,
        std::vector<ref::line>& L_list,
        std::vector<ref::face>& F_list);
    

    // Loop subdivision
    void loop_subdivision(
        std::vector<ref::point>& P_list,
        const std::vector<ref::face>& F_list,
        std::vector<ref::face>& F_list_new);

    void make_face(
        std::vector<ref::point> P_list,
        std::vector<ref::face> F_list,
        std::vector<Eigen::Vector3f>& points, 
        std::vector<Eigen::Vector3f>& normals, 
        std::vector<Eigen::Vector2f>& uvs
    );
}
//line will decide which point is boundary
ref::point::point()
{
    this->is_border = false;
}
ref::point::~point()
{
    
}

void ref::point::add_n_point(int index) {
    if (std::find(n_point.begin(), n_point.end(), index) == n_point.end()) {
        n_point.push_back(index);
    }
}

void ref::point::add_b_point(int index) {
    if (std::find(b_point.begin(), b_point.end(), index) == b_point.end()) {
        b_point.push_back(index);
    }
}
//face will decide which line is not boundary
ref::line::line()
{
    this->is_border = true;
}
ref::line::~line()
{
    
}

ref::face::face()
{
}

ref::face::~face()
{
}

void ref::build_data_struct(
    const std::vector < Eigen::Vector3f >& out_vertices,
    std::vector < Eigen::Vector3f > out_normals,
    std::vector < int > out_v_index,
    std::vector < int > out_vn_index,
    std::vector<ref::point>& P_list,
    std::vector<ref::line>& L_list,
    std::vector<ref::face>& F_list)
{
    // out_vertices has same index as P_list
    // line will have multiplications while faces and points won't
    for(const auto& vert : out_vertices) {
        ref::point p;
        p.pos = vert;
        P_list.push_back(p);
    }
    for (int i = 0; i < out_v_index.size(); i+=3) {
        ref::face f;
        for (int j = 0; j < 3; ++j) {
            f.v[j] = out_v_index[i + j];
            // update face in points
            P_list[out_v_index[i + j]].n_face.push_back(F_list.size());
        }
        // update point n_point
        P_list[f.v[0]].add_n_point(f.v[1]);
        P_list[f.v[1]].add_n_point(f.v[0]);

        // update point n_point
        P_list[f.v[1]].add_n_point(f.v[2]);
        P_list[f.v[2]].add_n_point(f.v[1]);

        // update point n_point
        P_list[f.v[2]].add_n_point(f.v[0]);
        P_list[f.v[0]].add_n_point(f.v[2]);

        f.normal = out_normals[out_vn_index[i]];
        F_list.push_back(f);
    }
}

int get_or_create_point(std::vector<ref::point> P_list, const Eigen::Vector3f& pos) {
    for (int i = 0; i < P_list.size(); ++i) {
        if (P_list[i].pos == pos) {
            return i;
        }
    }
    return -1;
}

void ref::loop_subdivision(
    std::vector<ref::point>& P_list,
    const std::vector<ref::face>& F_list,
    std::vector<ref::face>& F_list_new)
{
    int old_point_size = P_list.size();
    // first step generate new points
    for (const auto& face : F_list) {
        ref::point A = P_list[face.v[0]], B = P_list[face.v[1]], C = P_list[face.v[2]];
        // point between A B
        std::vector<int> two_points;
        for (auto point_A : A.n_point) {
            for (auto point_B : B.n_point) {
                if (point_A == point_B) {
                    two_points.push_back(point_A);
                }
            }
        }
        int id_AB = P_list.size();
        int id_BC = P_list.size() + 1;
        int id_AC = P_list.size() + 2;
        Eigen::Vector3f new_pos;
        int return_find;
        if (two_points.size() == 2) {
            new_pos = (A.pos + B.pos) * 3.0f / 8.0f + (P_list[two_points[0]].pos + P_list[two_points[1]].pos) / 8.0f;
            //std::cout<<"positions: "<<std::endl<<A.pos<<std::endl<<B.pos<<std::endl<<P_list[two_points[0]].pos<<std::endl<<P_list[two_points[1]].pos<<std::endl;
            // std::cout<<"or value: "<<std::endl<<(A.pos + B.pos)<<std::endl<<" result: "<<std::endl<<(A.pos + B.pos) * 3.0f / 8.0f <<std::endl;
        } else {
            new_pos = 0.5f * (A.pos + B.pos);
        }
        ref::point new_point1;
        return_find = get_or_create_point(P_list, new_pos);
        if (return_find != -1) {
            new_point1 = P_list[return_find];
            id_AB = return_find;
            id_AC --;
            id_BC --;
        } else {
            new_point1.pos = new_pos;
        }
        //std::cout<<"result pos: "<<std::endl<<new_point1.pos<<std::endl;

        //std::cout<<"id: "<<id_AB<<std::endl;
        new_point1.n_face.push_back(F_list_new.size());
        new_point1.n_face.push_back(F_list_new.size() + 1);
        new_point1.n_face.push_back(F_list_new.size() + 3);

        // B C
        two_points.clear();
        for (auto point_B : B.n_point) {
            for (auto point_C : C.n_point) {
                if (point_B == point_C) {
                    two_points.push_back(point_C);
                }
            }
        }
        if (two_points.size() == 2) {
            //std::cout<<"positions: "<<std::endl<<C.pos<<std::endl<<B.pos<<std::endl<<P_list[two_points[0]].pos<<std::endl<<P_list[two_points[1]].pos<<std::endl;
            new_pos = (C.pos + B.pos) * 3.0f / 8.0f + (P_list[two_points[0]].pos + P_list[two_points[1]].pos) / 8.0f;
        } else {
            new_pos = 0.5f * (C.pos + B.pos);
        }
        ref::point new_point2;
        return_find = get_or_create_point(P_list, new_pos);
        if (return_find != -1) {
            new_point2 = P_list[return_find];
            id_BC = return_find;
            id_AC --;
        } else {
            new_point2.pos = new_pos;
        }

        //std::cout<<"result pos: "<<std::endl<<new_point1.pos<<std::endl;
        //std::cout<<"id: "<<id_BC<<std::endl;
        new_point2.n_face.push_back(F_list_new.size() + 1);
        new_point2.n_face.push_back(F_list_new.size() + 2);
        new_point2.n_face.push_back(F_list_new.size() + 3);


        // A C
        two_points.clear();
        for (auto point_A : A.n_point) {
            for (auto point_C : C.n_point) {
                if (point_A == point_C) {
                    two_points.push_back(point_C);
                }
            }
        }
        if (two_points.size() == 2) {
            //std::cout<<"positions: "<<std::endl<<C.pos<<std::endl<<A.pos<<std::endl<<P_list[two_points[0]].pos<<std::endl<<P_list[two_points[1]].pos<<std::endl;
            new_pos = (C.pos + A.pos) * 3.0f / 8.0f + (P_list[two_points[0]].pos + P_list[two_points[1]].pos) / 8.0f;
        } else {
            new_pos = 0.5f * (C.pos + A.pos);
        }
        ref::point new_point3;
        return_find = get_or_create_point(P_list, new_pos);
        if (return_find != -1) {
            new_point3 = P_list[return_find];
            id_AC = return_find;
        } else {
            new_point3.pos = new_pos;
        }

        //std::cout<<"result pos: "<<std::endl<<new_point1.pos<<std::endl;
        //std::cout<<"id: "<<id_AC<<std::endl;
        new_point3.n_face.push_back(F_list_new.size());
        new_point3.n_face.push_back(F_list_new.size() + 2);
        new_point3.n_face.push_back(F_list_new.size() + 3);

        new_point1.add_n_point(face.v[0]);
        new_point1.add_n_point(face.v[1]);
        new_point1.add_n_point(id_AC);
        new_point1.add_n_point(id_BC);

        new_point2.add_n_point(face.v[2]);
        new_point2.add_n_point(face.v[1]);
        new_point2.add_n_point(id_AB);
        new_point2.add_n_point(id_AC);

        new_point3.add_n_point(face.v[2]);
        new_point3.add_n_point(face.v[0]);
        new_point3.add_n_point(id_AB);
        new_point3.add_n_point(id_BC);


        // push processed points
        int size_p_list = P_list.size();
        if (id_AB >= P_list.size()) {
            P_list.push_back(new_point1);
        }
        if (id_BC >= P_list.size()) {
            P_list.push_back(new_point2);
        }
        if (id_AC >= P_list.size()) {
            P_list.push_back(new_point3);
        }

        // add nearby points for older points
        P_list[face.v[0]].add_b_point(id_AB);
        P_list[face.v[0]].add_b_point(id_AC);

        P_list[face.v[1]].add_b_point(id_AB);
        P_list[face.v[1]].add_b_point(id_BC);

        P_list[face.v[2]].add_b_point(id_BC);
        P_list[face.v[2]].add_b_point(id_AC);

        // generate faces
        ref::face face1, face2, face3, face4;
        Eigen::Vector3f tem_norm;
        face1.v[0] = id_AB;
        face1.v[1] = id_AC;
        face1.v[2] = face.v[0];
        tem_norm = (A.pos - new_point1.pos).cross(A.pos - new_point2.pos);
        face1.normal = tem_norm.dot(face.normal) > 0 ? tem_norm : -tem_norm;

        face2.v[0] = id_AB;
        face2.v[1] = id_BC;
        face2.v[2] = face.v[1];
        tem_norm = (B.pos - new_point1.pos).cross(B.pos - new_point3.pos);
        face2.normal = tem_norm.dot(face.normal) > 0 ? tem_norm : -tem_norm;

        face3.v[0] = id_BC;
        face3.v[1] = id_AC;
        face3.v[2] = face.v[2];
        tem_norm = (C.pos - new_point2.pos).cross(C.pos - new_point3.pos);
        face3.normal = tem_norm.dot(face.normal) > 0 ? tem_norm : -tem_norm;

        face4.v[0] = id_BC;
        face4.v[1] = id_AC;
        face4.v[2] = id_AB;
        tem_norm = (new_point2.pos - new_point1.pos).cross(new_point2.pos - new_point3.pos);
        face2.normal = tem_norm.dot(face.normal) > 0 ? tem_norm : -tem_norm;

        F_list_new.push_back(std::move(face1));
        F_list_new.push_back(std::move(face2));
        F_list_new.push_back(std::move(face3));
        F_list_new.push_back(std::move(face4));
    }
    // update old points
    for (int i = 0; i < old_point_size; ++i) {
        ref::point *p = &P_list[i];
        if (p->b_point.size() <= 2) {
            p->pos = p->pos * 3.0f / 4.0f;
            for(auto bp : p->b_point) {
                p->pos += P_list[bp].pos / 8.0f;
            }
        } else {
            double k = p->b_point.size();
            double b = (5.0f / 8.0f - (3.0f/8.0f + cos(2*M_PI/k) / 4.0f) * (3.0f/8.0f + cos(2*M_PI/k) / 4.0f)) / k;
            p->pos = p->pos * (1 -k*b);
            for (auto bp : p->b_point) {
                p->pos += b*P_list[bp].pos;
            }
        }
    }
    // filter data of points
    // old points swap b_point and n_point
    for (int j = 0; j < old_point_size; ++j) {
        ref::point *p = &P_list[j];
        p->n_point.clear();
        for (auto index : p->b_point) {
            p->n_point.push_back(index);
        }
        p->b_point.clear();
    }
}


void ref::make_face(
        std::vector<ref::point> P_list,
        std::vector<ref::face> F_list,
        std::vector<Eigen::Vector3f>& points, 
        std::vector<Eigen::Vector3f>& normals, 
        std::vector<Eigen::Vector2f>& uvs)
{
	return;
}
		

