//
//  zz3793.h
//  project3a
//
//  Created by zhiyuan zou on 5/2/15.
//  Copyright (c) 2015 zhiyuan zou. All rights reserved.
//

#ifndef project3a_zz3793_h
#define project3a_zz3793_h

#include <memory>
#include "LifeForm.h"
#include "Init.h"

class zz3793 : public LifeForm {
protected:
    double last_eat;
    static void initialize(void);
    void spawn(void);
    void hunt(void);
    void ambush(void);
    void startup();
    Event* hunt_event=nullptr;
public:
    zz3793(void);
    zz3793(std::string);
    ~zz3793(void);
    Color my_color(void) const;   // defines LifeForm::my_color
    static SmartPointer<LifeForm> create(void);
    virtual std::string species_name(void) const;
    virtual std::string player_name(void) const;
    virtual Action encounter(const ObjInfo&);
    friend class Initializer<zz3793>;
    std::string myname;
    
};


#endif
