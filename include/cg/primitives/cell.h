#pragma once

#include "point.h"
#include <vector>
#include "triangle.h"

#include <cg/io/point.h>

namespace cg
{
    template <class Scalar>
    struct face_2t;
    template <class Scalar>
    struct edge_2t;
    template <class Scalar>
    struct vertex_2t;

    template <class Scalar>
    using vertex_handle = std::shared_ptr<vertex_2t <Scalar> >;
    template <class Scalar>
    using face_handle = std::shared_ptr<face_2t <Scalar> >;
    template <class Scalar>
    using edge_handle = std::shared_ptr<edge_2t <Scalar> >;

    template <class Scalar>
    struct vertex_2t : public point_2t<Scalar>
    {
        vertex_2t() {}

        vertex_2t(const Scalar & p1, const Scalar & p2)
            : point_2t<Scalar>(p1, p2)
        {}

        vertex_2t(const point_2t<Scalar> & p, const edge_handle<Scalar> & e)
            : point_2t<Scalar>(p), _e(e)
        {}

        void set_edge(const edge_handle<Scalar> & e)
        {
            _e = e;
        }

        edge_handle<Scalar> get_edge()
        {
            return _e;
        }

    private:
        edge_handle<Scalar> _e;
    };

    template <class Scalar>
    struct face_2t
    {
        face_2t()
        {}

        face_2t(const edge_handle<Scalar> & e)
            : _e(e)
        {}

        void set_edge(const edge_handle<Scalar> & e)
        {
            _e = e;
        }

        face_handle<Scalar> neighbour(size_t index)
        {
            edge_handle<Scalar> e = _e;
            for(size_t i = 0; i < index; ++i)
                e = e->next();
            e = e->twin();
            return e->face();
        }

        edge_handle<Scalar> edge(size_t index)
        {
            edge_handle<Scalar> e = _e;
            for(size_t i = 0; i < index; ++i)
                e = e->next();
            return e;
        }

    private:
        edge_handle<Scalar> _e;
    };

    template <class Scalar>
    struct edge_2t
    {
        edge_2t() {}

        edge_2t(const face_handle<Scalar> & f, const vertex_handle<Scalar> & v,
                const edge_handle<Scalar> & next, const edge_handle<Scalar> & twin)
            : _f(f), _twin(twin), _v(v), _next(next)
        {}

        vertex_handle<Scalar> vertex() const
        {
            return _v;
        }

        face_handle<Scalar> face() const
        {
            return _f;
        }

        edge_handle<Scalar> twin() const
        {
            return _twin;
        }

        edge_handle<Scalar> next() const
        {
            return _next;
        }

        void set_twin(const edge_handle<Scalar> & twin)
        {
            _twin = twin;
        }

        void set_next(const edge_handle<Scalar> & next)
        {
            _next = next;
        }

        void set_face(const face_handle<Scalar> & f)
        {
            _f = f;
        }

        void set_vertex(const vertex_handle<Scalar> & v)
        {
            _v = v;
        }

    private:
        vertex_handle<Scalar> _v;
        face_handle<Scalar> _f;
        edge_handle<Scalar> _twin, _next;
    };

    template <class Scalar>
    struct cell_2t
    {
        const double INF = 100.0;

        cell_2t()
        {
            _vtxs.push_back(std::shared_ptr <vertex_2t <Scalar> >(new vertex_2t <Scalar>(-INF, -INF)));
            _vtxs.push_back(std::shared_ptr <vertex_2t <Scalar> >(new vertex_2t <Scalar>(INF, -INF)));
            _vtxs.push_back(std::shared_ptr <vertex_2t <Scalar> >(new vertex_2t <Scalar>(0, INF)));
            for(size_t i = 0; i < 2; ++i)
                _fcs.push_back(std::shared_ptr <face_2t <Scalar> >(new face_2t <Scalar>()));
            for(size_t i = 0; i < 6; ++i)
                _edgs.push_back(std::shared_ptr <edge_2t <Scalar> >(new edge_2t <Scalar>()));

            for(size_t i = 0; i < 6; ++i)
            {
                _edgs[i]->set_face(_fcs[i/3]);
                _edgs[i]->set_vertex(_vtxs[i % 3]);
                _edgs[i]->set_twin(_edgs[(i + 3) % 6]);
                _edgs[i]->set_next(_edgs[(3 * (i / 3)) + (i + 1) % 3]);
            }

            for(size_t i = 0; i < 3; ++i)
                _vtxs[i]->set_edge(_edgs[i]);

            for(size_t i = 0; i < 2; ++i)
                _fcs[i]->set_edge(_edgs[i * 3]);
        }

        std::vector < triangle_2t <Scalar> > get_triangulation()
        {
            std::vector < triangle_2t <Scalar> > res;
            for(auto face : _fcs)
            {
                res.push_back(triangle_2t<Scalar>(
                                  *face->edge(0)->vertex(),
                                  *face->edge(1)->vertex(),
                                  *face->edge(2)->vertex()));
            }
            return res;
        }

//        void add_vertex(const vertex_2t & v);
//        face_handle locate_vertex();

    private:
        std::vector < std::shared_ptr < vertex_2t <Scalar> > > _vtxs;
        std::vector < std::shared_ptr < edge_2t <Scalar> > > _edgs;
        std::vector < std::shared_ptr < face_2t <Scalar> > > _fcs;
    };
}
