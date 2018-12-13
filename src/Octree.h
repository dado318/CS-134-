#pragma once
#include "ofMain.h"
#include "box.h"
#include "ray.h"


class TreeNode {
public:
    Box box;
    vector<int> points;
    vector<TreeNode> children;
};

class Octree {
    const ofColor colors[7]{ ofColor::red, ofColor::green, ofColor::blue, ofColor::purple, ofColor::orange, ofColor::white, ofColor::yellow };
public:
    void create(const ofMesh & mesh, int numLevels);
    void subdivide(const ofMesh & mesh, const vector<int> & points, TreeNode & node, int numLevels, int level);
    bool intersect(const Ray &, const TreeNode & node, TreeNode & nodeRtn);
    void draw(TreeNode & node, int numLevels, int level);
    void draw(int numLevels, int level) {
        draw(root, numLevels, level);
    }
    void drawLeafNodes(TreeNode & node);
    void drawBox(const Box &box, int colorIndex = 1);
    static Box meshBounds(const ofMesh &);
    int getMeshPointsInBox(const ofMesh &mesh, const vector<int> & points, Box & box, vector<int> & pointsRtn);
    void subDivideBox8(const Box &b, vector<Box> & boxList);
    
    ofMesh mesh;
    TreeNode root;
};
