#pragma once
#include <vector>
#include "shape.hpp"
#include "objloader.hpp"
#include "refine.hpp"

class TriangleMesh : public Shape
{
public:
	// number of triangles
	int triangleCount;
	// verteces, normals and indices data
	std::vector<Eigen::Vector3f> out_vertices;
	std::vector<Eigen::Vector3f> out_normals;
	std::vector<int> out_v_index;
	std::vector<int> out_vn_index;
	// uniform grid data
	bool isUniformExisting;
	std::vector<std::vector<Eigen::Vector3i>> grid;
	int grid_step;
	Eigen::Vector3i gridDim;
	Eigen::Vector3f gridDeltaDist;
	
	explicit TriangleMesh(const Eigen::Vector3f& color, std::string filePos)
		: Shape(color)
	{
		std::vector <Eigen::Vector2f> out_uvs;
		std::vector <int> out_vt_index;
		loadOBJ_index(filePos.c_str(), out_vertices, out_uvs, out_normals, out_v_index, out_vt_index, out_vn_index);
		
		triangleCount = out_v_index.size() / 3;
		
		m_BoundingBox.lb = out_vertices[0].cwiseMin(out_vertices[1]);
		m_BoundingBox.ub = out_vertices[0].cwiseMax(out_vertices[1]);
		for (int i = 2; i < out_vertices.size(); i++)
		{
			m_BoundingBox.lb = m_BoundingBox.lb.cwiseMin(out_vertices[i]);
			m_BoundingBox.ub = m_BoundingBox.ub.cwiseMax(out_vertices[i]);
		}

		isUniformExisting = false;
	}

	// ray intersection with single triangle, result saves in @Interaction
	bool raySingleTriangleIntersection(Interaction& interaction, const Ray& ray, int v0_idx, int v1_idx, int v2_idx)
	{
		/*
		 * Note:
		 * 1. If intersected, fill every variable in @Interaction class except for @Interaction.exitDist.
		 * 2. Normals are interpolated using barycentric coordinates.
		 */
		Eigen::Vector3f A = out_vertices[out_v_index[v0_idx]],
		B = out_vertices[out_v_index[v1_idx]],
		C = out_vertices[out_v_index[v2_idx]],
		P = ray.m_Ori + ray.m_fMin * ray.m_Dir,
		// Q is one unit away from ray source point
		Q = ray.m_Ori + ray.m_Dir + + ray.m_fMin * ray.m_Dir,
		n, e;
		float t, v, w, d;
		n = (B - A).cross(C - A);
        d = (P - Q).dot(n);
        if (d == 0) {
            return false;
        }
        e = (P - Q).cross(P - A);
        t = (P - A).dot(n)/d;
        v = (C - A).dot(e)/d;
        w = -(B - A).dot(e)/d;
        // pass condition
        if (t >= 0 && v >= 0 && w >= 0 && w + v <= 1) {
            interaction.entryPoint = P + t*(Q - P);
            interaction.entryDist = t;
            interaction.normal = out_normals[out_vn_index[v2_idx]];
            interaction.uv = Eigen::Vector2f(w, v);
            interaction.surfaceColor = color;
            return true;
        }
        return false;
	}

    static bool raySingleTriangleIntersection(Interaction& interaction, const Ray& ray, std::vector<Eigen::Vector3f> points)
    {
        /*
         * Note:
         * 1. If intersected, fill every variable in @Interaction class except for @Interaction.exitDist.
         * 2. Normals are interpolated using barycentric coordinates.
         */
        Eigen::Vector3f A = points[0],
                B = points[1],
                C = points[2],
                P = ray.m_Ori + ray.m_fMin * ray.m_Dir,
        // Q is one unit away from ray source point
        Q = ray.m_Ori + ray.m_Dir + + ray.m_fMin * ray.m_Dir,
                n, e;
        float t, v, w, d;
        n = (B - A).cross(C - A);
        d = (P - Q).dot(n);
        if (d == 0) {
            return false;
        }
        e = (P - Q).cross(P - A);
        t = (P - A).dot(n)/d;
        v = (C - A).dot(e)/d;
        w = -(B - A).dot(e)/d;
        // pass condition
        if (t >= 0 && v >= 0 && w >= 0 && w + v <= 1) {
            interaction.entryPoint = P + t*(Q - P);
            return true;
        }
        return false;
    }

	// ray intersection with mesh, result saves in @Interaction
	bool rayIntersection(Interaction& interaction, const Ray& ray) override
	{
		Interaction finalInteraction;
		if (isUniformExisting) {
			/*
			 * Note: Grid traversal algorithm must be used here.
			 */
			// find the cell passed by ray
            double Nx = abs(m_BoundingBox.ub.x() - m_BoundingBox.lb.x());
            double Ny = abs(m_BoundingBox.ub.y() - m_BoundingBox.lb.y());
            double Nz = abs(m_BoundingBox.ub.z() - m_BoundingBox.lb.z());

            std::vector<Eigen::Vector3f> store;
			Eigen::Vector3f b_1 = m_BoundingBox.lb,
			b_2 = m_BoundingBox.lb + Eigen::Vector3f(0, Ny, 0),
			b_3 = m_BoundingBox.ub + Eigen::Vector3f(-Nx, 0, 0),
			b_4 = m_BoundingBox.lb + Eigen::Vector3f(0, 0, Nz),
			b_5 = m_BoundingBox.lb + Eigen::Vector3f(Nx, 0, 0),
			b_6 = m_BoundingBox.ub + Eigen::Vector3f(0, 0, -Nz),
			b_7 = m_BoundingBox.ub,
			b_8 = m_BoundingBox.ub + Eigen::Vector3f(0, -Ny, 0);
			// find the point intersect with bounding box
			// front face
			store.push_back(b_1);
            store.push_back(b_2);
            store.push_back(b_3);
            store.push_back(b_1);
            store.push_back(b_4);
            store.push_back(b_3);
            // upper face
            store.push_back(b_3);
            store.push_back(b_4);
            store.push_back(b_8);
            store.push_back(b_7);
            store.push_back(b_8);
            store.push_back(b_3);
            // right face
            store.push_back(b_1);
            store.push_back(b_5);
            store.push_back(b_4);
            store.push_back(b_5);
            store.push_back(b_4);
            store.push_back(b_8);
            // left face
            store.push_back(b_7);
            store.push_back(b_2);
            store.push_back(b_3);
            store.push_back(b_7);
            store.push_back(b_6);
            store.push_back(b_2);
            // bottom face
            store.push_back(b_1);
            store.push_back(b_2);
            store.push_back(b_5);
            store.push_back(b_5);
            store.push_back(b_6);
            store.push_back(b_2);
            // back face
            store.push_back(b_7);
            store.push_back(b_8);
            store.push_back(b_5);
            store.push_back(b_5);
            store.push_back(b_6);
            store.push_back(b_7);

            // find the cell intersect with the ray
			Interaction box_intersection;
			Eigen::Vector3f intersect_point(-1, -2, -3);
            for (int i = 0; i < 12; i+=3) {
                std::vector<Eigen::Vector3f> input;
                input.reserve(3);
                for (int j = 0; j < 3; ++j) {
                    input.push_back(store[i * 3 + j]);
                }
                if (raySingleTriangleIntersection(box_intersection, ray, input)) {
                    intersect_point = box_intersection.entryPoint;
                    break;
                }
            }
            if (intersect_point == Eigen::Vector3f (-1, -2, -3)) {
                return false;
            }
            int index_x, index_y, index_z;
            // std::cout<<"lb ub: "<<m_BoundingBox.lb<<std::endl<<m_BoundingBox.ub<<std::endl;
            // std::cout<<"intersect point: "<<std::endl<<intersect_point<<std::endl;
            // std::cout<<abs(intersect_point.x() - m_BoundingBox.lb.x()) / Nx<<std::endl;
            index_x = (abs(intersect_point.x() - m_BoundingBox.lb.x()) / Nx) * grid_step;
            index_y = (abs(intersect_point.y() - m_BoundingBox.lb.y()) / Ny) * grid_step;
            index_z = (abs(intersect_point.z() - m_BoundingBox.lb.z()) / Nz) * grid_step;
            Eigen::Vector3f ray_dir = ray.m_Dir.normalized();
            double delta_x = Nx / ray_dir.x();
            double delta_y = Ny / ray_dir.y();
            double delta_z = Nz / ray_dir.z();

            std::vector<int> index_cell;
            double tx = delta_x, ty = delta_y, tz = delta_z;

            // add intersect cell into index_cell
            // std::cout<<"here"<<std::endl;
            // std::cout<<"orignal index x y z: "<<index_x<<" "<<index_y<<" "<<index_z<<std::endl;
            while(index_x <= grid_step - 1 && index_x >= 0 && index_y <= grid_step - 1 && index_y >= 0 && index_z <= grid_step - 1 && index_z >= 0) {
                index_cell.push_back(index_x);
                index_cell.push_back(index_y);
                index_cell.push_back(index_z);
                if (std::min(std::min(tx, ty), tz) == tx) {
                    index_x ++;
                    tx += delta_x;
                } else if (std::min(std::min(tx, ty), tz) == ty) {
                    index_y ++;
                    ty += delta_y;
                } else {
                    index_z ++;
                    tz += delta_z;
                }
                // std::cout<<"push index x y z: "<<index_x<<" "<<index_y<<" "<<index_z<<std::endl;
            }
            // find intersection
            // std::cout<<index_cell.size()<<std::endl;
            for (int i = 0; i < index_cell.size() / 3; i++) {
                Interaction curInteraction;
                std::vector<Eigen::Vector3i> cell_index = grid[index_cell[3 * i] * pow(grid_step, 2)
                                                               + index_cell[3 * i + 1] * grid_step + index_cell[3 * i + 2]];
                for (auto cell : cell_index) {
                    if (raySingleTriangleIntersection(curInteraction, ray, cell.x(), cell.y(), cell.z())) {
                        if (finalInteraction.entryDist == -1 || curInteraction.entryDist < finalInteraction.entryDist) {
                            finalInteraction = curInteraction;
                        }
                    }
                }
            }
        } else {
			for (int i = 0; i < triangleCount; i++) {
				Interaction curInteraction;
				if (raySingleTriangleIntersection(curInteraction, ray, 3 * i, 3 * i + 1, 3 * i + 2)) {
					if (finalInteraction.entryDist == -1 || curInteraction.entryDist < finalInteraction.entryDist) {
						finalInteraction = curInteraction;
					}
				}
			}
		}

		if (finalInteraction.entryDist != -1)
		{
			interaction = finalInteraction;
			return true;
		}
		return false;
	}

	// apply a certain transformation to all vertices, normals and bounding box
	void applyTransformation(const Eigen::Affine3f& t)
	{
		for (auto & out_vertice : out_vertices)
		{
			out_vertice = t * out_vertice;
		}

		Eigen::Matrix3f tInvTr = t.linear().inverse().transpose();
		for (auto & out_normal : out_normals)
		{
			out_normal = (tInvTr * out_normal).normalized();
		}

		m_BoundingBox.lb = out_vertices[0].cwiseMin(out_vertices[1]);
		m_BoundingBox.ub = out_vertices[0].cwiseMax(out_vertices[1]);
		for (int i = 2; i < out_vertices.size(); i++)
		{
			m_BoundingBox.lb = m_BoundingBox.lb.cwiseMin(out_vertices[i]);
			m_BoundingBox.ub = m_BoundingBox.ub.cwiseMax(out_vertices[i]);
		}
	}

	void subdivision()
	{
		std::vector<ref::point> P_list;
		std::vector<ref::line> L_list;
		std::vector<ref::face> F_list;
        //std::cout<<"p_list size: "<<P_list.size()<<" F_list size: "<<F_list.size()<<std::endl;
		ref::build_data_struct(
			out_vertices, out_normals,
			out_v_index, out_vn_index,
			P_list, L_list, F_list);
		//std::cout<<"p_list size: "<<P_list.size()<<" F_list size: "<<F_list.size()<<std::endl;
		std::vector<ref::point> P_list_new;
		std::vector<ref::line> L_list_new;
		std::vector<ref::face> F_list_new;

		
		int division_time = 3;
        //std::cout<<"vertices size: "<<out_vertices.size()<<" vertices indexs: "<<out_v_index.size()<<std::endl;

		ref::loop_subdivision(
			P_list, F_list, F_list_new);
		F_list.clear();
        ref::loop_subdivision(
                P_list, F_list_new, F_list);
        F_list_new.clear();
        ref::loop_subdivision(
                P_list, F_list, F_list_new);
		
		// add new points to out vertices
        //std::cout<<"p_list size: "<<P_list.size()<<" F_list size: "<<F_list_new.size()<<std::endl;
        out_vertices.clear();
        for (const auto& point : P_list) {
            out_vertices.push_back(point.pos);
            //std::cout<<"push vertice: "<<i<<std::endl<<P_list[i].pos<<std::endl;
        }
        out_normals.clear();
        out_v_index.clear();
        out_vn_index.clear();
        for (const auto& face :F_list_new) {
            // add vertice index and normals
            int normal_index = out_normals.size();
            out_normals.push_back(face.normal);
            for (auto index_p : face.v) {
                out_v_index.push_back(index_p);
                out_vn_index.push_back(normal_index);
            }
        }
        //std::cout<<"vertices size: "<<out_vertices.size()<<" vertices indexs: "<<out_v_index.size()<<std::endl;
		// Finally, update the new size
		triangleCount = out_v_index.size() / 3;
	}


	void buildUniformGrid() {
		// generate girds
		//count grid dimension
		double Nx = abs(m_BoundingBox.ub.x() - m_BoundingBox.lb.x());
        double Ny = abs(m_BoundingBox.ub.y() - m_BoundingBox.lb.y());
        double Nz = abs(m_BoundingBox.ub.z() - m_BoundingBox.lb.z());
        grid_step = pow(triangleCount * 4 / (Nx * Ny *Nz), 0.3);
        double dx = Nx / grid_step;
        double dy = Ny / grid_step;
        double dz = Nz / grid_step;
        gridDeltaDist = Eigen::Vector3f(dx, dy, dz);
        // x
        for (int x = 0; x < grid_step; ++x) {
            // y
            for (int y = 0; y < grid_step; ++y) {
                // z
                for (int z = 0; z < grid_step; ++z) {
                    std::vector<Eigen::Vector3i> result;
                    // generate ray to test
                    Ray ray1(m_BoundingBox.lb + Eigen::Vector3f(x * dx, y *dy, z * dz),
                            (m_BoundingBox.ub - m_BoundingBox.lb).normalized());
                    Ray ray2(m_BoundingBox.lb + Eigen::Vector3f(x * dx + dx, y *dy, z * dz + dz),
                            (m_BoundingBox.lb + Eigen::Vector3f(x * dx + dx, y *dy, z * dz + dz)) - (m_BoundingBox.lb + Eigen::Vector3f(x * dx, y *dy + dy, z * dz)).normalized());
                    int j = 0;
                    for (int i = 0; i < triangleCount; i++) {
                        Interaction curInteraction;
                        if (raySingleTriangleIntersection(curInteraction, ray1, 3 * i, 3 * i + 1, 3 * i + 2) ||
                                raySingleTriangleIntersection(curInteraction, ray2, 3 * i, 3 * i + 1, 3 * i + 2)) {
                            // ray1 or ray2 intersect with current triangle
                            Eigen::Vector3i a(3 * i, 3 * i + 1, 3 * i + 2);
                            result.emplace_back(a);
                            j++;
                        }
                    }
                    //std::cout<<"faces in one vertices: "<<j<<std::endl;
                    // grid[x * step^2 + y * step + z]
                    grid.push_back(result);
                }
            }
        }
		// Don't forget to change isUniformExisting to true after building the grid
		isUniformExisting = false;
	}

};
