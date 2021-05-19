#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <list>

#include <cairo/cairo-pdf.h>

void write_pdf(
        const std::function<void(cairo_t *)>& func,
        double xmin   = -10,
        double xmax   = 10,
        double ymin   = -10,
        double ymax   = 10,
        double width  = 100,
        double height = 100
) {
    cairo_surface_t *surface;
    cairo_t *cr;

    double origin_x = 0.0  - xmin;
    double origin_y = 0.0  - ymin;

    /* To render a pdf file, we must create a pdf surface using the cairo_pdf_surface_create() function call */

    surface = cairo_pdf_surface_create("testfile1.pdf", width, height);
    cr = cairo_create(surface);

    cairo_scale(cr, width / (xmax - xmin) , height / (ymax - ymin) );
    cairo_translate(cr, origin_x, origin_y);

    func(cr);

    /* the cairo_show_page() finishes rendering of the pdf file */
    cairo_show_page(cr);

    cairo_surface_destroy(surface);
    cairo_destroy(cr);
}

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2                                   Point_2;
typedef CGAL::Polygon_2<Kernel>                           Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel>                Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;

template<class Kernel, class Container>
void print_polygon (const CGAL::Polygon_2<Kernel, Container>& P)
{
    typename CGAL::Polygon_2<Kernel, Container>::Vertex_const_iterator vit;
    std::cout << "[ " << P.size() << " vertices:";
    for (vit = P.vertices_begin(); vit != P.vertices_end(); ++vit)
        std::cout << " (" << *vit << ')';
    std::cout << " ]" << std::endl;
}

template<class Kernel, class Container>
void print_polygon_with_holes(const CGAL::Polygon_with_holes_2<Kernel, Container> & pwh)
{
    if (! pwh.is_unbounded()) {
        std::cout << "{ Outer boundary = ";
        print_polygon (pwh.outer_boundary());
    } else {
        std::cout << "{ Unbounded polygon." << std::endl;
    }

    typename CGAL::Polygon_with_holes_2<Kernel,Container>::Hole_const_iterator hit;
    unsigned int k = 1;
    std::cout << " " << pwh.number_of_holes() << " holes:" << std::endl;
    for (hit = pwh.holes_begin(); hit != pwh.holes_end(); ++hit, ++k) {
        std::cout << " Hole #" << k << " = ";
        print_polygon (*hit);
    }
    std::cout << " }" << std::endl;
}


int main ()
{
    // Construct the two input polygons.
    Polygon_2 P;
    P.push_back (Point_2 (0, 0));
    P.push_back (Point_2 (5, 0));
    P.push_back (Point_2 (3.5, 1.5));
    P.push_back (Point_2 (2.5, 0.5));
    P.push_back (Point_2 (1.5, 1.5));
    std::cout << "P = "; print_polygon (P);
    Polygon_2 Q;
    Q.push_back (Point_2 (0, 2));
    Q.push_back (Point_2 (1.5, 0.5));
    Q.push_back (Point_2 (2.5, 1.5));
    Q.push_back (Point_2 (3.5, 0.5));
    Q.push_back (Point_2 (5, 2));
    std::cout << "Q = "; print_polygon (Q);
    // Compute the union of P and Q.
    Polygon_with_holes_2 unionR;
    if (CGAL::join (P, Q, unionR)) {
        std::cout << "The union: ";
        print_polygon_with_holes (unionR);
    } else
        std::cout << "P and Q are disjoint and their union is trivial."
                  << std::endl;
    std::cout << std::endl;
    // Compute the intersection of P and Q.
    Pwh_list_2                  intR;
    Pwh_list_2::const_iterator  it;
    CGAL::intersection (P, Q, std::back_inserter(intR));
    std::cout << "The intersection:" << std::endl;
    for (it = intR.begin(); it != intR.end(); ++it) {
        std::cout << "--> ";
        print_polygon_with_holes (*it);
    }

    write_pdf(
            [&](cairo_t* cr){
                cairo_set_line_width(cr, 0.1);
                cairo_set_source_rgb(cr, 0, 0, 0);

                auto line_to = [cr](auto point) {
                    cairo_line_to(cr,
                                  CGAL::to_double(point.x()),
                                  CGAL::to_double(point.y())
                                  );
                };

                auto draw_polygon = [cr, &line_to](const Polygon_2 & poly) {
                    auto point = poly.begin();
                    cairo_move_to(cr, CGAL::to_double(point->x()), CGAL::to_double(point->y()));
                    ++point;

                    for (; point != poly.end(); ++point) {
                        line_to(*point);
                    }
                    line_to(*poly.begin());
                };

                if (unionR.is_unbounded()) throw std::runtime_error {"polygon is unbounded."};

                draw_polygon(unionR.outer_boundary());

                for (auto hit = unionR.holes_begin(); hit != unionR.holes_end(); ++hit) {
                    draw_polygon (*hit);
                }

                cairo_stroke_preserve(cr);
                cairo_fill(cr);
            },
            -1.0, 6.0,
            -1.0, 6.0
            );

    return 0;
}
