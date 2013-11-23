﻿#pragma once

#include <vector>
#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>
#include <cg/operations/orientation.h>

namespace cg
{
    template <class Scalar> struct face_2t;
    template <class Scalar> struct edge_2t;
    template <class Scalar> struct vertex_2t;

    template <class Scalar>
    using vertex_handle = std::shared_ptr < vertex_2t <Scalar> >;
    template <class Scalar>
    using face_handle = std::shared_ptr < face_2t <Scalar> >;
    template <class Scalar>
    using edge_handle = std::shared_ptr < edge_2t <Scalar> >;

    template<class Scalar>
    bool contains(const std::vector< vertex_handle <Scalar> > & t, point_2t <Scalar> const & q)
    {
       for (size_t l = 0, lp = 2; l != 3; lp = l++)
           if (!t[lp]->inf() && !t[l]->inf() && orientation(*t[lp], *t[l], q) == CG_RIGHT)
              return false;
       return true;
    }

    template<class Scalar>
    Scalar contains_impl(const point_2t<Scalar> & a, const point_2t<Scalar> & b,
                         const point_2t<Scalar> & c, const point_2t<Scalar> & d)
    {
        Scalar a11 = a.x - d.x, a12 = a.y - d.y, a13 = (a.x * a.x - d.x * d.x) + (a.y * a.y - d.y * d.y);
        Scalar a21 = b.x - d.x, a22 = b.y - d.y, a23 = (b.x * b.x - d.x * d.x) + (b.y * b.y - d.y * d.y);
        Scalar a31 = c.x - d.x, a32 = c.y - d.y, a33 = (c.x * c.x - d.x * d.x) + (c.y * c.y - d.y * d.y);
        return a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31);
    }

    template<class Scalar>
    bool contains(vertex_handle<Scalar> a, vertex_handle<Scalar> b,
                  vertex_handle<Scalar> c, vertex_handle<Scalar> d)
    {
       if(a->inf())
           return orientation(*b, *c, *d) == CG_LEFT;
       else if(b->inf())
           return orientation(*c, *a, *d) == CG_LEFT;
       else if(c->inf())
           return orientation(*a, *b, *d) == CG_LEFT;
       else if(d->inf())
           return false;

//       double a11 = a->x - d->x, a12 = a->y - d->y, a13 = (a->x * a->x - d->x * d->x) + (a->y * a->y - d->y * d->y);
//       double a21 = b->x - d->x, a22 = b->y - d->y, a23 = (b->x * b->x - d->x * d->x) + (b->y * b->y - d->y * d->y);
//       double a31 = c->x - d->x, a32 = c->y - d->y, a33 = (c->x * c->x - d->x * d->x) + (c->y * c->y - d->y * d->y);
//       return a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31) > 0;

       return contains_impl(*a, *b, *c, *d) > 0;
    }

    template <class Scalar>
    struct vertex_2t : public point_2t<Scalar>
    {
        vertex_2t(bool inf = false) : _inf(inf) {}

        vertex_2t(const point_2t<Scalar> & p, bool inf = false)
            : point_2t<Scalar>(p), _inf(inf)
        {}

        void set_edge(const edge_handle<Scalar> & e)
        {
            _e = e;
        }

        void clear_edge()
        {
            _e.reset();
        }

        face_handle<Scalar> face(size_t ind = 0) const
        {
            return edge(ind)->face();
        }

        edge_handle<Scalar> edge(size_t ind = 0) const
        {
            edge_handle<Scalar> edge = _e;
            for(size_t i = 0; i < ind; ++i)
                edge = edge->neighbour();
            return edge;
        }

        bool inf() const
        {
            return _inf;
        }

    private:
        edge_handle<Scalar> _e;
        bool _inf;
    };

    template <class Scalar>
    struct face_2t
    {
        face_2t() {}

        face_2t(const edge_handle<Scalar> & e) : _e(e) {}

        vertex_handle<Scalar> vertex(size_t index = 0) const
        {
            return edge(index)->vertex();
        }

        edge_handle<Scalar> edge(size_t index = 0) const
        {
            edge_handle<Scalar> e = _e;
            for(size_t i = 0; i < index; ++i)
                e = e->next();
            return e;
        }

        bool inf() const
        {
            return vertex(0)->inf() || vertex(1)->inf() || vertex(2)->inf();
        }

        void set_edge(const edge_handle<Scalar> & e)
        {
            _e = e;
        }

        void clear_edge()
        {
            _e.reset();
        }

    private:
        edge_handle<Scalar> _e;
    };

    template <class Scalar>
    struct edge_2t
    {
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

        edge_handle<Scalar> neighbour() const
        {
            return _twin->next();
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

        void clear_twin()
        {
            _twin.reset();
        }

        void clear_next()
        {
            _next.reset();
        }

        void clear_face()
        {
            _f.reset();
        }

        void clear_vertex()
        {
            _v.reset();
        }

    private:
        vertex_handle<Scalar> _v;
        face_handle<Scalar> _f;
        edge_handle<Scalar> _twin, _next;
    };

    template <class Scalar>
    std::ostream & operator<<(std::ostream & os, const face_handle<Scalar> & face)
    {
        os << "(" << face->vertex(0) << ", " << face->vertex(1) << ", " << face->vertex(2) << ")\n" <<
              "edge0: " << face->edge(0) << ", twin " << face->edge(0)->twin() <<
              ", face vertex " << face->edge(0)->face()->vertex() << ", next " <<
              face->edge(0)->next() << "\n"<<
              "edge1: " << face->edge(1) << ", twin " << face->edge(1)->twin() <<
              ", face vertex " << face->edge(1)->face()->vertex() << ", next " <<
              face->edge(1)->next() << "\n"<<
              "edge2: " << face->edge(2) << ", twin " << face->edge(2)->twin() <<
              ", face vertex " << face->edge(2)->face()->vertex() << ", next " <<
              face->edge(2)->next() << "\n";
        return os;
    }

    template <class Scalar>
    std::ostream & operator<<(std::ostream & os, const edge_handle<Scalar> & edge)
    {
        os << "(" << *edge->vertex() << ", " << *edge->next()->vertex() << ")";
        return os;
    }

    template <class Scalar>
    std::ostream & operator<<(std::ostream & os, const vertex_handle<Scalar> & vertex)
    {
        os << "(" << *vertex << ")";
        return os;
    }

    template <class Scalar>
    struct delaunay_triangulation_2t
    {
        delaunay_triangulation_2t()
        {
            _vtxs.push_back(std::shared_ptr <vertex_2t <Scalar> >(new vertex_2t <Scalar>(true)));
        }

        std::vector < point_2t < Scalar > > get_points()
        {
            std::vector < point_2t < Scalar > >res;
            for(auto pt : _vtxs)
                if(!pt->inf())
                    res.push_back(*pt);
            return res;
        }

        std::vector < triangle_2t <Scalar> > get_triangulation()
        {
            std::vector < triangle_2t <Scalar> > res;
            for(auto face : _fcs)
            {
                if(face->inf())
                    continue;
                res.push_back(triangle_2t<Scalar>(*face->vertex(0),
                                                  *face->vertex(1),
                                                  *face->vertex(2)));
            }
            return res;
        }

        void add_vertex(const point_2t <Scalar> & v)
        {
            if(_vtxs.size() == 1)
            {
                _vtxs.push_back(vertex_handle<Scalar>(new vertex_2t <Scalar>(v)));
                return;
            }
            else if(_vtxs.size() == 2)
            {
                _vtxs.push_back(vertex_handle<Scalar>(new vertex_2t <Scalar>(v)));
                for(size_t i = 0; i < 6; ++i)
                    _edgs.push_back(edge_handle<Scalar>(new edge_2t <Scalar>()));
                for(size_t i = 0; i < 2; ++i)
                    _fcs.push_back(face_handle<Scalar>(new face_2t <Scalar>(_edgs[i * 3])));
                for(size_t i = 0; i < 6; ++i)
                {
                    _edgs[i]->set_twin(_edgs[(i + 3) % 6]);
                    _edgs[i]->set_face(_fcs[i / 3]);
                }
                for(size_t i = 0; i < 3; ++i)
                {
                    _edgs[i]->set_vertex(_vtxs[i]);
                    _edgs[i + 3]->set_vertex(_vtxs[(i + 1) % 3]);
                    _edgs[i]->set_next(_edgs[(i + 1) % 3]);
                    _edgs[i + 3]->set_next(_edgs[3 + (i + 2) % 3]);
                }
                for(size_t i = 0; i < 3; ++i)
                    _vtxs[i]->set_edge(_edgs[i]);
                return;
            }

            for(typename std::vector<face_handle <Scalar> >::iterator it = _fcs.begin(); it != _fcs.end(); ++it)
            {
                face_handle<Scalar> face = *it;
                if(contains<Scalar>({{face->vertex(0), face->vertex(1), face->vertex(2)}}, v))
                {
                    std::iter_swap(it, _fcs.rbegin());
                    add_vertex_on_face(v, face);
                    edge_handle <Scalar> edge = _vtxs.back()->edge();
                    for(size_t i = 0; i < 3; ++i)
                    {
                        fix_correctness(edge->next());
                        edge = edge->neighbour();
                    }
                    return;
                }
            }
        }

        void del_vertex(const point_2t<Scalar> & p)
        {
            if(_vtxs.size() <= 3)
            {
                for(typename std::vector < vertex_handle <Scalar> >::const_iterator it = _vtxs.begin(); it != _vtxs.end();)
                    if(**it == p)
                        it = _vtxs.erase(it);
                    else
                        ++it;
                _edgs.clear();
                _fcs.clear();
                return;
            }

            for(typename std::vector < vertex_handle <Scalar> >::const_iterator it = _vtxs.begin(); it != _vtxs.end(); ++it)
            {
                auto vtx = *it;
                if(*vtx == p)
                {
                    std::vector< edge_handle <Scalar> > flipped;
                    edge_handle<Scalar> edge = vtx->edge();
                    size_t count = 1;
                    while(edge->neighbour() != vtx->edge())
                    {
                        ++count;
                        edge = edge->neighbour();
                    }

                    for(size_t i = 3; i < count; ++i)
                    {
                        edge_handle<Scalar> to_flip = edge;
                        edge = edge->neighbour();
                        flip(to_flip);
                        flipped.push_back(to_flip);
                    }

                    _vtxs.erase(it);

                    face_handle<Scalar> face = vtx->face();
                    edge = vtx->edge()->next();
                    for(size_t i = 0; i < 3; ++i)
                    {
                        vtx->edge(i)->next()->set_next(vtx->edge(i)->prev()->neighbour());
                        vtx->edge(i)->next()->vertex()->set_edge(vtx->edge(i)->next());
                        vtx->edge(i)->next()->face()->clear_edge();
                        if(vtx->edge(i)->face() != face)
                        {
                            auto iter = std::find(_fcs.begin(), _fcs.end(), vtx->edge(i)->face());
                            _fcs.erase(iter);
                        }
                        vtx->edge(i)->next()->set_face(face);
                    }

                    face->set_edge(edge);

                    edge_handle<Scalar> edgs[6];
                    for(size_t i = 0; i < 3; ++i)
                    {
                        edgs[2 * i] = vtx->edge(i);
                        edgs[2 * i + 1] = vtx->edge(i)->twin();
                    }

                    for(size_t i = 0; i < 6; ++i)
                    {
                        edgs[i]->clear_face();
                        edgs[i]->clear_next();
                        edgs[i]->clear_twin();
                        edgs[i]->clear_vertex();
                        auto iter = std::find(_edgs.begin(), _edgs.end(), edgs[i]);
                        _edgs.erase(iter);
                    }

                    for(auto edge : flipped)
                        flip(edge);

                    for(auto edge : flipped)
                        fix_correctness(edge);

                    return;
                }
            }
        }

    protected:
        /* face should be last in _fcs */
        void add_vertex_on_face(const point_2t <Scalar> & v, const face_handle <Scalar> & face)
        {
            _vtxs.push_back(vertex_handle<Scalar>(new vertex_2t <Scalar>(v)));
            for(size_t i = 0; i < 2; ++i)
                _fcs.push_back(face_handle<Scalar>(new face_2t <Scalar>()));
            const size_t _edgs_sz = _edgs.size();
            for(size_t i = 0; i < 6; ++i)
                _edgs.push_back(edge_handle<Scalar>(new edge_2t <Scalar>()));

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

        void fix_correctness(const edge_handle <Scalar> & e0)
        {
            edge_handle <Scalar> e1 = e0->next(), e2 = e1->next();
            vertex_handle<Scalar> v0 = e0->vertex(), v1 = e1->vertex(), v2 = e2->vertex();
            vertex_handle<Scalar> v = e0->twin()->prev()->vertex();

            if(contains(v0, v1, v2, v) || contains(v1, v0, v, v2))
            {
                flip(e0);
                fix_correctness(e0->next());
                fix_correctness(e0->prev());
                fix_correctness(e0->twin()->next());
                fix_correctness(e0->twin()->prev());
            }
        }

        void flip(const edge_handle <Scalar> & cur_edge)
        {
            face_handle <Scalar> face = cur_edge->face();
            edge_handle <Scalar> cur_twin = cur_edge->twin();
            edge_handle <Scalar> prev_edge = cur_edge->prev();
            edge_handle <Scalar> twin_prev = cur_twin->prev();
            face_handle <Scalar> twin_face = cur_twin->face();
            face->set_edge(cur_edge);
            twin_face->set_edge(cur_twin);
            twin_prev->set_next(cur_edge->next());
            prev_edge->set_next(cur_twin->next());
            cur_edge->vertex()->set_edge(cur_twin->next());
            cur_twin->vertex()->set_edge(cur_edge->next());
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

        std::vector < vertex_handle <Scalar> > _vtxs;
        std::vector < edge_handle <Scalar> > _edgs;
        std::vector < face_handle <Scalar> > _fcs;
    };
}
