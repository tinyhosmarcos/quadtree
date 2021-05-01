
#include "LeafLetParser.h"
#include <fstream>
#include <string>
#include <random>
#include <unordered_map>
#include <cfloat>
#include <sstream>
using namespace std;

string read_file(string filename){
	ifstream file;
	string linea_leida,header;
	file.open(filename);
	file.is_open();
	while(getline(file, linea_leida))
		header += linea_leida + "\n";
	file.close();
	return header;
}

int main(int argc, char** argv){
	unsigned int tree_depth;
    stringstream geek(argv[1]);
    geek>>tree_depth;
	string html_format=read_file("HtmlFormat.txt");
	string script=read_file("ScriptMap.txt");

	ifstream data_set;
	string filename = "Points.txt";
	data_set.open(filename);
	double rndlng, rndlat;
	double latitud_array[2],longitud_array[2] ;
	vector<pair<double, double>> DataSetPoints;
	data_set.is_open();
	while (data_set >> rndlat >> rndlng) {
		DataSetPoints.push_back(make_pair(rndlng, rndlat));
		latitud_array[0] = min(latitud_array[0], rndlat);
		latitud_array[1] = max(latitud_array[1], rndlat);
		longitud_array[0] = min(longitud_array[0], rndlng);
		longitud_array[1] = max(longitud_array[1], rndlng);
 		}
	data_set.close();
	
	pair<double, double> Box[2] = { make_pair(longitud_array[0], longitud_array[1]),make_pair(latitud_array[0], latitud_array[1]) };

	QuadTree map_quad(Box, tree_depth);
	size_t DataSetSize = DataSetPoints.size();
	for (size_t i = 0; i < DataSetSize; ++i) {
		map_quad.insert(DataSetPoints[i]);
	}

	vector<RectangleLeaf> Rectangles = leaflet::GenerateLeafRectangles(map_quad);
	
	unordered_map<string, string> Rect;
	Rect["first_parameter"] = "L.rectangle(";
	Rect["second_parameter"] = ", {color: \"";
	Rect["third_parameter"] = "\", weight: 1, fillOpacity: 1, fill: true}).addTo(mymap);\n";
	string rectangles_points = "";
	int RectanglesSize = Rectangles.size();
	for (int i = 0; i < RectanglesSize; ++i) {
		rectangles_points += Rect["first_parameter"];
		rectangles_points += "[[" + to_string(Rectangles[i].ldcorner.first) + "," + to_string(Rectangles[i].ldcorner.second) + "],";
		rectangles_points += "[" + to_string(Rectangles[i].rucorner.first) + "," + to_string(Rectangles[i].rucorner.second) + "]]";
		rectangles_points += Rect["second_parameter"] + Rectangles[i].color + Rect["third_parameter"];
	}
	ofstream map_file;
	filename = "map.html"; 
	map_file.open(filename);
	map_file << html_format;
	map_file << rectangles_points;
	map_file << script; 
	map_file.close();
	return 0;
}