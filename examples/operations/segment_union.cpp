#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/primitives/segment.h>
#include <cg/primitives/point.h>

#include <cg/operations/segment_union.h>

using cg::point_2;
using cg::point_2f;

struct segments_intersect_viewer : cg::visualization::viewer_adapter
{
   segments_intersect_viewer()
      : rbutton_pressed_(false)
      , show_union(false)
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      if(show_union)
      {
         std::vector< cg::segment_2f > u = cg::segment_union(segm);
         drawer.set_color(Qt::green);
         for (auto s : u)
            drawer.draw_line(s[0], s[1]);
         drawer.set_color(Qt::red);
         for (auto s : u)
         {
            drawer.draw_point(s[0], 10);
            drawer.draw_point(s[1], 10);
         }
      }
      else
      {
         drawer.set_color(Qt::white);
         for (auto s : segm)
            drawer.draw_line(s[0], s[1]);
         drawer.set_color(Qt::red);
         for (auto s : segm)
         {
            drawer.draw_point(s[0], 10);
            drawer.draw_point(s[1], 10);
         }
      }

      if(current_point_)
      {
         drawer.set_color(Qt::yellow);
         drawer.draw_point(current_point_.get(), 10);
      }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "press mouse rbutton near segment vertex to move it"
                        << cg::visualization::endl
                        << "if segments are green they intersect"
                        << cg::visualization::endl;
   }

   bool on_press(const point_2f & p)
   {
      rbutton_pressed_ = true;
      return set_idx(p);
   }

   bool on_release(const point_2f & p)
   {
      rbutton_pressed_ = false;
      return false;
   }

   bool on_double_click(const point_2f & p)
   {
      segm.clear();
      current_point_.reset();
      return true;
   }

   bool on_key(int key)
   {
      switch (key)
      {
      case Qt::Key_S :
         show_union = !show_union;
         break;
      default :
         return false;
      }

      return true;
   }

private:
   bool set_idx (point_2f p)
   {
      p.y = 0;
      if(current_point_)
      {
         segm.push_back(cg::segment_2f(current_point_.get(), p));
         current_point_.reset();
      }
      else
         current_point_ = p;
      return true;
   }

   boost::optional<point_2f> current_point_;
   std::vector< cg::segment_2f > segm;

   bool rbutton_pressed_;
   bool show_union;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   segments_intersect_viewer viewer;
   cg::visualization::run_viewer(&viewer, "segments intersect");
}

