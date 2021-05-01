#include <iostream>
#include <vector>
#include <utility>
#include <stack>
#include <algorithm>
#define MAX_DEPTH 18
using namespace std;
class Node{ 
public:
	pair<double, double> QuadBox[2];
	bool colored;
	Node* child[4];
	Node(pair<double, double> _QuadBox[], bool _colored = false) {
		QuadBox[0] = _QuadBox[0];
		QuadBox[1] = _QuadBox[1];
		colored = _colored;          
		for (auto& child : child) 
			child = nullptr;
	}
	Node(vector<pair<double, double>> _QuadBox, bool _colored = false) {
		QuadBox[0] = _QuadBox[0];
		QuadBox[1] = _QuadBox[1];
		colored = _colored;          
		for (auto& child : child) 
			child = nullptr;
	}
	Node(Node &_node) { 
		QuadBox[0] = _node.QuadBox[0];
		QuadBox[1] = _node.QuadBox[1];
		colored = _node.colored;
		for (int i = 0; i < 4; ++i)
			child[i] = _node.child[i];
	}
	vector<pair<double,double>> GenerateQuadBox(int quadrant) { 
		if (quadrant < 0 || quadrant > 3) return vector<pair<double,double>>();
		vector<pair<double, double>> NewQuadBox(2); 
		double mid1 = QuadBox[0].first + (QuadBox[0].second - QuadBox[0].first) / 2; 
		double mid2 = QuadBox[1].first + (QuadBox[1].second - QuadBox[1].first) / 2; 
		switch (quadrant){
		case 0: 
			NewQuadBox.assign({ make_pair(QuadBox[0].first, mid1), make_pair(mid2, QuadBox[1].second) });
			break;
		case 1: 
			NewQuadBox.assign({ make_pair(mid1, QuadBox[0].second), make_pair(mid2, QuadBox[1].second) });
			break;
		case 2: 
			NewQuadBox.assign({ make_pair(QuadBox[0].first, mid1), make_pair(QuadBox[1].first, mid2) });
			break;
		case 3: 
			NewQuadBox.assign({ make_pair(mid1, QuadBox[0].second), make_pair(QuadBox[1].first, mid2) });
			break;
		}
		return NewQuadBox;
	}

	bool IsOnBox(pair<double, double> pair_points) {
		return (QuadBox[0].first <= pair_points.first && QuadBox[0].second >= pair_points.first)
			&& (QuadBox[1].first <= pair_points.second && QuadBox[1].second >= pair_points.second);
	}
	short ChooseLeaf(pair<double, double> pair_points) {
		if (!IsOnBox(pair_points))
			return -1;

		double mid1 = QuadBox[0].first + (QuadBox[0].second - QuadBox[0].first) / 2; 
		double mid2 = QuadBox[1].first + (QuadBox[1].second - QuadBox[1].first) / 2; 

		if ((QuadBox[0].first <= pair_points.first && pair_points.first <= mid1) && (mid2 <= pair_points.second && pair_points.second <= QuadBox[1].second))
			return 0; 
	
		if ((mid1 <= pair_points.first && pair_points.first <= QuadBox[0].second) && (mid2 <= pair_points.second && pair_points.second <= QuadBox[1].second))
			return 1;
	
		if ((QuadBox[0].first <= pair_points.first && pair_points.first <= mid1) && (QuadBox[1].first <= pair_points.second && pair_points.second <= mid2))
			return 2; 

		return 3; 
	}
	void SetQuadBox(pair<double, double> _QuadBox[], short lenght = 2) {
		QuadBox[0] = _QuadBox[0];
		QuadBox[1] = _QuadBox[1];
	}
	bool IsLeaf() {
		for (auto& child : child)
			if (child) return false;
		return colored;
	}
	
	~Node() {
		for (auto& child : child)
			delete child;
	}
};

class QuadTree{
private:
	unsigned int TreeDepth; 
	Node* root;
public:
	QuadTree(pair<double,double> _QuadBox[], unsigned int _TreeDepth) {
		root = new Node(_QuadBox);
		TreeDepth = min(static_cast<unsigned int>(MAX_DEPTH), _TreeDepth);
	}
	QuadTree(vector<pair<double, double>> _QuadBox, unsigned int _TreeDepth) {
		root = new Node(_QuadBox);
		TreeDepth = min(static_cast<unsigned int>(MAX_DEPTH), _TreeDepth);
	}
	QuadTree(Node _root, unsigned int _TreeDepth) {
		root = new Node(_root);
		TreeDepth = min(static_cast<unsigned int>(MAX_DEPTH), _TreeDepth);
	}

	Node* getRoot() {
		return root;
	}

	unsigned int getDepth() {
		return TreeDepth;
	}

	bool find(pair<double, double> pair_points, unsigned int& depth) {
		Node *ptr = root;
		int quadrant;
		depth = 0;
		if (!ptr || !ptr->IsOnBox(pair_points))
			return false;
		while (!ptr->IsLeaf()) {
			quadrant = ptr->ChooseLeaf(pair_points);
			if (!ptr->child[quadrant]) return false;
			ptr = ptr->child[quadrant];
			++depth; 
		}
		return true;
	}
	bool find(pair<double, double> pair_points, unsigned int& depth, stack<Node**>& ptrs) {
		ptrs.push(&root);
		int quadrant;
		depth = 0; 
		
		if (!(*ptrs.top()) || !(*ptrs.top())->IsOnBox(pair_points))
			return false;
		
		
		while (!(*ptrs.top())->IsLeaf()) {
			quadrant = (*ptrs.top())->ChooseLeaf(pair_points);
			
			if (!(*ptrs.top())->child[quadrant]) return false;
			
			ptrs.push(&(*ptrs.top())->child[quadrant]);
			++depth; 
		}
		return true;
	}


	short insert(pair<double, double> pair_points) {
		stack<Node**> ptrs;
		unsigned int depth;
		if (find(pair_points, depth, ptrs))
			return -1;
		int quadrant;
		vector<pair<double, double>> NewQuadBox;
	
		while (depth < TreeDepth - 1) { 
			quadrant = (*ptrs.top())->ChooseLeaf(pair_points); 
			NewQuadBox = (*ptrs.top())->GenerateQuadBox(quadrant);
			(*ptrs.top())->child[quadrant] = new Node(NewQuadBox);
			ptrs.push(&(*ptrs.top())->child[quadrant]);
			++depth;
		}

		(*ptrs.top())->colored = true;
		ptrs.pop(); 

		while (!ptrs.empty()) {
			bool is_full = 1;
			for (auto& child : (*ptrs.top())->child)
				if (!child || !child->IsLeaf()) is_full = 0;

			if (is_full) {
				
				for (auto& child : (*ptrs.top())->child) {
					delete child; 
					child = nullptr; 
				}
				
				(*ptrs.top())->colored = true;
			}
			ptrs.pop();
		}
		return depth;
	}

	bool remove(pair<double, double> pair_points) {
		stack<Node**> ptrs;
		unsigned int depth;
		if (!find(pair_points, depth, ptrs)) return false;
		int quadrant;	
		while (depth < TreeDepth - 1) {
			quadrant = (*ptrs.top())->ChooseLeaf(pair_points);
			for (int i = 0; i < 4; i++)
				(*ptrs.top())->child[i] = new Node((*ptrs.top())->GenerateQuadBox(i), true);
			
			(*ptrs.top())->colored = false; 
			ptrs.push(&(*ptrs.top())->child[quadrant]);
			++depth;
		}

		// the top of stack is the last child removed
		delete *ptrs.top();
		*ptrs.top() = nullptr;
		// we remove this node from the stack, because the parents needs to evaluate if they are
		// empty done this operation
		ptrs.pop();

		while (!ptrs.empty()) {
			bool is_empty = 1;
			for (auto& child : (*ptrs.top())->child)
				if (child) is_empty = 0;

			if (is_empty && *ptrs.top() != root) {
				delete* ptrs.top();
				*ptrs.top() = nullptr;
			}
			ptrs.pop();
		}
		return true;
	}


	~QuadTree() {
		delete root;
	}
};