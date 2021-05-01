#include "QuadTree.h"
#include <vector>
#include <math.h>
typedef std::pair<double, double> pair_dd;
class RectangleLeaf {
public:
	pair_dd ldcorner;	
	pair_dd rucorner;	
	std::string color; // 
};
namespace leaflet {
	std::vector<RectangleLeaf> GenerateLeafRectangles(QuadTree& QTree) {
		std::vector<std::string> PaintColors = {"#FB3F30","#FBBA30","#FBF830"};
		std::vector<RectangleLeaf> Rectangles;
		RectangleLeaf rectanglesLeaft;
		Node* root = QTree.getRoot();
		if (root->IsLeaf()) {
			rectanglesLeaft.ldcorner = std::make_pair(root->QuadBox[1].first, root->QuadBox[0].first);
			rectanglesLeaft.rucorner = std::make_pair(root->QuadBox[1].second, root->QuadBox[0].second);
			rectanglesLeaft.color = PaintColors[0];
			Rectangles.push_back(rectanglesLeaft);
			return Rectangles;
		}
		else {
			std::stack<std::pair<Node*, unsigned int>> NodesPool;
			NodesPool.push(std::make_pair(root, 0)); 
			while (!NodesPool.empty()) {
				std::pair<Node*, unsigned int> top = NodesPool.top();
				NodesPool.pop();
				for (auto& child : top.first->child){
					if (child) {
						if (child->IsLeaf()){
							rectanglesLeaft.ldcorner = std::make_pair(child->QuadBox[1].first, child->QuadBox[0].first);
							rectanglesLeaft.rucorner = std::make_pair(child->QuadBox[1].second, child->QuadBox[0].second);
							double leafdepth = static_cast<double>(top.second) + 1.0;
							if (leafdepth >=  0.85 * static_cast<double>(QTree.getDepth())) 
								rectanglesLeaft.color = PaintColors[2]; // some red
							else if(leafdepth >= 0.75 * static_cast<double>(QTree.getDepth())) 
								rectanglesLeaft.color = PaintColors[1]; // some orange
							else
								rectanglesLeaft.color = PaintColors[0]; // some yellow

							Rectangles.push_back(rectanglesLeaft);
						}
						else 
							NodesPool.push(std::make_pair(child, top.second + 1)); // the parent depth plus one
					}
				}
			}
		}
		return Rectangles;
	}

};