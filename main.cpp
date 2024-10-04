#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <cstdint>
#include <thread>

#include "PhyLib.h"

using namespace std;

vector<Body> bodys;

Vector2 gravity = Vector2(0, 0.125);

static std::condition_variable cv;
static std::mutex mut;
static bool stop = false;

float pi = 3.14159265359;
float ball_size = 10;
float speed_scale = 0.1;

int screen_width = 800;
int screen_height = 600;
float fract_f = 0.1;
float point_simulated_radius = 0.05;
int solve_iterations = 5;
float normal_bounce_eff = -1;
float tangent_bounce_eff = 0.9;
float body_body_normal_bouncienes = 1.0;
float body_body_tangent_bouncienes = 1.0;
int draw_springs_and_points = -1;

void draw_points(sf::RenderWindow* window);
void draw_springs(sf::RenderWindow* window);
void draw_sticks(sf::RenderWindow* window);
void update_points();
void solve_wall_collision();
void solve_springs();
void solve_stick_collisions();
float distance_to_nearest_stick(Vector2 v, Body body);
void draw_circle(Vector2 pos, float radius, sf::Color, sf::RenderWindow* window);
bool is_point_in_body(Vector2 pos, Body body);
void solve_bodys();
bool load_cfg(std::string file);

void pis(string s) {
    std::cout << s << std::endl;
}

void main_loop() {
    sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "jellyPhx");
    using delta = std::chrono::duration<std::int64_t, std::ratio<1,600>>;
    auto next = std::chrono::steady_clock::now() + delta{ 1 };
    std::unique_lock<std::mutex> lk(mut);
    while (!stop)
    {
        mut.unlock();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                {
                    window.close();
                    std::terminate();
                }
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::D) {
                    draw_springs_and_points *= -1;
                }
                if (event.key.code == sf::Keyboard::R) {
                    load_cfg("setup.cfg");
                }
            }

        }
        
        float speed = -1 * speed_scale;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            bodys[0].apply_velocity(Vector2(speed,0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            bodys[0].apply_velocity(Vector2(-speed, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            bodys[0].apply_velocity(Vector2(0,speed));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            bodys[0].apply_velocity(Vector2(0,-speed));
        }

        // Physics sequence
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            update_points();
            solve_springs();
            solve_wall_collision();
            solve_bodys();
        }
        
        // Draw sfml sequence
        window.clear();
        draw_sticks(&window);

        if (draw_springs_and_points == 1) {
            draw_springs(&window);
            draw_points(&window);
        }

        window.display();

        // Wait for the next 1/60 sec
        mut.lock();
        cv.wait_until(lk, next, [] {return false; });
        next += delta{ 1 };
    }
}

bool load_cfg(std::string file){
    bodys.clear();
	std::ifstream f(file);
	if (!f.is_open()) {return false;}
	while (!f.eof()){
        string b_to_load = "";
        float b_to_load_x;
        float b_to_load_y;
		char line[128];
		f.getline(line, 128);
		std::strstream str;
		str << line;
		char j;
		if (line[0] == '#') {}
        if (line[0] == 'g'){
            str >> j >> gravity.x >> gravity.y;
        }
		if (line[0] == 'w') {
            str >> j >> screen_width >> screen_height;
		}
		if (line[0] == 't') {
            str >> j >> speed_scale;
		}
        if (line[0] == 'l'){
            str >> j >> b_to_load >> b_to_load_x >> b_to_load_y;
        }
        if (b_to_load != ""){
            Body new_body;
            bodys.push_back(new_body);
            bodys[bodys.size() - 1].load_body_file(b_to_load,1,1,1,Vector2(b_to_load_x,b_to_load_y));
        }
	}
}


int main()
{

    load_cfg("setup.cfg");
    main_loop();

    //
    // Threaded implementation of loop
    //
    //using namespace std::chrono_literals;
    //std::thread t{ main_loop };
    //std::this_thread::sleep_for(1000000s);
    //{
    //    std::lock_guard<std::mutex> lk(mut);
    //    stop = true;
    //}
    //t.join();
    return 0;
}

void draw_circle(Vector2 pos, float radius,sf::Color color,sf::RenderWindow* window) {
    sf::CircleShape circle_shape(radius);
    circle_shape.setPosition(sf::Vector2f(pos.x - radius, pos.y - radius));
    circle_shape.setFillColor(color);
    window->draw(circle_shape);
}

void draw_points(sf::RenderWindow* window) {
    for (int body_i = 0; body_i < bodys.size(); body_i++) {
        for (int point_i = 0; point_i < bodys[body_i].points.size(); point_i++) {
            sf::CircleShape circle_shape(3);
            circle_shape.setPosition(sf::Vector2f(bodys[body_i].points[point_i].pos.x - 3, bodys[body_i].points[point_i].pos.y - 3));
            circle_shape.setFillColor(sf::Color::Red);
            window->draw(circle_shape);
        }
    }
}

void draw_springs(sf::RenderWindow* window) {
    for (int b = 0; b < bodys.size(); b++) {
        for (int s = 0; s < bodys[b].springs.size(); s++) {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(bodys[b].springs[s].point_A->pos.x, bodys[b].springs[s].point_A->pos.y)),
                sf::Vertex(sf::Vector2f(bodys[b].springs[s].point_B->pos.x, bodys[b].springs[s].point_B->pos.y))
            };
            line[0].color = sf::Color::Green;
            line[1].color = sf::Color::Green;
            window->draw(line, 2, sf::Lines);
        }
    }
}

void draw_sticks(sf::RenderWindow* window) {
    float thickness = 2;
    for (int b = 0; b < bodys.size(); b++) {
        for (int s = 0; s < bodys[b].outline_sticks.size(); s++) {
            sf::CircleShape circle_shape(3.0);
            Vector2 normal = (bodys[b].outline_sticks[s].point_A->pos - bodys[b].outline_sticks[s].point_B->pos).normalized().get_tangent();
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(bodys[b].outline_sticks[s].point_A->pos.x + normal.x * thickness, bodys[b].outline_sticks[s].point_A->pos.y + normal.y * thickness)),
                sf::Vertex(sf::Vector2f(bodys[b].outline_sticks[s].point_A->pos.x - normal.x * thickness, bodys[b].outline_sticks[s].point_A->pos.y - normal.y * thickness)),
                sf::Vertex(sf::Vector2f(bodys[b].outline_sticks[s].point_B->pos.x + normal.x * thickness, bodys[b].outline_sticks[s].point_B->pos.y + normal.y * thickness)),
                sf::Vertex(sf::Vector2f(bodys[b].outline_sticks[s].point_B->pos.x - normal.x * thickness, bodys[b].outline_sticks[s].point_B->pos.y - normal.y * thickness))
            };
            window->draw(line, 4, sf::TrianglesStrip);  
        }
    }
}

void update_points() {
    for (int b = 0; b < bodys.size(); b++) {
        for (int i = 0; i < bodys[b].points.size(); i++) {
            bodys[b].points[i].velo = bodys[b].points[i].velo + gravity * speed_scale;
            bodys[b].points[i].pos = bodys[b].points[i].pos + bodys[b].points[i].velo * speed_scale;
        }
    }
}

// Main collision solving function
void solve_bodys() {
    for (int b1 = 0; b1 < bodys.size(); b1++) {
        Body *body1 = &bodys[b1];
        for (int b2 = 0; b2 < bodys.size(); b2++) {
            if (b1 == b2) { continue; }
            Body* body2 = &bodys[b2];
            if (!body1->get_bb().is_overlap(body2->get_bb())) { continue; } // If bounding boxes dont overlap skip
            
            for (int p = 0; p < body1->points.size(); p++) {
                
                if (!is_point_in_body(body1->points[p].pos, *body2)) {continue;} // If point is outside outer body skip
                 
                // Gets data for stick-point solve
                float min_dist = INFINITY;
                OutlineStick closest_stick = body2->outline_sticks[0];
                Vector2 closest_projection = Vector2(0, 0);
                for (int s = 0; s < body2->outline_sticks.size(); s++) {
                    Vector2 projection = project_to_line_seg(body2->outline_sticks[s].point_A->pos, body2->outline_sticks[s].point_B->pos, body1->points[p].pos);
                    float dist = distance(projection, body1->points[p].pos);
                    if (dist < min_dist) {
                        min_dist = dist;
                        closest_stick = body2->outline_sticks[s];
                        closest_projection = projection;
                    }
                }
                
                // Updates positions
                float stick_lenght = distance(closest_stick.point_A->pos,closest_stick.point_B->pos);
                float to_point_lenght = distance(closest_stick.point_A->pos, closest_projection);
                float t = to_point_lenght / stick_lenght;

                Vector2 vs = closest_projection - body1->points[p].pos;

                if (abs(t) < 0.0001 || abs(t) > 0.9999){continue;} // t value causes bugs if too small so its better to skip entirely

                float vsa = (t - 1) / (2 - 2 * t + 2 * t * t);
                float vsb = vsa * (t / (1 - t));
                float vsp =  -vsa - vsb;
                closest_stick.point_A->pos = closest_stick.point_A->pos + vs * vsa;
                closest_stick.point_B->pos = closest_stick.point_B->pos + vs * vsb;
                body1->points[p].pos = body1->points[p].pos + vs * vsp;

                Vector2 stick_avarage_velo = (closest_stick.point_A->velo + closest_stick.point_B->velo) * 0.5;
                
                float vs_lenght = vs.length();

                if (vs_lenght < 0.0001){continue;} // Fixes a near zero division bug

                Vector2 normal = vs/vs_lenght;
                Vector2 tangent = Vector2(-normal.y, normal.x);

                float p_nor_dp = dot(normal, body1->points[p].velo);
                float s_nor_dp = dot(normal, stick_avarage_velo);

                float p_tan_dp = dot(tangent, body1->points[p].velo);
                float s_tan_dp = dot(tangent, stick_avarage_velo);

                float b1mass = 1.0; 
                float b2mass = 2.0; // Stick weight is set to be double the point weight for now

                float p_nor_m = (p_nor_dp * (b1mass - b2mass) + 2.0f * b2mass * s_nor_dp) / (b1mass + b2mass); //point normal motion
                float s_nor_m = (s_nor_dp * (b2mass - b1mass) + 2.0f * b1mass * p_nor_dp) / (b1mass + b2mass); //stick normal motion

                Vector2 new_point_velo = normal * p_nor_m * body_body_normal_bouncienes + tangent * p_tan_dp * body_body_tangent_bouncienes;
                Vector2 new_stick_velo = normal * s_nor_m * body_body_normal_bouncienes + tangent * s_tan_dp * body_body_tangent_bouncienes;

                body1->points[p].velo = new_point_velo;
                closest_stick.point_A->velo = new_stick_velo;
                closest_stick.point_B->velo = new_stick_velo;
            }
        }
    }
}

// Solves all spring weighted displacement
void solve_springs() {
    for (int b = 0; b < bodys.size(); b++) {
        for (int s = 0; s < bodys[b].springs.size(); s++) {
            Spring spring = bodys[b].springs[s];
            Vector2 dist_v = spring.point_B->pos - spring.point_A->pos;
            float dist_f = dist_v.length();
            Vector2 s_normal = dist_v.normalized();
            float relative_velo = s_normal.dot(spring.point_B->velo - spring.point_A->velo);
            Vector2 shift_vector = dist_v * 0.5 * ((spring.lenght - dist_f) / dist_f);
            spring.point_A->velo = spring.point_A->velo - (shift_vector * spring.stifness * speed_scale - s_normal * relative_velo * spring.damping * speed_scale)/spring.point_A->mass;
            spring.point_B->velo = spring.point_B->velo + (shift_vector * spring.stifness * speed_scale - s_normal * relative_velo * spring.damping * speed_scale)/spring.point_B->mass;
        }
    }
}

// Collision between points and screen boundary, looks ugly, probably will be ommited from the final build , 
void solve_wall_collision() {
    for (int b = 0; b < bodys.size(); b++) {
        for (int i = 0; i < bodys[b].points.size(); i++) {
            if (bodys[b].points[i].pos.y + bodys[b].points.size() > screen_height) {
                bodys[b].points[i].pos.y = screen_height -bodys[b].points.size() - fract_f;
                bodys[b].points[i].velo.y *= normal_bounce_eff;
                bodys[b].points[i].velo.x *= tangent_bounce_eff;
            }
            if (bodys[b].points[i].pos.y - bodys[b].points.size() < 0) {
                bodys[b].points[i].pos.y = 0 +bodys[b].points.size() + fract_f;
                bodys[b].points[i].velo.y *= normal_bounce_eff;
                bodys[b].points[i].velo.x *= tangent_bounce_eff;
            }
            if (bodys[b].points[i].pos.x + bodys[b].points.size() > screen_width) {
                bodys[b].points[i].pos.x = screen_width -bodys[b].points.size() - fract_f;
                bodys[b].points[i].velo.x *= normal_bounce_eff;
                bodys[b].points[i].velo.y *= tangent_bounce_eff;
            }
            if (bodys[b].points[i].pos.x - bodys[b].points.size() < 0) {
                bodys[b].points[i].pos.x = 0 + bodys[b].points.size() + fract_f;
                bodys[b].points[i].velo.x *= normal_bounce_eff;
                bodys[b].points[i].velo.y *= tangent_bounce_eff;
            }
        }
    }
}
