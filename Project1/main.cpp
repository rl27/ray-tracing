#include <fstream>
#include <iostream>
#include <ctime>
#include <limits>
#include "Camera.h"
#include "Material.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Mesh.h"
#include "Octree.h"
#include "BVH.h"
using namespace std;

Vec getcolor(const Ray &r, Octree *tree, int depth)
{
	if (depth > 5) return Vec();

	vector<Object*> *list;
	if (tree->triangles.size() < 100)
		list = &tree->triangles;
	else
	{
		list = new vector<Object*>;
		tree->hit(r, list);
		if (list->size() == 0) return Vec();
	}

	hit_record rec;
	bool hit = false;
	float tempmax = numeric_limits<float>::max();
	for (int i = 0; i < list->size(); i++)
	{
		if (list->at(i)->hit(r, 1e-3, tempmax, rec))
		{
			hit = true;
			tempmax = rec.t;
		}
	}

	if (!hit) return Vec();

	Ray scatt;
	Vec att;
	if ((rec.mat)->scatter(r, rec, att, scatt))
	{
		float cos_theta = ((scatt.direction()).unit_vec()).dot(rec.normal);
		Vec BRDF = att / M_PI;
		//return rec.mat->gete() + att * (getcolor(scatt, tree, light, depth + 1));
		Vec inc = getcolor(scatt, tree, depth + 1);
		return rec.mat->gete() + (BRDF*cos_theta*inc * (2 * M_PI));
	}
	else
		return Vec();
}

int main()
{
	srand(time(NULL));

	int start_time = clock();

	int nx = 800;
	int ny = 600;
	int ns = 1000;


	vector<Mesh*> meshes;
	//meshes.push_back(new Mesh("obj/sphere1.obj", Vec(0, 2.5, 0), new Lambertian(Vec(1, 0.75, 0.5), Vec())));

	vector<Object*> objList;
	for (int i = 0; i < meshes.size(); i++)
	{
		for (int j = 0; j < meshes.at(i)->triangles.size(); j++)
			objList.push_back(meshes.at(i)->triangles.at(j));
	}

	objList.push_back(new Sphere(Vec(), 1, new Lambertian(Vec(0.7, 0.7, 0.7))));
	objList.push_back(new Sphere(Vec(2.5, 0, 0), 1, new Lambertian(Vec(0.7, 0, 0))));
	objList.push_back(new Sphere(Vec(-2.5, 0, 0), 1, new Lambertian(Vec(0, 0, 0.7))));
	objList.push_back(new Sphere(Vec(0, 0, 2.5), 1, new Lambertian(Vec(0, 0.7, 0))));
	objList.push_back(new Sphere(Vec(0, 0, -2.5), 1, new Lambertian(Vec(0.7, 0.7, 0))));

	objList.push_back(new Sphere(Vec(0, -1e3 - 1, 0), 1e3, new Lambertian(Vec(0.8, 0.6, 0.2))));

	objList.push_back(new Sphere(Vec(4, 8, 0), 3, new Lambertian(Vec(0.8, 0.6, 0.2), Vec(5, 5, 5))));


	//BVH *root = new BVH(objList, 0);
	Octree *root = new Octree(objList, 0);

	Vec fr(3, 5, 8);
	Vec to(0, 0, 0);
	//Vec to = root->box.getCenter();
	Vec up(0, 1, 0);
	float dist_to = (fr - to).length();
	float aperture = 0;
	Camera cam(fr, to, up, 45, float(nx) / float(ny), aperture, dist_to);
	Vec *c = new Vec[nx * ny];

#pragma omp parallel for schedule(dynamic) num_threads(4)

    for (int j = ny-1; j >= 0; j--)
    {
		fprintf(stderr, "\rRendering (%d spp) %5.2f%%", ns, 100.*(ny - j - 1) / (ny - 1));
        for (int i = 0; i < nx; i++)
        {
			Vec color(0, 0, 0);
			for (int k = 0; k < ns; k++)
			{
				float u = float(i + (rand() / (RAND_MAX + 1.0))) / float(nx);
				float v = float(j + (rand() / (RAND_MAX + 1.0))) / float(ny);
				Ray r = cam.get_ray(u, v);
				color += getcolor(r, root, 0);
			
			}
			color /= float(ns);
			color = Vec(pow(clamp(color.e[0]), 1.0 / 2.2), pow(clamp(color.e[1]), 1.0 / 2.2), pow(clamp(color.e[2]), 1.0 / 2.2));

			c[(ny - j - 1)*nx + i] = Vec(int(255.99*color.e[0]), int(255.99*color.e[1]), int(255.99*color.e[2]));
	    }
    }

	ofstream myfile;
	myfile.open("output.ppm");
	myfile << "P3\n" << nx << " " << ny << "\n255\n";

	for (int i = 0; i < nx*ny; i++)
		myfile << c[i].e[0] << " " << c[i].e[1] << " " << c[i].e[2] << "\n";

    myfile.close();

	int stop_time = clock();
	
	cout << "\n>> Running time : " << (stop_time - start_time) / 1000.0 << endl;
	cout << ">> Objects : " << objList.size() << endl;

	cin.get();

    return 0;
}