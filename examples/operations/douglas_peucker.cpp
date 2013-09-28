#include <QColor>
#include <QApplication>

#include <boost/optional.hpp>

#include "cg/visualization/viewer_adapter.h"
#include "cg/visualization/draw_util.h"

#include "cg/operations/douglas_peucker.h"
#include "cg/io/point.h"

#include <cg/primitives/triangle.h>
#include <cg/primitives/point.h>

#include <cg/operations/contains/contour_point.h>
#include <vector>

using cg::point_2f;
using cg::point_2;


struct contour_contains_point_viewer : cg::visualization::viewer_adapter
{
   contour_contains_point_viewer()
      : chain(std::vector<point_2>({point_2(0, 0), point_2(20, 20), point_2(40, 40)}))
      , eps(0.5)
      , rbutton_pressed_(false)
   {}

   void draw(cg::visualization::drawer_type & drawer) const override
   {
      drawer.set_color(Qt::white);
      drawer.draw_point(*chain.begin(), 5);
      for(auto i = chain.begin() + 1; i != chain.end(); ++i)
      {
         drawer.draw_point(*i, 5);
         drawer.draw_line(*i, *(i-1));
      }
      
      std::vector<point_2> simple;
      douglas_peucker(chain.begin(), chain.end(), eps, back_inserter(simple));

      drawer.set_color(Qt::green);
      drawer.draw_point(*simple.begin(), 5);
      for(auto i = simple.begin() + 1; i != simple.end(); ++i)
      {
         drawer.draw_point(*i, 5);
         drawer.draw_line(*i, *(i-1));
      }
   }

   void print(cg::visualization::printer_type & p) const override
   {
      p.corner_stream() << "eps: " << eps * 10
                        << cg::visualization::endl
                        << "size: " << chain.size()
                        << cg::visualization::endl;
   }

   bool on_double_click(const point_2f & p)
   {
      chain.clear();
      chain = {point_2(0, 0), point_2(20, 20), point_2(40, 40)};
      current_vertex_.reset();
      return true;
   }

   bool on_press(const point_2f & p)
   {
      for (size_t i = 0; i < chain.size(); ++i)
      {
         if (fabs(chain[i].x - p.x) < 15 && fabs(chain[i].y - p.y) < 15)
         {
            current_vertex_ = i;
            return true;
         }
      }

      if(!current_vertex_)
         chain.push_back(point_2(p.x, p.y));

      return true;
   }

   bool on_release(const point_2f & p)
   {
      current_vertex_.reset();

      return true;
   }

   bool on_move(const point_2f & p)
   {
      if (current_vertex_)
      {
         chain[*current_vertex_] = p;
      }

      return true;
   }

   bool on_key(int key)
   {
      switch (key)
      {
      case Qt::Key_Up : eps += 1; break;
      case Qt::Key_Down : eps -= 1; break;
      default : return false;
      }

      return true;
   }

private:
   std::vector<point_2> chain;
   double eps;
   bool rbutton_pressed_;
   boost::optional<int> current_vertex_;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   contour_contains_point_viewer viewer;
   cg::visualization::run_viewer(&viewer, "Contour contains point");
}
