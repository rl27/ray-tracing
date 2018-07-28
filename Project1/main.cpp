#include <fstream>
#include <iostream>
#include <ctime>
#include <limits>
#include "Triangle.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Octree.h"
#include "BVH.h"
using namespace std;

Vec getcolor(const Ray &r, Octree *tree, int depth)
{
	if (depth > 5) return Vec();

	vector<Triangle*> list;
	tree->hit(r, list);
	if (list.size() == 0) return Vec();

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

	if (!hit) return Vec();

	Ray scatt;
	Vec att;
	if ((rec.mat)->scatter(r, rec, att, scatt))
	{
		float cos_theta = (scatt.direction()).dot(rec.normal);
		Vec BRDF = att / M_PI;
		//return rec.mat->gete() + att * (getcolor(scatt, tree, light, depth + 1));
		Vec inc = getcolor(scatt, tree, depth + 1);
		return rec.mat->gete() + (BRDF*inc*cos_theta * 2 * M_PI);
	}
	else
		return Vec();
}

int main()
{
	srand(time(NULL));

	int start_time = clock();

	int nx = 400;
	int ny = 400;
	int ns = 10;

	vector<Mesh*> meshes;
	meshes.push_back(new Mesh("obj/sphere1.obj", Vec(), new Lambertian(Vec(1,0.75,0.5), Vec())));

	vector<Triangle*> triList;
	for (int i = 0; i < meshes.size(); i++)
	{
		for (int j = 0; j < meshes.at(i)->triangles.size(); j++)
			triList.push_back(meshes.at(i)->triangles.at(j));
	}

	triList.push_back(new Triangle(Vec(-10, -1, -10), Vec(-10, -1, 10), Vec(10, -1, 10), new Lambertian(Vec(0.8, 0.6, 0.2))));
	triList.push_back(new Triangle(Vec(-10, -1, -10), Vec(10, -1, 10), Vec(10, -1, -10), new Lambertian(Vec(0.8, 0.6, 0.2))));
	triList.push_back(new Triangle(Vec(0, 6, -30), Vec(20, 6, 15), Vec(-20, 6, -15), new Lambertian(Vec(0.8, 0.6, 0.2), Vec(5,5,5))));

	//BVH *root = new BVH(triList, 0);
	Octree *root = new Octree(triList, 0);

	cout << root->box.bounds[0] << " " << root->box.bounds[2] << " " << root->box.bounds[4] << "\n" << root->box.bounds[1] << " " << root->box.bounds[3] << " " << root->box.bounds[5] << " " << endl;

	Vec fr(2, 3, 6);
	Vec to(0, 0, 0);
	//Vec to = root->box.getCenter();
	Vec up(0, 1, 0);
	float dist_to = (fr - to).length();
	float aperture = 0.5;
	Camera cam(fr, to, up, 45, float(nx) / float(ny), aperture, dist_to);
	Vec *c = new Vec[nx * ny];

#pragma omp parallel for schedule(dynamic)// num_threads(4)

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
				color += getcolor(r, root, 0);
			
			}
			color /= float(ns);
			color = Vec(pow(clamp(color.e[0]), 1.0 / 2.2), pow(clamp(color.e[1]), 1.0 / 2.2), pow(clamp(color.e[2]), 1.0 / 2.2));

			//myfile << int(255.99*color.e[0]) << " " << int(255.99*color.e[1]) << " " << int(255.99*color.e[2]) << "\n";
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
	
	cout << ">> Running time : " << (stop_time - start_time) / 1000.0 << endl;
	cout << ">> Triangles : " << triList.size() << endl;

	cin.get();

    return 0;
}