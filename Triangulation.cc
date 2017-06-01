#include <geoc/geometry/Triangulation.h>
#include <geoc/geometry/Triangle.h>
#include <geoc/geometry/Circle.h>
#include <geoc/math/Math.h>
#include <geoc/scene/LineSegmentEnt.h>
#include <geoc/scene/TriangleEnt.h>
#include <geoc/GeocException.h>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <queue>
#include <cassert>

using namespace geoc;
using namespace std;

Triangulation::Triangulation() {
}

Triangulation::~Triangulation() {
}

void Triangulation::triangulate(const std::vector<Vector3>& ps,
                                const std::vector<int>& idxs,
                                std::vector<LineSegmentEnt>& segments,
                                std::vector<TriangleEnt>& triangles,
                                std::vector<TriangleEnt>& triangles_pruned) {
	printf("Compiling student triangulation\n");

    // Calculate bounding triangle
    Vector3 max, min;
    max = ps[0];
    min = ps[0];

    for (auto& v : ps)
    {
        max[X] = max[X] < v[X]? v[X] : max[X];
        min[X] = min[X] > v[X]? v[X] : min[X];

        max[Y] = max[Y] < v[Y]? v[Y] : max[Y];
        min[Y] = min[Y] > v[Y]? v[Y] : min[Y];

        max[Z] = max[Z] < v[Z]? v[Z] : max[Z];
        min[Z] = min[Z] > v[Z]? v[Z] : min[Z];
    }

    max += Vector3(1,1,0);
    min += Vector3(-1,-1,0);

    Vector3 diff = max - min;
    Vector3 init_1, init_2, init_3;
    init_1 = min;
    init_1[X] += 0.5 * diff[X];
    init_1[Y] -= diff[Y];
    init_2 = max;
    init_2[X] += 0.5 * diff[X];
    init_3 = max;
    init_3[X] -= 1.5 * diff[X];
    initial_points.push_back(init_1);
    initial_points.push_back(init_2);
    initial_points.push_back(init_3);

    // Initialize DCEL
    vertices.push_back(new Vertex{initial_points[0], nullptr});
    Vertex* v1 = vertices[0];
    vertices.push_back(new Vertex{initial_points[1], nullptr});
    Vertex* v2 = vertices[1];
    vertices.push_back(new Vertex{initial_points[2], nullptr});
    Vertex* v3 = vertices[2];

    faces.push_back(new Face());
    Face* face = faces[0];

    edges.push_back(new Edge{v1, face, nullptr, nullptr});
    Edge* e1 = edges[edges.size()-1];
    v1->edge = e1;
    face->edge = e1;
    edges.push_back(new Edge{v3, nullptr, nullptr, e1});
    Edge* e2 = edges[edges.size()-1];
    v3->edge = e2;
    e1->twin = e2;

    edges.push_back(new Edge{v3, face, nullptr, nullptr});
    Edge* e3 = edges[edges.size()-1];
    edges.push_back(new Edge{v2, nullptr, e2, e3});
    Edge* e4 = edges[edges.size()-1];
    v2->edge = e4;
    e3->twin = e4;
    e1->next = e3;

    edges.push_back(new Edge{v1, nullptr, e4, nullptr});
    Edge* e5 = edges[edges.size()-1];
    edges.push_back(new Edge{v2, face, e1, e5});
    Edge* e6 = edges[edges.size()-1];
    e5->twin = e6;
    e3->next = e6;
    e2->next = e5;

    // Auxuliary reference point
    aux_point = min + diff/2;
    aux_point_face = face;

    unsigned int p_count = 0;
    for (int i = 0; i < ps.size(); ++i)
    {
        insertPoint(ps[i]);

        if (++p_count % (ps.size()/10) == 0)
        {
            std::cout << "[PROGRESS] " << ((float)p_count/(float)ps.size()) * 100 << '%' << " of points inserted" << std::endl;
        }
    }

    std::cout << "[PROGRESS] 100.0" << '%' << " of points inserted" << std::endl;


    for (Face* face : faces)
    {
        Triangle t = getFaceAsTriangle(face);
        if (t[0] == initial_points[0] or t[0] == initial_points[1] or t[0] == initial_points[2] or
            t[1] == initial_points[0] or t[1] == initial_points[1] or t[1] == initial_points[2] or
            t[2] == initial_points[0] or t[2] == initial_points[1] or t[2] == initial_points[2])
            continue;
        triangles.push_back(t);
    }

    std::set<Face*> faces_to_ignore;
    prune(ps, idxs, faces_to_ignore);

    for (Face* face : faces)
    {
        if (faces_to_ignore.find(face) != faces_to_ignore.end())
            continue;
        Triangle t = getFaceAsTriangle(face);
        if (t[0] == initial_points[0] or t[0] == initial_points[1] or t[0] == initial_points[2] or
            t[1] == initial_points[0] or t[1] == initial_points[1] or t[1] == initial_points[2] or
            t[2] == initial_points[0] or t[2] == initial_points[1] or t[2] == initial_points[2])
            continue;
        triangles_pruned.push_back(t);
    }

    initial_points.clear();
    for (Vertex* v : vertices)
        delete v;
    vertices.clear();
    for (Edge* e : edges)
        delete e;
    edges.clear();
    for (Face* f : faces)
        delete f;
    faces.clear();
}

void Triangulation::prune(const std::vector<Vector3>& ps, const std::vector<int>& idxs, std::set<Face*>& faces_to_ignore)
{
    if (idxs.empty()) return;
    Vertex* s = vertices[idxs[0] + 3];
    Vertex* b_begin = s;
    Vertex* t = vertices[idxs[1] + 3];;
    Edge* current_edge = findEdge(t, s);

    for (int i = 2; i < idxs.size(); ++i)
    {
        int idx = idxs[i];
        Vertex* goal;

        goal = idx < 0 ? current_edge->twin->vertex : vertices[idx + 3];

        int count = 0;
        do
        {
            faces_to_ignore.insert(current_edge->twin->face);
            current_edge = current_edge->twin->next;
        } while (current_edge->twin->vertex != goal );

        current_edge = current_edge->twin;

        if (idx < 0)
        {
            s = vertices[idxs[++i] + 3];
            b_begin = s;
            t = vertices[idxs[++i] + 3];;
            current_edge = findEdge(t, s);
        }
    }
}

Triangulation::Edge* Triangulation::findEdge(const Vertex* s, const Vertex* e)
{
    Edge* current_edge = s->edge;
    do
    {
        if (current_edge->twin->vertex == e)
        {
            return current_edge;
        }
        current_edge = current_edge->twin->next;
    } while (current_edge != s->edge);

    return nullptr;
}

bool Triangulation::insertPoint(const Vector3& p)
{
    Face* face = findFace(p);
    if (face == nullptr)
        return false;
    Vertex* vertex = updateFace(face, p);

    delaunay(vertex);
    return true;
}

Triangulation::Face* Triangulation::findFace(const Vector3& p)
{
    updateAuxPoint2();
    Face* current_face = aux_point_face;
    Edge* previous_edge_crossed = nullptr;
    LineSegment guide_segment(aux_point, p);

    while (not isPointInFace(current_face, p))
    {
        std::vector<Edge*> face_edges;
        getEdgesOfFace(current_face, face_edges);

        current_face = nullptr;

        for (Edge* edge : face_edges)
        {
            if (edge == previous_edge_crossed) continue;
            if (isEdgeIntersectingSegment(edge, guide_segment))
            {
                current_face = edge->twin->face;
                previous_edge_crossed = edge->twin;
                break;
            }
        }
        if (current_face == nullptr)
        {
            return nullptr;
        }
    }
    return current_face;
}

Triangulation::Vertex* Triangulation::updateFace(Face* face, const Vector3& p)
{
    std::vector<Edge*> face_edges;
    getEdgesOfFace(face, face_edges);
    vertices.push_back(new Vertex{p, nullptr});
    Vertex* new_vertex = vertices[vertices.size()-1];

    face->edge = face_edges[0];
    edges.push_back(new Edge{new_vertex, face, nullptr, nullptr});
    edges.push_back(new Edge{face_edges[1]->vertex, face, nullptr, nullptr});
    Edge* e1 = edges[edges.size()-2];
    Edge* e2 = edges[edges.size()-1];
    new_vertex->edge = e1;
    face_edges[0]->next = e2;
    face_edges[0]->face = face;
    e2->next = e1;
    e1->next = face_edges[0];

    faces.push_back(new Face{face_edges[1]});
    Face* face2 = faces[faces.size()-1];
    edges.push_back(new Edge{face_edges[2]->vertex, face2, nullptr, nullptr});
    Edge* e3 = edges[edges.size()-1];
    face_edges[1]->next = e3;
    face_edges[1]->face = face2;
    edges.push_back(new Edge{new_vertex, face2, face_edges[1], e2});
    Edge* e4 = edges[edges.size()-1];
    e2->twin = e4;
    e3->next = e4;

    faces.push_back(new Face{face_edges[2]});
    Face* face3 = faces[faces.size()-1];
    edges.push_back(new Edge{face_edges[0]->vertex, face3, nullptr, e1});
    Edge* e5 = edges[edges.size()-1];
    e1->twin = e5;
    face_edges[2]->next = e5;
    face_edges[2]->face = face3;
    edges.push_back(new Edge{new_vertex, face3, face_edges[2], e3});
    Edge* e6 = edges[edges.size()-1];
    e3->twin = e6;
    e5->next = e6;

    return new_vertex;
}

void Triangulation::delaunay(Vertex* vertex)
{
    std::queue<Edge*> Q;
    Edge* current_edge = vertex->edge;
    do
    {
        Q.push(current_edge);
        current_edge = current_edge->twin->next;
    } while (current_edge != vertex->edge);

    while (not Q.empty())
    {
        Edge* current_edge = Q.front();
        Q.pop();
        Face* face = current_edge->next->twin->face;
        if (face != nullptr and isPointInFaceCircumcircle(face, vertex->point))
        {
            Face *f1, *f2;
            f1 = current_edge->face;
            f2 = current_edge->next->twin->face;

            Edge *e1, *e2, *e3, *e4;
            e1 = current_edge->next;
            e2 = current_edge->next->next;
            e4 = e1->twin->next;
            e3 = e4->next;

            Vertex* v = e1->vertex;

            v->edge = e4;
            current_edge->next = e4;
            e4->next = e1;
            e4->face = f1;
            e1->next = current_edge;
            e1->vertex = e4->twin->vertex;
            e1->twin->vertex = vertex;
            e1->twin->next = e3;
            e3->next = e2;
            e2->face = f2;
            e2->next = e1->twin;
            f1->edge = current_edge;
            f2->edge = e2;

            Q.push(current_edge);
            Q.push(e1->twin);
        }
    }

    updateAuxPoint();
}

void Triangulation::updateAuxPoint()
{

    Face* prev_face = aux_point_face;
    // choose a random face
    Face* face = faces[(rand() * rand()) % faces.size()];

    const Vector3& p1 = face->edge->vertex->point;
    const Vector3& p2 = face->edge->next->vertex->point;
    const Vector3& p3 = face->edge->next->next->vertex->point;

    aux_point = (p1 + p2 + p3)/3;
    aux_point_face = face;
}

void Triangulation::updateAuxPoint2()
{

    Face* face = aux_point_face;

    const Vector3& p1 = face->edge->vertex->point;
    const Vector3& p2 = face->edge->next->vertex->point;
    const Vector3& p3 = face->edge->next->next->vertex->point;

    aux_point = (p1 + p2 + p3)/3;
}

bool Triangulation::isPointInFaceCircumcircle(Face* face, const Vector3& p)
{
    const Vector3& p1 = face->edge->vertex->point;
    const Vector3& p2 = face->edge->next->vertex->point;
    const Vector3& p3 = face->edge->next->next->vertex->point;

    Circle circle(p3, p2, p1);
    Colour3 c;
    std::string desc;
    classify(circle, p, c, desc);
    if (desc != "Interior")
    {
        error_message = "[ERROR] point-face check returned '" + desc + "'";
        return false;
    }
    return true;
}

LineSegment Triangulation::getEdgeAsSegment(const Edge* edge)
{
    return LineSegment(edge->vertex->point, edge->twin->vertex->point);
}

Triangle Triangulation::getFaceAsTriangle(const Face* face)
{
    const Vector3& point1 = face->edge->vertex->point;
    const Vector3& point2 = face->edge->next->vertex->point;
    const Vector3& point3 = face->edge->next->next->vertex->point;
    return Triangle(point3, point2, point1);
}

bool Triangulation::isPointInFace(const Face* face, const Vector3& point)
{
    Triangle t = getFaceAsTriangle(face);
    Colour3 c;
    std::string desc;
    classify(t, point, c, desc);
    if (desc != "Interior")
    {
        error_message = "[ERROR] point-face check returned '" + desc + "'";
        return false;
    }
    return true;
}

bool Triangulation::isEdgeIntersectingSegment(const Edge* edge, const LineSegment& segment)
{
    Colour3 c;
    std::string desc;
    classifyIntersection(getEdgeAsSegment(edge), segment, c, desc);
    if (desc != "Full intersection")
    {
        error_message = "[ERROR] edge - segment intersection returned '" + desc + "'";
        return false;
    }
    return true;
}

// Returns the edges of the face (the ones that have the face to the right)
void Triangulation::getEdgesOfFace(const Face* face, std::vector<Edge*>& face_edges)
{
    face_edges.push_back(face->edge);
    face_edges.push_back(face->edge->next);
    face_edges.push_back(face->edge->next->next);
}
