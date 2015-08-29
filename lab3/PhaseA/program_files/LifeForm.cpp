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

#include "Random.h"
#if defined (_MSC_VER)
using namespace epl;
#endif
//const double time_tolerance = 0.001;		//using min_delta_time in Param.h

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

void cout_pos(Point point){
	std::cout<<"pos: "<<point.xpos<<" "<<point.ypos<<std::endl;
}
/*************************************************************************************************************/
void LifeForm::update_position(void) {
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}
	Point pos_new;
	//Point pos_old = pos;
	SimTime delta_t = Event::now() - update_time;

	if (delta_t >= min_delta_time) {
		double energy_consumed = movement_cost(speed, delta_t);
		energy -= energy_consumed;

		if (energy < min_energy) {
			die();
			return;
		}

		double delta_x = speed * cos(course) * delta_t;
		double delta_y = speed * sin(course) * delta_t;
		pos_new.xpos = pos.xpos + delta_x;
		pos_new.ypos = pos.ypos + delta_y;

		if (space.is_out_of_bounds(pos_new)) {
			die();
			return;
		}
		update_time = Event::now();
		space.update_position(pos, pos_new);
		pos.xpos = pos_new.xpos;
		pos.ypos = pos_new.ypos;
	}
}

void LifeForm::border_cross(void) {
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}

	update_position();
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}
	check_encounter();////////////////////////
	compute_next_move();
}

void LifeForm::region_resize(void) {
	//if (!is_alive) return;/////////////////////////because of reproduce
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}

	border_cross_event->cancel();
	update_position();
	//if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}
	compute_next_move();

}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> that) {
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	that->update_position();
	if (!self->is_alive || !that->is_alive) return;

	self->energy -= encounter_penalty;
	if (self->energy < min_energy) {
		self->die();
		return;
	}
	that->energy -= encounter_penalty;
	if (that->energy < min_energy) {
		that->die();
		return;
	}
	if (!self->is_alive || !that->is_alive) return;


	auto x_act = self->encounter(self->info_about_them(that));
	auto y_act = that->encounter(that->info_about_them(self));

	bool x_eat_y = false;
	bool y_eat_x = false;
	double roll = drand48();
	if ((roll < eat_success_chance(self->energy, that->energy)) && x_act == LIFEFORM_EAT) x_eat_y = true;
	roll = drand48();
	if ((roll < eat_success_chance(that->energy, self->energy)) && y_act == LIFEFORM_EAT) y_eat_x = true;

	if (x_eat_y && y_eat_x) {//////////////////////////solve both eat
		if (encounter_strategy == EVEN_MONEY) {
			if (drand48() < 0.5) x_eat_y = false;
			else y_eat_x = false;
		}
		else if (encounter_strategy == BIG_GUY_WINS) {
			if (self->energy > that->energy) y_eat_x = false;
			else x_eat_y = false;
		}
		else if (encounter_strategy == UNDERDOG_IS_HERE) {
			if (self->energy > that->energy) x_eat_y = false;
			else  y_eat_x = false;
		}
		else if (encounter_strategy == FASTER_GUY_WINS) {
			if (self->speed > that->speed) y_eat_x = false;
			else x_eat_y = false;
		}
		else if (encounter_strategy == SLOWER_GUY_WINS) {
			if (self->speed > that->speed) x_eat_y = false;
			else y_eat_x = false;
		}
	}

	if (x_eat_y) {
		self->eat(that);
		that->die();
	}
	else if (y_eat_x) {
		that->eat(self);
		self->die();
	}
	else {
		;
	}
}

void LifeForm::eat(SmartPointer<LifeForm> that) {
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	double award = that->energy * eat_efficiency - eat_cost_function();
	new Event(digestion_time, [self, award](void) { self->gain_energy(award); });

}

void LifeForm::age(void) {
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);

	if (!is_alive) return;
	energy -= age_penalty;
	if (energy < min_energy) {
		die();
		return;
	}
	new Event(age_frequency, [self](void) { self->age(); });
}

void LifeForm::gain_energy(double that) {
	if (!is_alive) return;
	update_position();
	if (!is_alive) return;
	compute_next_move();

	energy += that;
}

void LifeForm::check_encounter(void) {
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}

	auto near = space.nearby(pos, encounter_distance);
	if (near.size() == 0) return;

	resolve_encounter(space.closest(pos));
}

void LifeForm::compute_next_move(void) {
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}

	double dist_to_edge = space.distance_to_edge(pos, course);
	if (speed) {
		double delta_t = dist_to_edge / speed;
		SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
		border_cross_event = new Event(delta_t + Point::tolerance, [self](void) { self->border_cross(); });
	}
}

void LifeForm::set_course(double that) {
	course = that;
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}

	border_cross_event->cancel();
	update_position();
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}
	compute_next_move();
}

void LifeForm::set_speed(double that) {
	if (that > max_speed)
		speed = max_speed;
	else
		speed = that;
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}

	border_cross_event->cancel();
	update_position();
	if (!is_alive) return;
	if (space.is_out_of_bounds(pos)) {
		die();
		return;
	}
	compute_next_move();
}

void LifeForm::reproduce(SmartPointer<LifeForm> that) {
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	if (!self->is_alive) return;

	if (Event::now() - self->reproduce_time < min_reproduce_time) {//////////forbid reproduce within min_reproduce_time
		that->die();
		return;
	}

	double old_energy = self->energy;////////////////////////energy
	self->energy = old_energy / 2 * (1 - reproduce_cost);
	that->energy = old_energy / 2 * (1 - reproduce_cost);

	SmartPointer<LifeForm> nearest;
	const int max_loop = 5;
	int loop = 0;
	/******************************finding pos************************************/
	do {
		++loop;
		that->pos.ypos = drand48() * reproduce_dist + self->pos.ypos;
		that->pos.xpos = drand48() * reproduce_dist + self->pos.xpos;
		nearest = space.closest(that->pos);
	} while (that->position().distance(self->position()) >= reproduce_dist
		&& nearest->position().distance(that->position()) <= encounter_distance
		&& loop <= max_loop);
	if (loop == max_loop) {
		that->pos.ypos = std::min<double>(drand48() * reproduce_dist + self->pos.ypos, grid_max);
		that->pos.xpos = std::min<double>(drand48() * reproduce_dist + self->pos.xpos, grid_max);
	}
	/******************************************************************************/
	that->start_point = that->pos;
	space.insert(that, that->pos, [that]() { that->region_resize(); });
	(void) new Event(age_frequency, [that](void) { that->age(); });
	that->is_alive = true;

	self->reproduce_time = Event::now();
}

ObjList LifeForm::perceive(double that) {
	ObjList result;
	result.clear();

	if (!is_alive) return result;

	double rad = that;
	if (rad < min_perceive_range) rad = min_perceive_range;
	if (rad > max_perceive_range) rad = max_perceive_range;

	double energy_cost = perceive_cost(rad);////////////////energy cost
	energy -= energy_cost;
	if (energy < min_energy) {
		die();
		return result;
	}

	auto temp_result = space.nearby(pos, rad);
	for (auto i = temp_result.begin(); i != temp_result.end(); ++i) {
		result.push_back(info_about_them(*i));
	}
	return result;
}
