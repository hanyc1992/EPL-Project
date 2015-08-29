#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "yh7483.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

#ifdef _MSC_VER
using namespace epl;
#endif
using namespace std;
using String = std::string;

Initializer<yh7483> __yh7483_initializer;

/****************************************************check if it's me*************/
double my_speed(double speed) {
	speed = (int)(speed * 100) / 100.0;
	speed += 0.001616;
	return speed;
}

bool check_speed(double speed) {
	int temp = speed * 1000000;
	return (temp % 10000 == 1616);
}
/*************************************************threshold for spawn*************/
double spawn_thres() {
	if (encounter_strategy == UNDERDOG_IS_HERE)
		return 3.0;
	if (encounter_strategy == BIG_GUY_WINS)
		return 8.0;
	return 4.0;
}
/***********************************************************************************/
struct obj {
	double dist;
	double bearing;
	std::string name;
	obj(double dist, double bearing, std::string name) :dist(dist), bearing(bearing), name(name) {};
};

struct comp_obj {
	bool operator()(const obj& a, const obj& b) {
		return a.dist < b.dist;
	}
};
/*********************************************************************************/
double pi_trans(double in) {
	while (in > M_PI) {
		in = in - M_PI;
	}
	return in;
}

String yh7483::player_name(void) const {
	return "yh7483";
}

String yh7483::species_name(void) const
{
	return name;
}

Action yh7483::encounter(const ObjInfo& info)
{
	if (check_speed(info.their_speed)) {///////////////////////////////////////////////
										/* don't be cannibalistic */
		set_course(info.bearing + M_PI);
		return LIFEFORM_IGNORE;
	}
	else {
		hunt_event->cancel();
		SmartPointer<yh7483> self = SmartPointer<yh7483>(this);

		last_eat = Event::now();

		if (encounter_strategy == UNDERDOG_IS_HERE && health() > 2.0)
			spawn();
		if (encounter_strategy == FASTER_GUY_WINS)
			set_speed(5 + 10.0 * drand48());//////////////////////////////////////////////speed up
		if (encounter_strategy == SLOWER_GUY_WINS)
			set_speed(0);////////////////////////////////////////////////////////////////slow_down
		name = info.species;/////////////////////////////////////////////////////////////pretend

		hunt_event = new Event(0.0, [self](void) { self->escape(); });
		return LIFEFORM_EAT;
	}
}

void yh7483::initialize(void) {
	LifeForm::add_creator(yh7483::create, "yh7483");
}

/*
* REMEMBER: do not call set_course, set_speed, perceive, or reproduce
* from inside the constructor!!!!
* you must wait until the object is actually alive
*/
yh7483::yh7483() {
	SmartPointer<yh7483> self = SmartPointer<yh7483>(this);
	new Event(0, [self](void) { self->startup(); });
}

yh7483::~yh7483() {}

void yh7483::startup(void) {
	last_eat = Event::now();

	set_course(drand48() * 2.0 * M_PI);
	set_speed(2 + 5.0 * drand48());
	SmartPointer<yh7483> self = SmartPointer<yh7483>(this);
	hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void yh7483::spawn(void) {
	SmartPointer<yh7483> child = new yh7483;
	reproduce(child);
}


Color yh7483::my_color(void) const {
	return MAGENTA;
}

SmartPointer<LifeForm> yh7483::create(void) {
	return new yh7483;
}


void yh7483::hunt(void) {
	const String fav_food = "Algae";
	double sqrt_health = sqrt(health());

	hunt_event = nullptr;
	if (health() == 0.0) { return; } // we died

	double course_old = get_course();

	ObjList prey;
	prey = perceive(15 * spawn_thres() - 20 * sqrt_health);

	double best_d = HUGE;
	for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
		if (!check_speed((*i).their_speed)) {
			if (best_d > (*i).distance) {
				set_course((*i).bearing);
				best_d = (*i).distance;
				name = (*i).species;
			}
		}
	}
	double course_new = get_course();
	if (course_old == course_new)
		set_course(M_PI + course_old);

	SmartPointer<yh7483> self = SmartPointer<yh7483>(this);
	hunt_event = new Event(10.0, [self](void) { self->hunt(); });

	if (health() >= spawn_thres()) spawn();

}


void yh7483::escape() {
	const String fav_food = "Algae";
	double sqrt_health = sqrt(health());
	hunt_event = nullptr;
	if (health() == 0.0) { return; } // we died

	ObjList prey;
	//prey = perceive(20);
	prey = perceive(15 * spawn_thres() - 20 * sqrt_health);

	bool find_algea = false;
	double course_old = get_course();

	double best_d = HUGE;
	for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
		if ((*i).species != fav_food) {
			if (best_d > (*i).distance) {
				set_course((*i).bearing + M_PI);
				best_d = (*i).distance;
				name = (*i).species;
			}
		}
		else
			find_algea = true;
	}
	if (!find_algea)
		set_course(M_PI + course_old);


	SmartPointer<yh7483> self = SmartPointer<yh7483>(this);
	hunt_event = new Event(digestion_time, [self](void) { self->hunt(); });

	if (health() >= spawn_thres()) spawn();
}



