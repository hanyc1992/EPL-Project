/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

#ifndef M_PI
#	define M_PI ((double) 3.1415926535897932)
#endif

using namespace std;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
}



ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
	ObjInfo info;

	info.species = neighbor->species_name();
	info.health = neighbor->health();
	info.distance = pos.distance(neighbor->position());
	info.bearing = pos.bearing(neighbor->position());
	info.their_speed = neighbor->speed;
	info.their_course = neighbor->course;
	return info;
}

void LifeForm::border_cross(void) {
	if(!is_alive) return;
	//do I need to delete the Event here??
	update_position();
	check_encounter();
	compute_next_move();
}

void LifeForm::compute_next_move(void) {
	if(!is_alive) return;
	if(border_cross_event) border_cross_event->cancel();
	double dis = space.distance_to_edge(pos, course);
	//if stationary, no event
	if(speed > Point::tolerance) { //or within a tolerable range??????
		SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
		border_cross_event = new Event(dis/speed+Point::tolerance, [self](void) { self->border_cross(); });
	}

}

void LifeForm::update_position(void) {
	if(!is_alive) return;
	SimTime elapse_time = Event::now() - update_time;
	if(elapse_time > 0.001) { //Time::tolerance
		gain_energy(-movement_cost(speed, elapse_time));
		if(!is_alive) return; //die after charge energy penalty
		double dist = elapse_time * speed;
		//why????????????????????
		//NOTE: if I need to change here, i need to change other places using space.update_position as well
#if 1
		Point old_pos = pos;
		Point new_pos = pos;
		new_pos.xpos += dist*cos(course);
		new_pos.ypos += dist*sin(course);
		if(space.is_out_of_bounds(new_pos)) { die(); return; }
		update_time = Event::now();
		pos = new_pos;
		space.update_position(old_pos, pos);

#endif
#if 0
		Point new_pos = pos;
		new_pos.xpos += dist*cos(course);
		new_pos.ypos += dist*sin(course);
		//should first set the update_time, otherwise the loop will not end
		update_time = Event::now();
		space.update_position(new_pos, pos);
		pos = new_pos;
		if(space.is_out_of_bounds(pos)) die();
#endif
// I will not implement this function for the time being
// can use intersect in QuadTree
//		space.update_all_affected(pos); //a new const function in QuadTree that updates all the objects in the affected regions
	}
}

void LifeForm::region_resize(void) {
	//no need tp check_encounter if b is the one who's region get resized
	//because a will check
	if(!is_alive) return;
	update_position();
	compute_next_move();
}

void LifeForm::eat(SmartPointer<LifeForm> eatee) {
	assert(eatee->is_alive);
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	double eng = eatee->energy;
	//(void) new Event(digestion_time, [self](void){ self->gain_energy((eatee->energy)*eat_efficiency); });
	(void) new Event(digestion_time, [self, eng](void){ self->gain_energy(eng*eat_efficiency); });
	eatee->die();
}

void LifeForm::age(void) {
	if(!is_alive) return; //???
	this->gain_energy(-age_penalty);
	if(!is_alive) return;
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	(void) new Event(age_frequency, [self](void) { self->age(); });
}

void LifeForm::gain_energy(double delta_energy) {
	energy += delta_energy;
	if(energy < min_energy) this->die();
}

void LifeForm::check_encounter(void) {
	if(!is_alive) return; //if not alive
	assert(is_alive);
	SmartPointer<LifeForm> nearest;
	nearest = space.closest(pos);
	//will the QuadTree return a nearest object that is not alive???
//	if(nearest) { //do I need this??? in QuadTree already assert that nearest should be found
	if(!nearest->is_alive) {
		std::cout<<"Oops! Nearest object is not alive!"<<std::endl;
		exit(0);
	}
	assert(nearest->is_alive);
	//I will not update nearest's posititon here
	if(nearest->position().distance(pos) <= encounter_distance) {
		resolve_encounter(nearest);
	}
//	}
//	else {std::cout<<"Nearest not found!"<<std::endl;}
}

//here both a and b are alive
void LifeForm::resolve_encounter(SmartPointer<LifeForm> nearest) {
	assert(is_alive&&nearest->is_alive);

	//apply encounter_penalty
	gain_energy(-encounter_penalty);
	nearest->gain_energy(-encounter_penalty);

	//????
	if(!(is_alive&&nearest->is_alive)) {
		std::cout<<"One of them die!"<<std::endl;
		return;
	}
	assert(is_alive&&nearest->is_alive);
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	//could I access the energy of the nearest, not only the health???
	Action act1=encounter(info_about_them(nearest));
	Action act2=nearest->encounter(nearest->info_about_them(self));
	bool eat_success1 = drand48() < eat_success_chance(energy,nearest->energy);
	bool eat_success2 = drand48() < eat_success_chance(nearest->energy,energy);
	if(act1==LIFEFORM_EAT&&act2==LIFEFORM_IGNORE)
	{
		if(eat_success1)
			eat(nearest);
	}
	else if(act1==LIFEFORM_IGNORE&&act2==LIFEFORM_EAT)
	{
		if(eat_success2)
			nearest->eat(self);
	}
	else if(act1==LIFEFORM_EAT&&act2==LIFEFORM_EAT)
	{
		if(eat_success1&&!eat_success2)
			eat(nearest);
		else if(!eat_success1&&eat_success2)
			nearest->eat(self);
		else if(eat_success1&&eat_success2) {
			switch(encounter_strategy) {
				case EVEN_MONEY:
					if(drand48()<0.5) eat(nearest);
					else nearest->eat(self);
					break;
				case BIG_GUY_WINS:
					if(energy>=nearest->energy) eat(nearest);
					else nearest->eat(self);
					break;
				case UNDERDOG_IS_HERE:
					if(energy>=nearest->energy) nearest->eat(self);
					else eat(nearest);
					break;
				case FASTER_GUY_WINS:
					if(speed>=nearest->speed) eat(nearest);
					else nearest->eat(self);
					break;
				case SLOWER_GUY_WINS:
					if(speed>=nearest->speed) nearest->eat(self);
					else eat(nearest);
					break;
				default:
					std::cout<<"Unknown encounter_strategy!"<<endl;
			}
		}
	}

	//why can I use the protected function for other LifeForm????
	//can I use private methods of other LifeForm???
}


void LifeForm::set_course(double new_course) {
	if(!is_alive) return; //???
	update_position();
	if(!is_alive) return;
	course = new_course;
	compute_next_move();
}

void LifeForm::set_speed(double new_speed) {
	if(!is_alive) return; //???
	update_position();
	if(!is_alive) return;
	if(new_speed > max_speed) speed = max_speed;
	else speed = new_speed;
	compute_next_move();
}

void LifeForm::reproduce(SmartPointer<LifeForm> child) {
    SmartPointer<LifeForm> nearest;
	if(is_alive && (Event::now()-reproduce_time)>=min_reproduce_time) {
		double dist, theta;
		bool flag=0;
		for(int i=0; i<5; i++) {
			dist = drand48()*(reproduce_dist-encounter_distance)+encounter_distance;
			theta = drand48()*2*M_PI;
			child->pos.xpos = pos.xpos + dist*cos(theta);
			child->pos.ypos = pos.ypos + dist*sin(theta);
			nearest = space.closest(child->pos);
			//if(nearest==nullptr) { flag=1; break; } //maybe I don't need this, because
			if(!space.is_out_of_bounds(child->pos) &&
					nearest->position().distance(child->position())>encounter_distance)
			{ flag=1; break; }
		}
		if(flag) {
			child->start_point = child->pos;
			space.insert(child, child->pos, [child](void) { child->region_resize(); });
			(void) new Event(age_frequency, [child](void){ child->age(); });
			child->is_alive = true;
			gain_energy(-energy/2);
			gain_energy(-energy*reproduce_cost);
			child->gain_energy(energy);
			reproduce_time = Event::now();
		}
//		else { delete child; }
	}
	//do I need to do delete?????????????
//	else { delete child; }
}

ObjList LifeForm::perceive(double range) {
	update_position(); //first update object's position
	if(range > max_perceive_range) range = max_perceive_range;
	if(range < min_perceive_range) range = min_perceive_range;
	gain_energy(-perceive_cost(range));

	ObjList obj_list;
	if(is_alive){
		std::vector<SmartPointer<LifeForm> > lf_list = space.nearby(pos, range);
		for(auto it=lf_list.begin(); it!=lf_list.end(); ++it)
		{
			if((*it)->is_alive) //only alive objects can be perceived
				obj_list.push_back(info_about_them(*it));
		}
	}
	return obj_list;
}

//void LifeForm::runTests(void);
//bool LifeForm::testMode;
//void LifeForm::place(std::shared_ptr<LifeForm>, Point p);




