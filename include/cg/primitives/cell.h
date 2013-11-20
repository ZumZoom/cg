#pragma once

#include "point.h"
#include <vector>
#include "triangle.h"

#include <cg/operations/contains/triangle_point.h>
#include <cg/operations/contains/segment_point.h>
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

        vertex_2t(const point_2t<Scalar> & p)
            : point_2t<Scalar>(p)
        {}

        vertex_2t(const point_2t<Scalar> & p, const edge_handle<Scalar> & e)
            : point_2t<Scalar>(p), _e(e)
        {}

        void set_edge(const edge_handle<Scalar> & e)
        {
            _e = e;
        }

        edge_handle<Scalar> edge() const
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

        face_handle<Scalar> neighbour(size_t index) const
        {
            edge_handle<Scalar> e = _e;
            for(size_t i = 0; i < index; ++i)
                e = e->next();
            e = e->twin();
            return e->face();
        }

        vertex_handle<Scalar> vertex(size_t index) const
        {
            return edge(index)->vertex();
        }

        edge_handle<Scalar> edge(size_t index) const
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

        edge_handle<Scalar> prev() const
        {
            return _next->next();
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
        typedef typename std::vector < std::shared_ptr < face_2t <Scalar> > >::const_iterator const_iterator;
        typedef typename std::vector < std::shared_ptr < face_2t <Scalar> > >::iterator iterator;

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
                _edgs[i]->set_twin(_edgs[(i + 3) % 6]);
            }

            for(size_t i = 0; i < 3; ++i)
            {
                _edgs[i]->set_next(_edgs[(i + 1) % 3]);
                _edgs[i + 3]->set_next(_edgs[3 + (i + 2) % 3]);
                _edgs[i]->set_vertex(_vtxs[i]);
                _edgs[i + 3]->set_vertex(_vtxs[(i + 1) % 3]);
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
                std::cout << "Face: "
                          << *face->vertex(0) << " "
                          << *face->vertex(1) << " "
                          << *face->vertex(2) << " "
                          << *face->vertex(3) << " "
                          << *face->vertex(4) << " "
                          << *face->vertex(5) << std::endl;

                res.push_back(triangle_2t<Scalar>(
                                  *face->vertex(0),
                                  *face->vertex(1),
                                  *face->vertex(2)));

            }

            std::cout << std::endl;
            return res;
        }

        const_iterator begin() const
        {
            return _fcs.begin();
        }

        const_iterator end() const
        {
            return _fcs.end();
        }

        iterator begin()
        {
            return _fcs.begin();
        }

        iterator end()
        {
            return _fcs.end();
        }

        void add_vertex(const point_2t <Scalar> & v)
        {
            for(typename std::vector<face_handle <Scalar> >::iterator it = _fcs.begin(); it != _fcs.end(); ++it)
            {
                face_handle<Scalar> face = *it;

                if(contains(triangle_2t <Scalar>(*face->vertex(0), *face->vertex(1), *face->vertex(2)), v))
                {
                    std::iter_swap(it, _fcs.rbegin());
                    for(size_t i = 0; i < 3; ++i)
                    {
                        if(contains(segment_2t <Scalar>(*face->vertex(i), *face->vertex((i + 1) % 3)), v))
                        {
                            add_vertex_on_edge(v, face, i);
                            return;
                        }
                    }
                    add_vertex_on_face(v, face);
                    return;
                }
            }
        }

    protected:
        /* face should be last in _fcs */
        void add_vertex_on_face(const point_2t <Scalar> & v, const face_handle <Scalar> & face)
        {
            _vtxs.push_back(std::shared_ptr < vertex_2t <Scalar> >(new vertex_2t <Scalar>(v)));
            for(size_t i = 0; i < 2; ++i)
                _fcs.push_back(std::shared_ptr < face_2t <Scalar> >(new face_2t <Scalar>()));

            const size_t _edgs_sz = _edgs.size();
            for(size_t i = 0; i < 6; ++i)
                _edgs.push_back(std::shared_ptr <edge_2t <Scalar> >(new edge_2t <Scalar>()));

            _vtxs.back()->set_edge(_edgs.back());

            for(size_t i = 0; i < 6; ++i)
                _edgs[_edgs_sz + i]->set_twin(_edgs[_edgs_sz + (i + 3) % 6]);
            for(size_t i = 0; i < 3; ++i)
            {
                _edgs[_edgs_sz + i]->set_vertex(face->vertex(i));
                _edgs[_edgs_sz + 3 + i]->set_vertex(_vtxs.back());
                _edgs[_edgs_sz + i]->set_next(_edgs[_edgs_sz + 3 + (i + 2) % 3]);
                _edgs[_edgs_sz + 3 + i]->set_next(face->edge(i));
                face->edge(i)->set_face(_fcs[_fcs.size() - 3 + i]);
            }
            for(int i = 2; i >= 0; --i)
            {
                face->edge(i)->set_next(_edgs[_edgs_sz + (i + 1) % 3]);
            }

            for(size_t i = 0; i < 3; ++i)
            {
                _fcs[_fcs.size() - 1 - i]->set_edge(_edgs[_edgs.size() - 1 - i]);
                _edgs[_edgs_sz + i]->set_face(_fcs[_fcs.size() - 3 + (i + 2) % 3]);
                _edgs[_edgs_sz + 3 + i]->set_face(_fcs[_fcs.size() - 3 + i]);
            }
        }

        /* face should be last in _fcs */
        void add_vertex_on_edge(const point_2t <Scalar> & v, const face_handle <Scalar> & face,
                                size_t ind)
        {
            edge_handle <Scalar> cur_edge = face->edge(ind);
            edge_handle <Scalar> cur_twin = cur_edge->twin();
            cur_twin->prev()->set_next(cur_edge->next());
            cur_edge->prev()->set_next(cur_twin->next());

            _vtxs.push_back(std::shared_ptr < vertex_2t <Scalar> >(new vertex_2t <Scalar>(v)));
            for(size_t i = 0; i < 2; ++i)
                _fcs.push_back(std::shared_ptr < face_2t <Scalar> >(new face_2t <Scalar>()));

            const size_t _edgs_sz = _edgs.size();
            for(size_t i = 0; i < 6; ++i)
                _edgs.push_back(std::shared_ptr <edge_2t <Scalar> >(new edge_2t <Scalar>()));

            _vtxs.back()->set_edge(_edgs.back());

            cur_twin->set_vertex(_vtxs.back());
            cur_edge->set_next(_edgs[_edgs_sz + 5]);
            face->set_edge(cur_twin->next()->next());
            cur_twin->face()->set_edge(cur_twin);

            for(size_t i = 0; i < 6; ++i)
                _edgs[_edgs_sz + i]->set_twin(_edgs[_edgs_sz + (i + 3) % 6]);

            for(size_t i = 0; i < 3; ++i)
            {
                _edgs[_edgs_sz + i]->set_vertex(face->vertex(i));
                _edgs[_edgs_sz + 3 + i]->set_vertex(_vtxs.back());
                _edgs[_edgs_sz + 3 + i]->set_next(face->edge(i));
                face->edge(i)->set_face(_fcs[_fcs.size() - 1 - i]);
            }

            for(size_t i = 1; i < 3; ++i)
            {
                _edgs[_edgs_sz + i]->set_next(_edgs[_edgs_sz + 3 + (i + 2) % 3]);
            }
            _edgs[_edgs_sz]->set_next(cur_twin);

            face->edge(3)->set_next(_edgs[_edgs_sz]);
            face->edge(2)->set_next(cur_edge);
            for(int i = 1; i >= 0; --i)
            {
                face->edge(i)->set_next(_edgs[_edgs_sz + (i + 1) % 3]);
                _edgs[_edgs_sz + i + 1]->set_face(_fcs[_fcs.size() - 1 - i]);
            }
            _edgs[_edgs_sz]->set_face(cur_twin->face());

            for(size_t i = 0; i < 3; ++i)
            {
                _fcs[_fcs.size() - 1 - i]->set_edge(_edgs[_edgs.size() - 1 - i]);
                _edgs[_edgs_sz + 3 + i]->set_face(_fcs[_fcs.size() - 1 - i]);
            }
        }

        std::vector < vertex_handle <Scalar> > _vtxs;
        std::vector < edge_handle <Scalar> > _edgs;
        std::vector < face_handle <Scalar> > _fcs;
    };


    template <class Scalar>
    struct delaunay_triangulation : public cell_2t <Scalar>
    {
    public:

    protected:
        void flip(const face_handle & face, size_t index)
        {
            edge_handle <Scalar> cur_edge = face->edge(ind);
            edge_handle <Scalar> cur_twin = cur_edge->twin();
            edge_handle <Scalar> prev_edge = cur_edge->prev();
            edge_handle <Scalar> twin_prev = cur_twin->prev();
            face_handle <Scalar> twin_face = cur_twin->face();
            face->set_edge(cur_edge);
            twin_face->set_edge(cur_twin);
            twin_prev->set_next(cur_edge->next());
            prev_edge->set_next(cur_twin->next());
            cur_twin->set_vertex(twin_prev->vertex());
            cur_edge->set_vertex(prev_edge->vertex());
            cur_edge->next()->set_next(cur_edge);
            cur_twin->next()->set_next(cur_twin);
            cur_edge->set_next(twin_prev);
            cur_twin->set_next(prev_edge);
            cur_edge->next()->set_face(face);
            cur_edge->next()->next()->set_face(face);
            cur_twin->next()->set_face(twin_face);
            cur_twin->next()->next()->set_face(twin_face);
        }
    };
}
