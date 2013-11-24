#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>
#include <cg/triangulation/delaunay_triangulation.h>

#include <cg/operations/contains/triangle_point.h>

using cg::point_2f;
using cg::point_2;
using cg::triangle_2;
using cg::segment_2;

struct delaunay_viewer : cg::visualization::viewer_adapter
{
   delaunay_viewer()
      : mode(inserting), res(tr.get_triangulation())
   {}

   double area(triangle_2 const & tr) const {
       double res = 0;
       for (int i = 0; i < 3; i++) {
           double dx = tr[i].x - tr[(i+1)%3].x;
           double dy = tr[i].y + tr[(i+1)%3].y;
           res += dx*dy;
       }
       return fabs(res) / 2.;
   }

   double distance(point_2 const & p1, point_2 const & p2) const {
       double dx = p1.x - p2.x;
       double dy = p1.y - p2.y;
       return sqrt(dx*dx+dy*dy);
   }

   point_2 intersect(segment_2 const & s1, segment_2 const & s2) const {
       double a1 = s1[0].y - s1[1].y;
       double b1 = -s1[0].x + s1[1].x;
       double c1 = -(a1 * s1[0].x + b1 * s1[0].y);
       double a2 = s2[0].y - s2[1].y;
       double b2 = -s2[0].x + s2[1].x;
       double c2 = -(a2 * s2[0].x + b2 * s2[0].y);
       double zn = a1*b2-a2*b1;
       double x = (c2*b1-c1*b2) / zn;
       double y = (c1*a2-c2*a1) / zn;
       return point_2(x, y);
   }

   void draw_circle2(cg::visualization::drawer_type & drawer, point_2 const & c, double r) const {
       double PI = asin(1) * 2;
       const int MAX_ITER = 360;
       for (int it = 0; it< MAX_ITER; it++) {
           double angle1 = PI *2 *it / MAX_ITER;
           double angle2 = PI *2 * (it+1) / MAX_ITER;
           double x1 = c.x + r * cos(angle1);
           double x2 = c.x + r * cos(angle2);
           double y1 = c.y + r * sin(angle1);
           double y2 = c.y + r * sin(angle2);
           drawer.draw_line(point_2(x1, y1), point_2(x2, y2));
       }
   }

   void draw_circle(cg::visualization::drawer_type & drawer, triangle_2 const & tr) const {
       double s = area(tr);
       segment_2 s1, s2;
       {
           point_2 ab((tr[0].x+tr[1].x)/2, (tr[0].y+tr[1].y)/2);
           double dx = ab.x - tr[0].x;
           double dy = ab.y - tr[0].y;
           point_2 p2(ab.x + dy, ab.y - dx);
           s1 = segment_2(ab, p2);
       }
       {
           point_2 ac((tr[0].x+tr[2].x)/2, (tr[0].y+tr[2].y)/2);
           double dx = ac.x - tr[0].x;
           double dy = ac.y - tr[0].y;
           point_2 p2(ac.x + dy, ac.y - dx);
           s2 = segment_2(ac, p2);
       }
       point_2 center = intersect(s1, s2);
       double a = distance(tr[0], tr[1]);
       double b = distance(tr[1], tr[2]);
       double c = distance(tr[2], tr[0]);
       double r = a *b * c / 4 / s;
       draw_circle2(drawer, center, r);
   }

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      for(point_2 p : pts)
         drawer.draw_point(p, 2);

      drawer.set_color(Qt::green);
      for(triangle_2 t : res)
      {
         drawer.draw_line(t[0], t[1]);
         drawer.draw_line(t[0], t[2]);
         drawer.draw_line(t[2], t[1]);
      }

      drawer.set_color(Qt::red);
      for(triangle_2 t : failed)
      {
         drawer.draw_line(t[0], t[1]);
         drawer.draw_line(t[0], t[2]);
         drawer.draw_line(t[2], t[1]);
      }

      drawer.set_color(Qt::white);
      for(segment_2 s : fxds)
         drawer.draw_line(s[0], s[1]);



      drawer.set_color(Qt::yellow);
      if(cur_pt)
         drawer.draw_point(*cur_pt, 10);
      if(fixed)
         drawer.draw_point(*fixed, 10);
      if(cur_triangle)
         draw_circle(drawer, *cur_triangle);
   }

   bool on_move(const point_2f& p)
   {
      set_cur_pt(p);
      set_cur_triangle(p);
      return true;
   }

   bool on_key(int key)
   {
      switch (key)
      {
      case Qt::Key_I :
         mode = inserting;
         break;

      case Qt::Key_D :
         mode = deleting;
         break;

      case Qt::Key_C :
         mode = constraint;
         fixed.reset();
         break;

      case Qt::Key_R :
         mode = remove_constraint;
         fixed.reset();
         break;

      default :
         return false;
      }

      return true;
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press mouse rbutton to add point" << cg::visualization::endl;
      switch(mode)
      {
      case inserting :
         p.corner_stream() << "mode: inserting" << cg::visualization::endl;
         break;
      case deleting :
         p.corner_stream() << "mode: deleting" << cg::visualization::endl;
         break;
      case constraint :
         p.corner_stream() << "mode: constraint" << cg::visualization::endl;
         break;
      case remove_constraint :
         p.corner_stream() << "mode: remove_constraint" << cg::visualization::endl;
         break;
      }
   }

   bool on_release(const point_2f & p)
   {
      switch(mode)
      {
      case inserting:
         tr.add_vertex(p);
         break;
      case deleting:
         if(set_cur_pt(p))
         {
            tr.del_vertex(*cur_pt);
            cur_pt.reset();
         }
         else
            return false;
         break;
      case constraint:
         if(set_cur_pt(p))
         {
            if(fixed && *fixed != *cur_pt)
            {
               tr.add_constraint(*cur_pt, *fixed);
               fixed.reset();
            }
            else
               fixed = *cur_pt;
         }
         else
            return false;
         break;
      case remove_constraint:
         if(set_cur_pt(p))
         {
            if(fixed && *fixed != *cur_pt)
            {
               tr.del_constraint(*cur_pt, *fixed);
               fixed.reset();
            }
            else
               fixed = *cur_pt;
         }
         else
            return false;
         break;
      default:
         return false;
      }
      res = tr.get_triangulation();
      pts = tr.get_points();
      fxds = tr.get_constraints();
      check_edges();
      return true;
   }

   void check_edges()
   {
      failed.clear();
      for(triangle_2 t : res)
         for(point_2 s : pts)
            if(t[0] != s && t[1] != s && t[2] != s && cg::contains(t[0], t[1], t[2], s))
               failed.push_back(t);
   }

private:

   bool set_cur_triangle(const point_2f & p)
   {
      cur_triangle.reset();
      for(auto tr : res)
      {
         if(cg::contains<double>(tr, p))
         {
            cur_triangle = tr;
            break;
         }
      }
      return cur_triangle;
   }

   bool set_cur_pt (const point_2f& p)
   {
      cur_pt.reset();
      float max_r;
      for (auto pt : pts)
      {
         float current_r = (p.x - pt.x) * (p.x - pt.x) + (p.y - pt.y) * (p.y - pt.y);
         if ((cur_pt && current_r < max_r) || (!cur_pt && current_r < 30))
         {
            cur_pt = pt;
            max_r = current_r;
         }
      }
      return cur_pt;
   }

   boost::optional<point_2> cur_pt;
   boost::optional<point_2> fixed;
   boost::optional<triangle_2> cur_triangle;

   enum modes
   {
      inserting = 0,
      deleting = 1,
      constraint = 2,
      remove_constraint = 3
   } mode;

   cg::delaunay_triangulation_2t<double> tr;
   std::vector<triangle_2> res;
   std::vector<point_2> pts;
   std::vector<triangle_2> failed;
   std::vector<segment_2> fxds;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   delaunay_viewer viewer;
   cg::visualization::run_viewer(&viewer, "delaunay_viewer");
}
