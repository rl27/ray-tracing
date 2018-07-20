#include <fstream>
#include <iostream>
#include <ctime>
#include <limits>
#include "ObjectList.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Octree.h"
using namespace std;

Vec getcolor(const Ray &r, Octree *tree, int depth, Box bound)
{
	vector<Mesh*> list;
	tree->hit(r, list);
	if (list.size() == 0)
	{
		Vec dir = (r.direction()).unit_vec();
		float t = 0.5*(dir.e[1] + 1.0);
		return Vec(1.0, 1.0, 1.0)*(1.0 - t) + Vec(0.5, 0.7, 1.0)*t;
	}

	hit_record rec;
	bool hit = false;
	float tempmax = numeric_limits<float>::max();
	for (int i = 0; i < list.size(); i++)
	{
		if (list.at(i)->hit(r, 1e-3, tempmax, rec))
		{
			hit = true;
			tempmax = rec.t;
		}
	}

	if (hit)
	{
		Ray scatt;
		Vec att;
		if (depth < 10 && (rec.mat)->scatter(r, rec, att, scatt))
			return att * getcolor(scatt, tree, depth + 1, bound);
		else
			return Vec(0, 0, 0);
	}
	else
	{
		Vec dir = (r.direction()).unit_vec();
		float t = 0.5*(dir.e[1] + 1.0);
		return Vec(1.0, 1.0, 1.0)*(1.0 - t) + Vec(0.5, 0.7, 1.0)*t;
	}
}


Vec getcolor(const Ray &r, const vector<Mesh*> &meshes/*Object *list*/, int depth, Box bound)
{
	if (!bound.hitBox(r))
	{
		Vec dir = (r.direction()).unit_vec();
		float t = 0.5*(dir.e[1] + 1.0);
		return Vec(1.0, 1.0, 1.0)*(1.0 - t) + Vec(0.5, 0.7, 1.0)*t;
	}

	hit_record rec;
	bool hit = false;
	float tempmax = numeric_limits<float>::max();
	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes.at(i)->hit(r, 1e-3, tempmax, rec))
		{
			hit = true;
			tempmax = rec.t;
		}
	}

	if (hit)
	{
		Ray scatt;
		Vec att;
		if (depth < 10 && (rec.mat)->scatter(r, rec, att, scatt))
			return att * getcolor(scatt, meshes, depth + 1, bound);
		else
			return Vec(0, 0, 0);
	}
	else
	{
		Vec dir = (r.direction()).unit_vec();
		float t = 0.5*(dir.e[1] + 1.0);
		return Vec(1.0, 1.0, 1.0)*(1.0 - t) + Vec(0.5, 0.7, 1.0)*t;
	}
}

int main()
{
	srand(time(NULL));

	ofstream myfile;
	myfile.open("output.ppm");

	int start_time = clock();

	int nx = 400;
	int ny = 200;
	int ns = 2;
	myfile << "P3\n" << nx << " " << ny << "\n255\n";

	vector<Mesh*> meshes;
	meshes.push_back(new Mesh("obj/dodecahedron.obj", Vec()));
	meshes.push_back(new Mesh("obj/cube2.obj", Vec(-1,0,0)));


	Box totalbound;
	for (int i = 0; i < meshes.size(); i++)
	{
		totalbound.expand(meshes.at(i)->box.bounds);
		cout << meshes.at(i)->box.bounds[0] << "  " << meshes.at(i)->box.bounds[1] << "  " << meshes.at(i)->box.bounds[2] << "  " << meshes.at(i)->box.bounds[3] << "  " << meshes.at(i)->box.bounds[4] << "  " << meshes.at(i)->box.bounds[5] << endl;
	}
	cout << totalbound.bounds[0] << "  " << totalbound.bounds[1] << "  " << totalbound.bounds[2] << "  " << totalbound.bounds[3] << "  " << totalbound.bounds[4] << "  " << totalbound.bounds[5] << endl;

	//Octree *root = new Octree(totalbound.getCenter(), totalbound.getMaxLength(), 0, meshes);
//	root->meshes = meshes;


	/*
	Object *list[2];
	Mesh* m1 = new Mesh("obj/dodecahedron.obj");
	list[0] = m1;
	list[1] = new Sphere(Vec(0, -101, -1), 100, new Lambertian(Vec(1, 0.5, 0.3)));
	Object *mylist = new ObjectList(list, 2);
	*/

	/*
	Object *list[5];
	//list[0] = new Plane(Vec(0, -0.5, 0), Vec(0, 1, 0), new Lambertian(Vec(0.8, 0.8, 0.8)));
	list[0] = new Sphere(Vec(0, -100.5, -1), 100, new Lambertian(Vec(1, 0.5, 0.3)));
	list[1] = new Sphere(Vec(0, 0, -1), 0.5, new Lambertian(Vec(0.3, 0.5, 0.7)));
	list[2] = new Sphere(Vec(1.0, 0, -1), 0.5, new Metal(Vec(0.7, 0.5, 0.1), 0.3));
	list[3] = new Sphere(Vec(-1.0, 0, -1), 0.5, new Dielectric(1.8));
	list[4] = new Triangle(Vec(-1.5, 0.5, -1), Vec(1.5, 0.5, -1), Vec(0, 3.1, -1), new Barycentric(Vec(0.6, 0.4, 0.1), Vec(0.1, 0.5, 0.3), Vec(0.1, 0.3, 0.7)));
	Object *mylist = new ObjectList(list, 5);
	*/
	
	Vec fr(-0.2, -0.2, 5);
	Vec to(-0.5, -0.5, 0);
	Vec up(0, 1, 0);
	float dist_to = (fr - to).length();
	float aperture = 0;
	Camera cam(fr, to, up, 40, float(nx) / float(ny), aperture, dist_to);

    for (int j = ny-1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
			Vec color(0, 0, 0);
			for (int k = 0; k < ns; k++)
			{
				float u = float(i + (rand() / (RAND_MAX + 1.0))) / float(nx);
				float v = float(j + (rand() / (RAND_MAX + 1.0))) / float(ny);
				Ray r = cam.get_ray(u, v);
				color += getcolor(r, meshes, 0, totalbound);
			}
			color /= float(ns);
			
			color = Vec(pow(color.e[0], 1.0 / 2.2), pow(color.e[1], 1.0 / 2.2), pow(color.e[2], 1.0 / 2.2)); // standard 2.2 gamma

			myfile << int(255.99*color.e[0]) << " " << int(255.99*color.e[1]) << " " << int(255.99*color.e[2]) << "\n";
	    }
    }

    myfile.close();

	int stop_time = clock();
	cout << ">> Running time : " << (stop_time - start_time) / 1000.0 << endl;

	int triangles = 0;
	for (int i = 0; i < meshes.size(); i++)
		triangles += meshes.at(i)->triangles.size();
	cout << ">> Triangles : " << triangles << endl;

	cin.get();

    return 0;
}