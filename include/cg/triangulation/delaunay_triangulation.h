#pragma once

#include <vector>
#include <cg/primitives/point.h>
#include <cg/primitives/triangle.h>
#include <cg/operations/contains/segment_point.h>
#include <cg/operations/has_intersection/segment_segment.h>
#include <cg/operations/orientation.h>
#include <set>

#include <cg/io/point.h>

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

template <class Scalar>
using vertex_weak_handle = std::weak_ptr < vertex_2t <Scalar> >;
template <class Scalar>
using face_weak_handle = std::weak_ptr < face_2t <Scalar> >;
template <class Scalar>
using edge_weak_handle = std::weak_ptr < edge_2t <Scalar> >;

template<class Scalar>
bool contains(const std::vector< vertex_handle <Scalar> > & t, point_2t <Scalar> const & q)
{
   for (size_t l = 0, lp = 2; l != 3; lp = l++)
      if (!t[lp]->inf() && !t[l]->inf() && orientation(*t[lp], *t[l], q) != CG_LEFT)
         return false;
   return true;
}

bool contains_r(const point_2 & a, const point_2 & b, const point_2 & c, const point_2 & d)
{
   mpq_class a11 = mpq_class(a.x) - d.x, a12 = mpq_class(a.y) - d.y,
            a13 = (mpq_class(a.x) * a.x - mpq_class(d.x) * d.x) + (mpq_class(a.y) * a.y - mpq_class(d.y) * d.y),
            a21 = mpq_class(b.x) - d.x, a22 = mpq_class(b.y) - d.y,
            a23 = (mpq_class(b.x) * b.x - mpq_class(d.x) * d.x) + (mpq_class(b.y) * b.y - mpq_class(d.y) * d.y),
            a31 = mpq_class(c.x) - d.x, a32 = mpq_class(c.y) - d.y,
            a33 = (mpq_class(c.x) * c.x - mpq_class(d.x) * d.x) + (mpq_class(c.y) * c.y - mpq_class(d.y) * d.y);

   mpq_class res = a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31);

   int cres = cmp(res, 0);

   if(cres > 0)
      return true;
   else
      return false;
}

bool contains(const point_2 & a, const point_2 & b, const point_2 & c, const point_2 & d)
{
   typedef boost::numeric::interval_lib::unprotect<boost::numeric::interval<double> >::type interval;
   boost::numeric::interval<double>::traits_type::rounding _;

   interval a11 = interval(a.x) - d.x, a12 = interval(a.y) - d.y,
            a13 = (interval(a.x) * a.x - interval(d.x) * d.x) + (interval(a.y) * a.y - interval(d.y) * d.y),
            a21 = interval(b.x) - d.x, a22 = interval(b.y) - d.y,
            a23 = (interval(b.x) * b.x - interval(d.x) * d.x) + (interval(b.y) * b.y - interval(d.y) * d.y),
            a31 = interval(c.x) - d.x, a32 = interval(c.y) - d.y,
            a33 = (interval(c.x) * c.x - interval(d.x) * d.x) + (interval(c.y) * c.y - interval(d.y) * d.y);

   interval res = a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31);

   if (res.lower() > 0)
      return true;

   if (res.upper() < 0)
      return false;

   if (res.upper() == res.lower())
      return false;

   return contains_r(a, b, c, d);
}

template<class Scalar>
bool contains(vertex_handle<Scalar> a, vertex_handle<Scalar> b,
              vertex_handle<Scalar> c, vertex_handle<Scalar> d)
{
   size_t inf = a->inf() + b->inf() + c->inf() + d->inf();
   if(inf > 1)
      return false;

   if(d->inf())
      return orientation(*a, *b, *c) == CG_COLLINEAR;
   vertex_handle<Scalar> vtx_[3] = {a, b, c};
   for(size_t i = 0; i < 3; ++i)
      if(vtx_[i]->inf())
         return orientation(*vtx_[(i+1) % 3], *vtx_[(i+2) % 3], *d) == CG_LEFT;

   return contains(*a, *b, *c, *d);
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

   void clear()
   {
      _e.reset();
   }

   face_handle<Scalar> face(size_t ind = 0) const
   {
      return edge(ind)->face();
   }

   edge_handle<Scalar> edge(size_t ind = 0) const
   {
      edge_handle<Scalar> edge = _e.lock();
      for(size_t i = 0; i < ind; ++i)
         edge = edge->neighbour();
      return edge;
   }

   bool inf() const
   {
      return _inf;
   }

private:
   edge_weak_handle<Scalar> _e;
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
      edge_handle<Scalar> e = _e.lock();
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

private:
   edge_weak_handle<Scalar> _e;
};

template <class Scalar>
struct edge_2t
{
   vertex_handle<Scalar> vertex() const
   {
      return _v.lock();
   }

   face_handle<Scalar> face() const
   {
      return _f.lock();
   }

   edge_handle<Scalar> twin() const
   {
      return _twin.lock();
   }

   edge_handle<Scalar> next() const
   {
      return _next.lock();
   }

   edge_handle<Scalar> prev() const
   {
      return _next.lock()->next();
   }

   edge_handle<Scalar> neighbour() const
   {
      return _twin.lock()->next();
   }

   bool constraint()
   {
      return _fixed;
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

   void set_constraint(bool fixed)
   {
      _fixed = fixed;
   }

private:
   vertex_weak_handle<Scalar> _v;
   face_weak_handle<Scalar> _f;
   edge_weak_handle<Scalar> _twin, _next;
   bool _fixed;
};

template <class Scalar>
std::ostream & operator<<(std::ostream & os, const face_handle<Scalar> & face)
{
   os << "(" << face->vertex(0) << ", " << face->vertex(1) << ", " << face->vertex(2);
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
      std::cout << "triang" << std::endl;
      for(auto face : _fcs)
      {
         std::cout << face << std::endl;
         if(face->inf())
            continue;
         res.push_back(triangle_2t<Scalar>(*face->vertex(0),
                                           *face->vertex(1),
                                           *face->vertex(2)));
      }

      std::cout << std::endl;

      return res;
   }

   std::vector < segment_2t <Scalar> > get_constraints()
   {
      std::vector < segment_2t <Scalar> > res;
      for(auto edge : _edgs)
      {
         if(edge->constraint())
            res.push_back(segment_2t<Scalar>(*edge->vertex(), *edge->next()->vertex()));
      }
      return res;
   }

   void add_vertex(const point_2t <Scalar> & v)
   {
      for(typename std::vector < vertex_handle <Scalar> >::const_iterator it = _vtxs.begin(); it != _vtxs.end(); ++it)
         if(**it == v)
            return;

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

      face_handle<Scalar> face;
      for(typename std::vector<face_handle <Scalar> >::iterator it = _fcs.begin(); it != _fcs.end(); ++it)
      {
         face = *it;
         if(contains<Scalar>({{face->vertex(0), face->vertex(1), face->vertex(2)}}, v))
         {
            std::cout << "adding on " << face << std::endl;
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
      face = get_face_on_collinear(v);
      assert( face.use_count() );

      add_vertex_on_face(v, face);
      edge_handle <Scalar> edge0 = _vtxs.back()->edge();
      if(edge0->face()->inf())
         edge0 = edge0->neighbour();
      edge_handle <Scalar> edge1 = edge0->neighbour();
      if(edge1->face()->inf())
         edge1 = edge1->neighbour();

      std::cout << "choose to fix" << edge0 << " " << edge1 << std::endl;

      if((*edge0->next()->vertex() > *edge1->next()->vertex() && *edge1->next()->vertex() > v) ||
         (*edge0->next()->vertex() < *edge1->next()->vertex() && *edge1->next()->vertex() < v))
         fix_correctness(edge0);
      else if((*edge1->next()->vertex() > *edge0->next()->vertex() && *edge0->next()->vertex() > v) ||
              (*edge1->next()->vertex() < *edge0->next()->vertex() && *edge0->next()->vertex() < v))
         fix_correctness(edge1);
      else
      {
         for(size_t i = 0; i < 3; ++i)
         {
            fix_correctness(edge0->next());
            edge0 = edge0->neighbour();
         }
      }
   }

   void del_vertex(const point_2t<Scalar> & p)
   {
      for(auto edge : _edgs)
         if(edge->constraint() && *edge->vertex() == p)
            return;

      if(_vtxs.size() <= 3)
      {
         for(typename std::vector < vertex_handle <Scalar> >::const_iterator it = _vtxs.begin(); it != _vtxs.end(); ++it)
         {
            if(**it == p)
            {
               _vtxs.erase(it);
               _edgs.clear();
               _fcs.clear();
               break;
            }
         }
         return;
      }

      for(typename std::vector < vertex_handle <Scalar> >::const_iterator it = _vtxs.begin(); it != _vtxs.end(); ++it)
      {
         auto vtx = *it;
         if(*vtx == p)
         {
            std::cout << "deleting " << vtx << std::endl;

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
               std::cout << "deleting" << edgs[i] << std::endl;
            std::cout << std::endl;

            for(size_t i = 0; i < 6; ++i)
            {
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

   void add_constraint(const point_2t <Scalar> & p1, const point_2t <Scalar> & p2)
   {
      vertex_handle<Scalar> vtx1, vtx2;
      for(typename std::vector < vertex_handle <Scalar> >::const_iterator it = _vtxs.begin(); it != _vtxs.end(); ++it)
      {
         if(**it == p1)
            vtx1 = *it;
         if(**it == p2)
            vtx2 = *it;
      }
      if(!vtx1)
      {
         add_vertex(p1);
         vtx1 = _vtxs.back();
      }
      if(!vtx2)
      {
         add_vertex(p2);
         vtx2 = _vtxs.back();
      }

      segment_2t<Scalar> new_segment(*vtx1, *vtx2);
      for(auto vertex : _vtxs)
         if(!vertex->inf() && vertex != vtx1 && vertex != vtx2 && contains(new_segment, *vertex))
            return;
      for(auto edge : _edgs)
         if(edge->constraint() &&
            edge->vertex() != vtx1 && edge->twin()->vertex() != vtx1 &&
            edge->vertex() != vtx2 && edge->twin()->vertex() != vtx2 &&
            has_intersection(new_segment, segment_2t<Scalar>(*edge->vertex(),*edge->next()->vertex())))
            return;

      std::set< edge_handle <Scalar> > to_check;

      while(true)
      {
         edge_handle<Scalar> edge;
         for(size_t i = 0;; ++i)
         {
            edge = vtx1->edge(i);
            if(!edge->face()->inf() && has_intersection(new_segment, segment_2t<Scalar>(*edge->next()->vertex(), *edge->prev()->vertex())))
               break;
         }
         if(edge->next()->vertex() == vtx2)
         {
            edge->twin()->set_constraint(true);
            edge->set_constraint(true);
            break;
         }
         if(edge->prev()->vertex() == vtx2)
         {
            edge = edge->prev();
            edge->twin()->set_constraint(true);
            edge->set_constraint(true);
            break;
         }

         edge = edge->next();

         while(edge->vertex() != vtx2)
         {
            if(can_flip(edge))
            {
               to_check.insert(edge);
               flip(edge);
               if(has_intersection(new_segment, segment_2t<Scalar>(*edge->next()->vertex(), *edge->prev()->vertex())))
                  edge = edge->next();
               else
                  edge = edge->twin()->prev();
            }
            else
            {
               if(has_intersection(new_segment, segment_2t<Scalar>(*edge->vertex(), *edge->twin()->prev()->vertex())))
                  edge = edge->twin()->next();
               else
                  edge = edge->twin()->prev();
            }
         }

         std::swap(vtx1, vtx2);
      }

      for(auto edge : to_check)
         fix_correctness(edge);
   }

   void del_constraint(const point_2t <Scalar> & p1, const point_2t <Scalar> & p2)
   {
      vertex_handle<Scalar> vtx1, vtx2;
      for(typename std::vector < vertex_handle <Scalar> >::const_iterator it = _vtxs.begin(); it != _vtxs.end(); ++it)
      {
         if(**it == p1)
            vtx1 = *it;
         if(**it == p2)
            vtx2 = *it;
      }
      if(!vtx1 || !vtx2)
         return;

      edge_handle<Scalar> edge = vtx1->edge(), edge_begin = vtx1->edge();
      while(edge->next()->vertex() != vtx2 && edge->neighbour() != edge_begin)
         edge = edge->neighbour();

      edge->twin()->set_constraint(false);
      edge->set_constraint(false);

      fix_correctness(edge);
   }

protected:
   edge_handle<Scalar> get_next_collinear(const edge_handle<Scalar> & edge)
   {
      edge_handle<Scalar> fin = edge->twin(), cur = edge->next();
      while(cur != fin && orientation(*edge->vertex(), *edge->next()->vertex(), *cur->next()->vertex()) != CG_COLLINEAR)
            cur = cur->neighbour();
      return cur;
   }

   face_handle<Scalar> get_face_on_collinear(const point_2t <Scalar> & v)
   {
      for(typename std::vector<edge_handle <Scalar> >::iterator it = _edgs.begin(); it != _edgs.end(); ++it)
      {
         edge_handle<Scalar> edge = *it;
         if(orientation(*edge->vertex(), *edge->next()->vertex(), v) == CG_COLLINEAR)
         {
            while((*edge->vertex() < *edge->next()->vertex() && *edge->next()->vertex() < v) ||
                  (*edge->vertex() > *edge->next()->vertex() && *edge->next()->vertex() > v))
               edge = get_next_collinear(edge);

            edge = edge->twin();

            while((*edge->vertex() < *edge->next()->vertex() && *edge->next()->vertex() < v) ||
                  (*edge->vertex() > *edge->next()->vertex() && *edge->next()->vertex() > v))
               edge = get_next_collinear(edge);

            std::cout << "Got collinear closest " << edge << std::endl;

            typename std::vector<face_handle <Scalar> >::iterator it =
                  std::find(_fcs.begin(), _fcs.end(), edge->face());
            assert( it != _fcs.end() );
            std::iter_swap(it, _fcs.rbegin());
            return _fcs.back();
         }
      }

      return face_handle<Scalar>();
   }

   bool can_flip(const edge_handle<Scalar> & edge)
   {
      vertex_handle<Scalar> v0 = edge->vertex(), v1 = edge->next()->vertex(),
            v2 = edge->prev()->vertex(), v3 = edge->twin()->prev()->vertex();
      return orientation(*v2, *v3, *v0) != orientation(*v2, *v3, *v1);
   }

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
      std::cout << "fixing " << e0 << std::endl;

      if(e0->constraint())
         return;
      edge_handle<Scalar> e1 = e0->next(), e2 = e1->next();
      vertex_handle<Scalar> v0 = e0->vertex(), v1 = e1->vertex(), v2 = e2->vertex();
      vertex_handle<Scalar> v = e0->twin()->prev()->vertex();

      if(contains(v0, v1, v2, v))
      {
         std::cout << v0 << v1 << v2 << " contains " << v << std::endl;
         flip(e0);
         fix_correctness(e0->next());
         fix_correctness(e0->prev());
         fix_correctness(e0->twin()->next());
         fix_correctness(e0->twin()->prev());
      }
      else if(contains(v1, v0, v, v2))
      {
         std::cout << v1 << v0 << v << " contains " << v2 << std::endl;
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
