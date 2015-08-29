//
//  zz3793.cpp
//  project3a
//
//  Created by zhiyuan zou on 5/2/15.
//  Copyright (c) 2015 zhiyuan zou. All rights reserved.
//

#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include "zz3793.h"
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

Initializer<zz3793> __zz3793_initializer;

std::string zz3793::player_name(void) const{
    return "zz3793";
}

String zz3793::species_name(void) const
{
    return myname;
}

Action zz3793::encounter(const ObjInfo& info)
{
    if (info.species == "zz3793") {
        /* don't be cannibalistic */
        set_course(info.bearing + M_PI);
        return LIFEFORM_IGNORE;
    }
    else {
        hunt_event->cancel();
        if(info.their_speed==0) last_eat=Event::now();
        if(health()>3.0) spawn();
        SmartPointer<zz3793> self = SmartPointer<zz3793>(this);
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }
}

void zz3793::initialize(void) {
    LifeForm::add_creator(zz3793::create, "zz3793");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
zz3793::zz3793() {
    this->myname="zz3793";
    SmartPointer<zz3793> self = SmartPointer<zz3793>(this);
    new Event(0, [self](void) { self->startup(); });
}

zz3793::zz3793(std::string name){
    this->myname=name;
    SmartPointer<zz3793> self = SmartPointer<zz3793>(this);
    new Event(0, [self](void) { self->startup(); });

}

zz3793::~zz3793() {}

void zz3793::startup() {
    last_eat=Event::now();
    ObjList prey = perceive(2);
    double best_d = HUGE;
    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        if (best_d > (*i).distance) {
            best_d = (*i).distance;
            if((*i).species!="Algae") myname=(*i).species;
        }
    }

    set_course(drand48() * 2.0 * M_PI);
    set_speed(2 + 5.0 * drand48());
    SmartPointer<zz3793> self = SmartPointer<zz3793>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
}

void zz3793::spawn(void) {
    SmartPointer<zz3793> child = new zz3793;
    reproduce(child);
}


Color zz3793::my_color(void) const {
    return ORANGE;
}

SmartPointer<LifeForm> zz3793::create(void) {
    return new zz3793;
}

void zz3793::ambush(void){
    cout<<"zz3793 ambush"<<endl;
    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died
    set_speed(0);
    myname="Algae";
    SmartPointer<zz3793> self = SmartPointer<zz3793>(this);
    hunt_event = new Event(10, [self](void) { self->spawn();self->hunt(); });

}
void zz3793::hunt(void) {
    cout<<"zz3793 hunt"<<endl;
    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died
    double mp=sqrt(health());
    ObjList prey;
    if(Event::now()-last_eat<5.0)
        prey = perceive(20*mp);
    else prey = perceive(40);
    bool found_prey=false;

    double best_d = HUGE;
    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        if ((*i).their_speed==0&&eat_success_chance(health(), (*i).health)>0.5) {
            if (best_d > (*i).distance) {
                set_course((*i).bearing);
                if(Event::now()-last_eat<5.0)
                    set_speed((3+6.0 *mp* drand48()));
                else set_speed(3+6.0*mp*drand48());
                best_d = (*i).distance;
                found_prey=true;
                if((*i).species!="Algae") myname=(*i).species;
            }
        }
    }
    if(found_prey==false){
        for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
            if (eat_success_chance(health(), (*i).health)>0.5) {
                if (best_d > (*i).distance) {
                    set_course((*i).bearing);
                    if(Event::now()-last_eat<5.0)
                        set_speed((3+6.0 *mp* drand48()));
                    else set_speed(3+6.0*mp*drand48());
                    best_d = (*i).distance;
                    found_prey=true;
                    if((*i).species!="Algae") myname=(*i).species;
                }
            }
        }

    }
    if(found_prey==false){
        for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
            if (best_d > (*i).distance) {
                set_course((*i).bearing);
                if(Event::now()-last_eat<5.0)
                    set_speed((3+6.0 *mp* drand48()));
                else set_speed(3+6.0*mp*drand48());
                best_d = (*i).distance;
                found_prey=true;
                if((*i).species!="Algae") myname=(*i).species;
            }
        }

    }
    if(found_prey==false){
        double temp=get_course()+M_PI;
        set_speed(3+6.0 *mp* drand48());
        if(temp>2*M_PI) temp-=2*M_PI;
        set_course(temp);
    }
    SmartPointer<zz3793> self = SmartPointer<zz3793>(this);
    double nexttime=50/get_speed();
    hunt_event = new Event(nexttime, [self](void) { self->hunt(); });
    if(encounter_strategy==UNDERDOG_IS_HERE){
        if(health()>=2)
            spawn();
    }else{
        if(health()>=3.0)
            ambush();
    }
}
