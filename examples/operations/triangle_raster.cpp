#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include <cg/primitives/triangle.h>
#include <cg/primitives/point.h>
#include <cg/operations/triangle_raster.h>

using cg::point_2f;
using cg::point_2;


struct triangle_contains_point_viewer : cg::visualization::viewer_adapter
{
   triangle_contains_point_viewer()
      : t_(point_2f(0, 0), point_2f(100, 100), point_2f(200, 0))
      , rbutton_pressed_(false)
      , _show(false)
   {}

   void draw_cell(int x, int y, cg::visualization::drawer_type & drawer) const
   {
      point_2f a(x, y), b(x+1, y), c(x, y-1), d(x+1, y-1);
      drawer.draw_line(a, b);
      drawer.draw_line(a, c);
      drawer.draw_line(b, d);
      drawer.draw_line(c, d);
   }

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::white);
      for (size_t l = 0, lp = 2; l != 3; lp = l++)
         drawer.draw_line(t_[lp], t_[l]);

      if(_show)
      {
         drawer.set_color(Qt::green);
         for(auto pt : ranges)
         {
            for(int i = pt.first.x; i < pt.second.x; ++i)
               draw_cell(i, pt.first.y, drawer);
         }
      }
      if (idx_)
      {
         drawer.set_color((rbutton_pressed_)? Qt::red : Qt::yellow);
         drawer.draw_point(t_[*idx_], 5);
      }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "triangle: " << t_[0] << " " << t_[1] << " " << t_[2]
                        << cg::visualization::endl;
   }

   bool on_key(int key)
   {
      switch (key)
      {
      case Qt::Key_S : _show = !_show; break;
      default : return false;
      }

      return true;
   }

   bool on_press(const point_2f & p)
   {
     rbutton_pressed_ = true;
      return set_idx(p);
   }

   bool on_release(const point_2f & p)
   {
      rbutton_pressed_ = false;
      ranges = cg::triangle_raster(t_);
      return true;
   }

   bool on_move(const point_2f & p)
   {
      if (!rbutton_pressed_)
      {
         current_point_ = p;
         set_idx(p);
      }
      if (!idx_)
         return true;

      if (rbutton_pressed_) t_[*idx_] = p;
      return true;
   }

private:
   bool set_idx (const point_2f & p)
   {
      idx_.reset();
      float max_r;
      for (size_t l = 0; l != 3; ++l)
      {
         float current_r = (p.x - t_[l].x) * (p.x - t_[l].x) + (p.y - t_[l].y) * (p.y - t_[l].y);
         if ((idx_ && current_r < max_r) || (!idx_ && current_r < 100))
         {
            idx_ = l;
            max_r = current_r;
         }
      }
      return idx_;
   }

   cg::triangle_2 t_;
   boost::optional<size_t> idx_;
   boost::optional<cg::point_2f> current_point_;
   bool rbutton_pressed_, _show;
   std::vector< std::pair< cg::point_2i, cg::point_2i > > ranges;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   triangle_contains_point_viewer viewer;
   cg::visualization::run_viewer(&viewer, "triangle contains point");
}
