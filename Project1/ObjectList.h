#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include "Object.h"

class ObjectList : public Object
{
public:
	ObjectList(Object **obj, int n) : list(obj), size(n) {}

	// Implementation below
	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec);

	Object **list;
	int size;
};

// Check every Object in the ObjectList for a hit. Find the closest object and use that as the hit.
bool ObjectList::hit(const Ray &r, float t_min, float t_max, hit_record &rec)
{
	hit_record temp;
	bool hit = false;
	double tempmax = t_max;
	for (int i = 0; i < size; i++)
	{
		// This checks every Object in the List, reducing the range for checking (t_min, tempmax) with each hit.
		// This process results in the closest object being recorded.
		if (list[i]->hit(r, t_min, tempmax, temp))
		{
			hit = true;
			tempmax = temp.t;
			rec = temp;
		}
	}
	return hit;
}

#endif