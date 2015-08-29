#ifndef yh7483_h
#define yh7483_h

#include <memory>
#include "LifeForm.h"
#include "Init.h"

class yh7483 : public LifeForm {
protected:
	static void initialize(void);
	void spawn(void);
	void hunt(void);
	void startup(void);
	Event* hunt_event;
	void escape();
	double last_eat;
public:
	yh7483(void);
	~yh7483(void);
	Color my_color(void) const;   // defines LifeForm::my_color
	static SmartPointer<LifeForm> create(void);
	virtual std::string species_name(void) const;
	virtual std::string player_name(void) const;
	virtual Action encounter(const ObjInfo&);
	friend class Initializer<yh7483>;
	std::string name = "hanyc";
};


#endif
