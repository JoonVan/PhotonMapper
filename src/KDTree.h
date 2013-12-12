#include "sceneStructs.h"
//#include "intersections.h"
#include <vector>

#define SPLIT_BINS 10
#define MAX_PRIMS_PER_LEAF 10
#define MAX_TREE_DEPTH 20
#define EPSILON 0.0001

enum {X_AXIS, Y_AXIS, Z_AXIS};
enum {LEFT, RIGHT, BOTTOM, TOP, BACK, FRONT};

extern glm::vec3* vertices;
extern triangle* faces;
extern int numberOfVertices;
extern int numberOfFaces;
class prim
{
public:
	triangle t;
	int index;
};

class Plane
{
public:
	int axis;
	float splitPoint;
	bool isFirst(prim p);
	bool isSecond(prim p);

	//for checking if a point is in left or right of split
	bool isPointInFirst(glm::vec3 p);
};

class KDNode
{
public:
	glm::vec3 llb; // lower left back
	glm::vec3 urf; // upper right front
	int numberOfPrims;
	int *primIndices;

	int kdIndex;
	KDNode *first;
	KDNode *second;

	Plane splitPlane;

	// ropes
	KDNode* ropes[6];

	bool isLeaf();
};

struct KDNodeGPU
{
	int first, second;
	int ropes[6];
	int startPrimIndex;
	int numPrims;
	glm::vec3 llb;
	glm::vec3 urf;
};

class KDTree
{
public:
	KDNode* tree;

	std::vector<int> primIndex;

	void buildKD();
	KDNode* buildTree(glm::vec3 llb, glm::vec3 urf, std::vector<prim> primsList, int depth);

	KDNode* optimize(KDNode* rope, int side, glm::vec3 llb, glm::vec3 urf);
	void processNode(KDNode* node, KDNode* ropes[]);

	float traverse(ray& r);

	//leave intersection tests here for now
	
	//for testing intersection with bounding box of node
	bool aabbIntersectionTest(glm::vec3 high, glm::vec3 low, ray& r, float& tNear, float& tFar);
	
	//checking intersection with primitives and if intersection point is between entry and exit
	float triangleIntersectionTest(ray& r, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

	//finds which neighbor node to use from an exit point
	KDNode* findNeighbor (glm::vec3 p, KDNode* k);

	// Set indices in dfs order
	void setIndices(KDNode * current, int &index);

	// Put data into GPU friendly tree
	void setGPUTreeData(KDNode * current, KDNodeGPU *gpuTree, std::vector<int> &primIndexList);

	// wrapper to build linear node structure
	int buildGPUKDTree(KDNodeGPU *gpuTree);
};
