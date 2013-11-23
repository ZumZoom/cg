#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>
#include <cg/triangulation/delaunay_triangulation.h>

using cg::point_2f;
using cg::point_2;
using cg::triangle_2;

struct delaunay_viewer : cg::visualization::viewer_adapter
{
   delaunay_viewer()
      : mode(inserting), res(tr.get_triangulation())
   {}

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

      drawer.set_color(Qt::yellow);
      if(cur_pt)
         drawer.draw_point(*cur_pt, 10);
   }

   bool on_move(const point_2f& p)
   {
      set_cur_pt(p);
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

      default :
         return false;
      }

      return true;
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press mouse rbutton to add point" << cg::visualization::endl <<
                           "mode: " << (mode == inserting ? "inserting" : "deleting") << cg::visualization::endl;
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
      default:
         return false;
      }
      res = tr.get_triangulation();
      pts = tr.get_points();
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

   enum modes
   {
      inserting = 0,
      deleting = 1
   } mode;

   cg::delaunay_triangulation_2t<double> tr;
   std::vector<triangle_2> res;
   std::vector<point_2> pts;
   std::vector<triangle_2> failed;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   delaunay_viewer viewer;
   cg::visualization::run_viewer(&viewer, "delaunay_viewer");
}
