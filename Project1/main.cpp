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

bool intersect(const Ray &r, vector<Object*> *list, hit_record &rec, int &index)
{
	bool hit = false;
	float tempmax = numeric_limits<float>::max();
	for (int i = 0; i < list->size(); i++)
	{
		if (list->at(i)->hit(r, 1e-3, tempmax, rec))
		{
			hit = true;
			tempmax = rec.t;
			index = i;
		}
	}

	return hit;
}

bool visible(vector<Object*> *list, Object* light, const Vec &startpoint, const Vec &hitpoint)
{
	Vec t = hitpoint - startpoint;
	float tempmax = t.length();
	hit_record rec;
	Ray r(startpoint, hitpoint - startpoint);
	for (int i = 0; i < list->size(); i++)
	{
		if (list->at(i)->hit(r, 1e-3, tempmax, rec))
			//return false;
		{
			if (list->at(i) != light)
				return false;
		}
	}

	return true;
}

Vec lightsampling(const hit_record &rec, vector<Object*> *list, const vector<Object*> &lights)
{
	Vec lightSampling(0, 0, 0);
	if (rec.mat->isDiffuse)
	{
		for (Object* temp : lights)
		{
			Vec lightPos = temp->random_on_surface();
			Vec lightDir = (lightPos - rec.p).unit_vec();
			Ray toLight(rec.p, lightDir);

			if (visible(list, temp, rec.p, lightPos))
			{
				float wi = lightDir.dot(rec.normal);
				if (wi > 0)
				{
					float rad = (temp->box.bounds[1] - temp->box.bounds[0]) / 2;
					float cos_a_max = sqrt(1 - rad * rad / (rec.p - lightPos).dot(rec.p - lightPos));
					float omega = 2/* * M_PI*/ * (1 - cos_a_max);
					lightSampling += temp->mat->emittance * wi * omega/* * M_1_PI*/;
				}
			}
		}
	}

	return lightSampling;
}

Vec lightsampling(const hit_record &rec, vector<Object*> *list, const vector<int> &index, const vector<hit_record> &lightpaths, const vector<Vec> &colors)
{
	Vec lightSampling(0, 0, 0);
	if (rec.mat->isDiffuse)
	{
		hit_record temp;
		for (int i = 0; i < index.size(); i++)
		{
			temp = lightpaths.at(i);
			Vec lightDir = (rec.p - temp.p).unit_vec();
			int enlightcount = 0;
			if (visible(list, list->at(index.at(i)), rec.p, temp.p))
			{
				float enlight = max(0.0f, (lightDir).dot(temp.normal)) * M_1_PI;
				float eye = max(0.0f, (lightDir * -1).dot(rec.normal)) * M_1_PI;
				float dist_sqr = (rec.p - temp.p).dot(rec.p - temp.p);
				lightSampling += colors.at(i) * enlight * eye / dist_sqr;
			}
		}
	}

	return lightSampling;
}

Vec radiance(const Ray &r, Octree *tree, int depth)
{
	if (depth > 5)
		return Vec();

	// CREATE THE LIST OF OBJECTS
	vector<Object*> *list;
	if (tree->triangles.size() < 64) // Simply use all the objects
		list = &tree->triangles;
	else // This uses an Octree to narrow down the list
	{
		list = new vector<Object*>;
		tree->hit(r, list);
		if (list->size() == 0) return Vec();
	}
	
	// GET THE LIGHTS
	vector<Object*> lights;
	for (Object* temp : tree->triangles)
		if (temp->mat->emittance.maximum() > 0) lights.push_back(temp);

	// TRACE THE LIGHTS
	hit_record rec3;
	vector<hit_record> lightpaths;
	vector<Vec> colors;
	vector<int> index;
	for (Object* temp : lights)
	{
		Vec start = temp->random_on_surface();
		Vec target = start + hemisphere(start - temp->box.center);
		Ray ray(start, target - start);

		Vec cf(1, 1, 1);
		Vec cl(0, 0, 0);
		int ind;

		for (int j = 0; j < 5; j++)
		{
			if (!intersect(ray, list, rec3, ind)) break;

			Vec att;
			Ray scatt;
			rec3.mat->scatter(ray, rec3, att, scatt);
			cf *= att;
			cl = cf * temp->mat->emittance;

			if (rec3.mat->isDiffuse)
			{
				lightpaths.push_back(rec3);
				colors.push_back(cl);
				index.push_back(ind);
			}
			ray = scatt;
		}
	}

	// TEST THE LIST FOR THE FIRST OBJECT HIT
	hit_record rec;
	int ind;
	if (!intersect(r, list, rec, ind)) return Vec(0.25, 0.25, 0.25);

	Ray scatt;
	Vec att;
	if (!(rec.mat)->scatter(r, rec, att, scatt)) return Vec();

	Vec light = lightsampling(rec, list, index, lightpaths, colors);

	if (rec.mat->isDiffuse)
	{
		att *= (2 * ((scatt.direction()).unit_vec()).dot(rec.normal));
		light += lightsampling(rec, list, lights);
	}

	// BRDF
	if (depth == 0)
		return rec.mat->emittance + att * light + att * radiance(scatt, tree, depth + 1);
	return att * light + att * radiance(scatt, tree, depth + 1);
}

int main()
{
	srand(time(NULL));

	int start_time = clock();

	int nx = 640;
	int ny = 360;
	int ns = 4;

	vector<Object*> objList;

	/*vector<Mesh*> meshes;
	meshes.push_back(new Mesh("obj/sphere1.obj", Vec(0, 2.3, 0), new Lambertian(Vec(1, 0.75, 0.5), Vec())));

	for (int i = 0; i < meshes.size(); i++)
	{
	for (int j = 0; j < meshes.at(i)->triangles.size(); j++)
	objList.push_back(meshes.at(i)->triangles.at(j));
	}*/

	objList.push_back(new Sphere(Vec(), 1, new Lambertian(Vec(1, 1, 1))));
	objList.push_back(new Sphere(Vec(2.5, 0, 0), 1, new Lambertian(Vec(0.75, 0, 0))));
	objList.push_back(new Sphere(Vec(-2.5, 0, 0), 1, new Lambertian(Vec(0, 0, 0.75))));
	objList.push_back(new Sphere(Vec(0, 0, 2.5), 1, new Lambertian(Vec(0, 0.75, 0))));
	objList.push_back(new Sphere(Vec(0, 0, -2.5), 1, new Lambertian(Vec(0.75, 0.75, 0))));

	objList.push_back(new Sphere(Vec(0, -1e3 - 1, 0), 1e3, new Lambertian(Vec(0.8, 0.6, 0.2))));

	objList.push_back(new Sphere(Vec(4, 8, 0), 2, new Lambertian(Vec(1, 1, 1), Vec(8, 8, 8))));

	//BVH *root = new BVH(objList, 0);
	Octree *root = new Octree(objList, 0);

	Vec fr(4, 3.5, 8);
	Vec to(0, 0, 0);
	//Vec to = root->box.getCenter();
	Vec up(0, 1, 0);
	float dist_to = (fr - to).length();
	float aperture = 0.7;
	Camera cam(fr, to, up, 45, float(nx) / float(ny), aperture, dist_to);
	Vec *c = new Vec[nx * ny];

#pragma omp parallel for schedule(dynamic) num_threads(8)

	for (int j = ny - 1; j >= 0; j--)
	{
		fprintf(stderr, "\rRendering : %d spp : %5.2f%%", ns, 100.*(ny - j - 1) / (ny - 1));
		for (int i = 0; i < nx; i++)
		{
			Vec color(0, 0, 0);
			for (int k = 0; k < ns; k++)
			{
				float r1 = 2 * (rand() / (RAND_MAX + 1.0)); // TENT FILTER
				float dx;
				if (r1 < 1) dx = sqrt(r1) - 1;
				else dx = 1 - sqrt(2 - r1);
				float r2 = 2 * (rand() / (RAND_MAX + 1.0));
				float dy;
				if (r2 < 1) dy = sqrt(r2) - 1;
				else dy = 1 - sqrt(2 - r2);

				float u = float(i + 0.5 + 0.5 * dx) / float(nx);
				float v = float(j + 0.5 + 0.5 * dy) / float(ny);

				//float u = float(i + (rand() / (RAND_MAX + 1.0))) / float(nx);
				//float v = float(j + (rand() / (RAND_MAX + 1.0))) / float(ny);
				Ray r = cam.get_ray(u, v);
				color += radiance(r, root, 0);

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

// PREVIOUS RADIANCE FUNCTIONS

// NON-RECURSION
/*if (rec.mat->gete().e[0] > 0)
return weight * rec.mat->gete();
else
return weight * att * radiance(scatt, tree, depth + 1, weight);*/

// NON-CONDENSED
/*float cos_theta = ((scatt.direction()).unit_vec()).dot(rec.normal);
Vec BRDF = att / M_PI;
Vec inc = radiance(scatt, tree, depth + 1);
return rec.mat->gete() + (BRDF*cos_theta*radiance(scatt, tree, depth + 1) * (2 * M_PI));*/