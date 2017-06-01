#ifndef _GEOC_TRIANGULATION_H
#define _GEOC_TRIANGULATION_H

#include <geoc/geoc.h>
#include <geoc/geometry/TriangulationBase.h>
#include <geoc/math/Vector_fwd_decl.h>
#include <vector>

#include <geoc/geometry/Triangle.h>
#include <geoc/geometry/LineSegment.h>

namespace geoc {

class LineSegmentEnt;
class TriangleEnt;

/** \ingroup Geometry */
//! A triangulation of a set of points in the plane.
class Triangulation : public TriangulationBase {
       public:
	Triangulation();
	~Triangulation();

	void triangulate(const std::vector<Vector3>& ps,
	                 const std::vector<int>& idxs,
	                 std::vector<LineSegmentEnt>& segments,
	                 std::vector<TriangleEnt>& triangles,
	                 std::vector<TriangleEnt>& triangles_pruned);

       private:
    struct Edge;
    struct Face;

    struct Vertex {
        const Vector3& point;
        Edge* edge;
    };
    struct Face {
        Edge* edge;
    };
    struct Edge {
        Vertex* vertex;
        Face* face;
        Edge* next;
        Edge* twin;
    };

    std::vector<Vector3> initial_points;
    std::vector<Vertex*>  vertices;
    std::vector<Face*>    faces;
    std::vector<Edge*>    edges;
    Vector3 aux_point;
    Face* aux_point_face;

    std::string error_message;
    std::string panic_message;

    void getEdgesOfFace(const Face* face, std::vector<Edge*>& face_edges);
    bool insertPoint(const Vector3& p);
    bool isPointInFace(const Face* face, const Vector3& point);
    bool isEdgeIntersectingSegment(const Edge* edge, const LineSegment& segment);
    Face* findFace(const Vector3& p);
    Vertex* updateFace(Face* face, const Vector3& p);
    void delaunay(Vertex* vertex);
    bool isPointInFaceCircumcircle(Face* face, const Vector3& p);
    void updateAuxPoint();
    void updateAuxPoint2();
    void prune(const std::vector<Vector3>& ps, const std::vector<int>& idxs, std::set<Face*>& faces_to_ignore);
    Edge* findEdge(const Vertex* s, const Vertex* e);
    LineSegment getEdgeAsSegment(const Edge* edge);
    Triangle getFaceAsTriangle(const Face* face);

};

}  // namespace geoc

#endif  //_GEOC_TRIANGULATION_H
