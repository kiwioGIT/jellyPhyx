#pragma once
#include <algorithm>
#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <strstream>

class Vector2
{
public:

	float x;
	float y;

	Vector2 rotated(float phi) {
		return Vector2(cos(phi), sin(phi)) * x + Vector2(-sin(phi), cos(phi)) * y;
	}

	float length() {
		return sqrt(x * x + y * y);
	}

	float length_squared() {
		return x * x + y * y;
	}

	float dot(Vector2 v2) {
		return x * v2.x + y * v2.y;
	}

	Vector2 get_tangent() {
		return Vector2(-y, x);
	}

	Vector2 normalized() {
		return Vector2(x,y) / Vector2(x, y).length();
	}

	Vector2 reflected(Vector2 normal) {
		return Vector2(x, y) + normal * dot(normal) * 2.0;
	}

	Vector2 reflected_weighted(Vector2 normal,float weight) {
		return Vector2(x, y) + normal * dot(normal) * (1.0 + weight);
	}

	//Constructor
	Vector2(float sx,float sy) {
		x = sx;
		y = sy;
	}

	//operators
	Vector2 operator+(const Vector2 p_v) const;
	Vector2 operator-(const Vector2 p_v) const;
	Vector2 operator*(const Vector2 p_v) const;
	Vector2 operator*(const float p_v) const;
	Vector2 operator/(const Vector2 p_v) const;
	Vector2 operator/(const float p_v) const;
	Vector2 operator+=(const Vector2 p_v) const;
	Vector2 operator-=(const Vector2 p_v) const;

	float operator%(const Vector2 p_v) const;

};

//Operator Implementation
Vector2 Vector2::operator+(const Vector2 p_v) const {
	return Vector2(x + p_v.x, y + p_v.y);
}

Vector2 Vector2::operator+=(const Vector2 p_v) const {
	return Vector2(x + p_v.x, y + p_v.y);
}

Vector2 Vector2::operator-(const Vector2 p_v) const {
	return Vector2(x - p_v.x, y - p_v.y);
}

Vector2 Vector2::operator-=(const Vector2 p_v) const {
	return Vector2(x - p_v.x, y - p_v.y);
}

float Vector2::operator%(const Vector2 p_v) const {
	return sqrt((x - p_v.x) * (x - p_v.x) + (y - p_v.y) * (y - p_v.y));
}

Vector2 Vector2::operator*(const Vector2 p_v) const {
	return Vector2(x * p_v.x, y * p_v.y);
}

Vector2 Vector2::operator*(const float p_v) const {
	return Vector2(x * p_v, y * p_v);
}

Vector2 Vector2::operator/(const Vector2 p_v) const {
	return Vector2(x / p_v.x, y / p_v.y);
}

Vector2 Vector2::operator/(const float p_v) const {
	return Vector2(x / p_v, y / p_v);
}

float dot(Vector2 v1,Vector2 v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

float distance(Vector2 v1, Vector2 v2) {
	return sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

int get_line_intersection(Vector2 p0, Vector2 p1,Vector2 p2, Vector2 p3/*, float* i_x, float* i_y*/)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1.x - p0.x;     s1_y = p1.y - p0.y;
    s2_x = p3.x - p2.x;     s2_y = p3.y - p2.y;

    float s, t;
    s = (-s1_y * (p0.x - p2.x) + s1_x * (p0.y - p2.y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = (s2_x * (p0.y - p2.y) - s2_y * (p0.x - p2.x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        // Point of collision not important in this case

        //if (i_x != NULL)
        //    *i_x = p0_x + (t * s1_x);
        //if (i_y != NULL)
        //    *i_y = p0_y + (t * s1_y);
        return 1; 
    }
    return 0; // No collision
}



float min_dist_to_line_seg(Vector2 v, Vector2 w, Vector2 p) {
	// MAGIC
	const float l2 = (w - v).length_squared();
	if (l2 == 0.0) return distance(p, v);
	const float t = std::max(float(0.0), std::min(float(1.0), dot(p - v, w - v) / l2));
	const Vector2 projection = v + (w - v) * t; 
	return distance(p, projection);
}

Vector2 project_to_line_seg(Vector2 v, Vector2 w, Vector2 p) {
	// MAGIC
	const float l2 = (w - v).length_squared();
	if (l2 == 0.0) return v;
	const float t = std::max(float(0.0), std::min(float(1.0), dot(p - v, w - v) / l2));
	const Vector2 projection = v + (w - v) * t;
	return projection;
}

struct Ball {
	Vector2 pos;
	Vector2 velo;
	float size;
};

struct Point {
	Vector2 pos;
	Vector2 velo;
	float mass = 1.0;
};

struct OutlineStick {
	Point* point_A;
	Point* point_B;
};


struct Spring {
	Point* point_A;
	Point* point_B;
	float lenght;
	float stifness = 0.5;
	float damping = 0.1;
};

bool do_ranges_overlap(float min1, float max1, float min2, float max2) {
	return max1 >= min2 && min1 <= max2;
}


struct BB {
	Vector2 mini;
	Vector2 maxi;

	bool includes_v(Vector2 v) {
		if (v.x >= mini.x) {
			if (v.x <= maxi.x) {
				if (v.y >= mini.y) {
					if (v.y <= maxi.y) {
						return true;
					}
				}
			}
		}
		return false;
	}

	

	bool is_overlap(BB bb) {
		return (mini.x < bb.maxi.x && bb.mini.x < maxi.x && mini.y < bb.maxi.y && bb.mini.y < maxi.y);
	}

};

struct Body {
	std::vector<Point> points;
	std::vector<Spring> springs;
	std::vector<OutlineStick> outline_sticks;

	Vector2 center_point() {
		Vector2 val = Vector2(0, 0);
		for (int i = 0; i < points.size(); i++) {
			val += points[i].pos;
		}
		return val / points.size();
	}

	BB get_bb(){
		float margin = 10.0;

		float min_y = 999999999;
		float max_y = -999999999;
		float min_x = 9999999999;
		float max_x = -9999999999;

		for (int p = 0; p < points.size(); p++) {
			Vector2 pos = points[p].pos;
			min_y = std::min(min_y, pos.y);
			max_y = std::max(max_y, pos.y);
			min_x = std::min(min_x, pos.x);
			max_x = std::max(max_x, pos.x);
		}
		return BB{ Vector2(min_x - margin,min_y - margin),Vector2(max_x + margin,max_y + margin) };
	}

	void move(Vector2 move_v) {
		for (int p = 0; p < points.size(); p++) {
			points[p].pos = points[p].pos + move_v;
		}
	}

	void apply_velocity(Vector2 vel) {
		for (int p = 0; p < points.size(); p++) {
			points[p].velo = points[p].velo + vel;
		}
	}

	bool load_body_file(std::string file, float def_scale, float def_stiffnes_scale, float def_damping_scale, Vector2 pos) {
		points.clear();
		std::ifstream f(file);
		if (!f.is_open()) {
			return false;
		}
		float scale = def_scale;
		float stiffnes_scale = def_stiffnes_scale;
		float damping_scale = def_damping_scale;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream str;
			str << line;

			

			char j;

			if (line[0] == '#') {
				
			}

			if (line[0] == 'm') { // Scale , Stifness , Dampling mulitplier
				float ss;
				float st;
				float dd;

				str >> j >> ss >> st >> dd;
				
				scale = def_scale * ss;
				stiffnes_scale = def_stiffnes_scale * st;
				damping_scale = def_damping_scale * dd;
			}

			if (line[0] == 'p') { // Point
				Vector2 vec(0.0,0.0);
				
				str >> j >> vec.x >> vec.y;
				
				vec.x *= scale;
				vec.y *= scale;
				vec.x += pos.x;
				vec.y += pos.y;

				points.push_back(Point{vec,Vector2(0.0,0.0),1.0});
			}

			if (line[0] == 's') {// Spring
				Spring spring;
				int pa;
				int pb;
				float lenght;
				float stifness;
				float damping;
				str >> j >> pa >> pb >> lenght >> stifness >> damping;
				spring.point_A = &points[pa];
				spring.point_B = &points[pb];
				spring.lenght = lenght * scale;
				spring.stifness = stifness * stiffnes_scale;
				spring.damping = damping * damping_scale;
				springs.push_back(spring);
			}

			if (line[0] == 'o') // Outline stick
			{
				OutlineStick stick;
				int pa;
				int pb;
				str >> j >> pa >> pb;
				stick.point_A = &points[pa];
				stick.point_B = &points[pb];
				outline_sticks.push_back(stick);
			}
		}
		return true;

	}
	

};

float distance_to_nearest_stick(Vector2 v, Body body) {
    float min_dist = INFINITY;
    OutlineStick closest_stick;
    Vector2 projection = Vector2(0, 0);
    for (int s = 0; s < body.outline_sticks.size(); s++) {
        Vector2 projection = project_to_line_seg(body.outline_sticks[s].point_A->pos, body.outline_sticks[s].point_B->pos, v);
        float dist = distance(projection, v);
        if (dist < min_dist) {
            min_dist = dist;
            closest_stick = body.outline_sticks[s];
        }
    }
    return min_dist;
}

bool is_point_in_body(Vector2 pos,Body body){
    BB bb = body.get_bb();
    if (!bb.includes_v(pos)) { return false; }
    Vector2 point_out = Vector2(bb.maxi.x + 1.0, pos.y); // Note: Possible optimization using a horizontal line
    std::vector<OutlineStick> intersected_sticks;
    for (int s = 0; s < body.outline_sticks.size(); s++) {
        int intersect = get_line_intersection(pos, point_out, body.outline_sticks[s].point_A->pos, body.outline_sticks[s].point_B->pos);
        if (intersect == 1) {
            intersected_sticks.push_back(body.outline_sticks[s]);
        }
    }
    if (intersected_sticks.size() % 2 != 0) {
        return true;
    }
    return false;
}