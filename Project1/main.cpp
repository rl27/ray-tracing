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

	// CREATE THE LIST
	vector<Object*> *list;
	if (tree->triangles.size() < 64)
		list = &tree->triangles;
	else
	{
		list = new vector<Object*>;
		tree->hit(r, list);
		if (list->size() == 0) return Vec();
	}

	// TEST THE LIST
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

	// DIDN'T HIT?
	if (!hit) return Vec(0.4, 0.4, 0.4);

	// DID HIT!

	Ray scatt;
	Vec att;
	if (!(rec.mat)->scatter(r, rec, att, scatt)) return Vec();


	Vec lightSampling(0, 0, 0);
	for (Object* temp : tree->triangles)
	{
		if (temp->mat->emittance.e[0] == 0) continue;

		Vec lightPos = random_on_sphere(temp->box.center, 3);
		Vec lightDir = (lightPos - rec.p).unit_vec();
		Ray toLight(rec.p, lightDir);

		hit_record rec2;
		float tempmax2 = numeric_limits<float>::max();
		vector<Object*> *list2 = &tree->triangles;
		bool hitLight = true;
		for (int i = 0; i < list2->size(); i++)
		{
			if (list2->at(i)->hit(toLight, 1e-3, tempmax2, rec2))
			{
				if (list2->at(i) != temp)
					hitLight = false;
				break;
			}
		}
		if (hitLight)
		{
			float wi = lightDir.dot(rec.normal);
			if (wi > 0)
			{
				float cos_a_max = sqrt(1 - 3 * 3 / (rec.p - lightPos).dot(rec.p - lightPos));
				float omega = 2 * M_PI*(1 - cos_a_max);
				lightSampling += temp->mat->emittance * wi * omega * M_1_PI;
			}
		}
	}

	// NON-RECURSION
	/*if (rec.mat->gete().e[0] > 0)
		return weight * rec.mat->gete();
	else
		return weight * att * getcolor(scatt, tree, depth + 1, weight);*/

	// NON-CONDENSED
	/*float cos_theta = ((scatt.direction()).unit_vec()).dot(rec.normal);
	Vec BRDF = att / M_PI;
	Vec inc = getcolor(scatt, tree, depth + 1);
	return rec.mat->gete() + (BRDF*cos_theta*getcolor(scatt, tree, depth + 1) * (2 * M_PI));*/

	if (depth == 0)
		return rec.mat->emittance + att * lightSampling + att * getcolor(scatt, tree, depth + 1);
	return att * lightSampling + att * getcolor(scatt, tree, depth + 1);
}

int main()
{
	srand(time(NULL));

	int start_time = clock();

	int nx = 400;
	int ny = 300;
	int ns = 5;

	vector<Object*> objList;

	/*vector<Mesh*> meshes;
	meshes.push_back(new Mesh("obj/sphere1.obj", Vec(0, 2.3, 0), new Lambertian(Vec(1, 0.75, 0.5), Vec())));

	for (int i = 0; i < meshes.size(); i++)
	{
		for (int j = 0; j < meshes.at(i)->triangles.size(); j++)
			objList.push_back(meshes.at(i)->triangles.at(j));
	}*/

	objList.push_back(new Sphere(Vec(), 1, new Metal(Vec(1,1,1), 0)));
	objList.push_back(new Sphere(Vec(3, 0, 0), 1, new Lambertian(Vec(0.75, 0, 0))));
	objList.push_back(new Sphere(Vec(-3, 0, 0), 1, new Lambertian(Vec(0, 0, 0.75))));
	objList.push_back(new Sphere(Vec(0, 0, 3), 1, new Lambertian(Vec(0, 0.75, 0))));
	objList.push_back(new Sphere(Vec(0, 0, -3), 1, new Lambertian(Vec(0.75, 0.75, 0))));

	objList.push_back(new Sphere(Vec(0, -1e3 - 1, 0), 1e3, new Lambertian(Vec(0.8, 0.6, 0.2))));

	objList.push_back(new Sphere(Vec(4, 8, 0), 3, new Lambertian(Vec(1, 1, 1), Vec(5, 5, 5))));

	//BVH *root = new BVH(objList, 0);
	Octree *root = new Octree(objList, 0);

	Vec fr(3, 5, 8);
	//Vec fr(2, 1.8, 5);
	Vec to(0, 0, 0);
	//Vec to = root->box.getCenter();
	Vec up(0, 1, 0);
	float dist_to = (fr - to).length();
	float aperture = 0.5;
	Camera cam(fr, to, up, 45, float(nx) / float(ny), aperture, dist_to);
	Vec *c = new Vec[nx * ny];

#pragma omp parallel for schedule(dynamic) num_threads(4)

    for (int j = ny-1; j >= 0; j--)
    {
		fprintf(stderr, "\rRendering : %d spp : %5.2f%%", ns, 100.*(ny - j - 1) / (ny - 1));
        for (int i = 0; i < nx; i++)
        {
			Vec color(0, 0, 0);
			for (int k = 0; k < ns; k++)
			{
				/*float r1 = 2 * (rand() / (RAND_MAX + 1.0)); // TENT FILTER
				float dx;
				if (r1 < 1) dx = sqrt(r1) - 1;
				else dx = 1 - sqrt(2 - r1);
				float r2 = 2 * (rand() / (RAND_MAX + 1.0));
				float dy;
				if (r2 < 1) dy = sqrt(r2) - 1;
				else dy = 1 - sqrt(2 - r2);

				float u = float(i + 0.5 + 0.5 * dx) / float(nx);
				float v = float(j + 0.5 + 0.5 * dy) / float(ny);*/

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