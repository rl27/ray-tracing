#include <fstream>
#include <iostream>
#include <ctime>
#include <limits>
#include "Triangle.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Octree.h"
using namespace std;

Vec getcolor(const Ray &r, Octree *tree, int depth)
{
	vector<Triangle*> list;
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
			return att * getcolor(scatt, tree, depth + 1);
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
	int ny = 300;
	int ns = 2;
	myfile << "P3\n" << nx << " " << ny << "\n255\n";

	vector<Mesh*> meshes;
	/*
	meshes.push_back(new Mesh("obj/dodecahedron.obj", Vec(2, 0.2, 0.7)));
	meshes.push_back(new Mesh("obj/dodecahedron.obj", Vec(3.2, 0.5, 2.5)));
	meshes.push_back(new Mesh("obj/cube.obj", Vec(-3, 1, -0.7)));
	meshes.push_back(new Mesh("obj/cube.obj", Vec(2, -1, -0.5)));
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(0.3, 1, 4)));
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(2, 3, 1.5)));
	*/
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(-0.5,0,0)));
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(1,1,1)));
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(1,2,3)));
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(-1,2,-1)));
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(2,-1.5,-2)));
	meshes.push_back(new Mesh("obj/sphere.obj", Vec(-2,-2,1.5)));
	meshes.push_back(new Mesh("obj/dodecahedron.obj", Vec(4,0,0)));
	meshes.push_back(new Mesh("obj/dodecahedron.obj", Vec(4,-2,0.5)));
	meshes.push_back(new Mesh("obj/dodecahedron.obj", Vec(2,-4,-1)));

	vector<Triangle*> triList;
	for (int i = 0; i < meshes.size(); i++)
	{
		for (int j = 0; j < meshes.at(i)->triangles.size(); j++)
			triList.push_back(meshes.at(i)->triangles.at(j));
	}

	Octree *root = new Octree(triList, 0);
	
	Vec fr(-5, 0, 10);
	//Vec to(0, 0, 0);
	Vec to = root->box.getCenter();
	Vec up(0, 1, 0);
	float dist_to = (fr - to).length();
	float aperture = 0;
	Camera cam(fr, to, up, 45, float(nx) / float(ny), aperture, dist_to);

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
			
			color = Vec(pow(color.e[0], 1.0 / 2.2), pow(color.e[1], 1.0 / 2.2), pow(color.e[2], 1.0 / 2.2));

			myfile << int(255.99*color.e[0]) << " " << int(255.99*color.e[1]) << " " << int(255.99*color.e[2]) << "\n";
	    }
    }

    myfile.close();

	int stop_time = clock();
	cout << ">> Running time : " << (stop_time - start_time) / 1000.0 << endl;
	cout << ">> Triangles : " << triList.size() << endl;

	cin.get();

    return 0;
}