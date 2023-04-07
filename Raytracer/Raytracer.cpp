#include <iostream>
#include "color.h"
#include "vec3.h"
#include "ray.h"

double hit_sphere(const point3& center, double radius, const ray& r)
{
    /*
    * C = the center of the sphere to render
    * A and b comes from P(t)=A+tb
    * t2b⋅b+2tb⋅(A−C)+(A−C)⋅(A−C)−r2=0
    */

    vec3 oc = r.origin() - center;
    //double a = dot(r.direction(), r.direction()); //a vector dotted with itself is equal to the squared length of that vector.
    auto a = r.direction().length_squared();
    /*
    there's something I'm not understanding on the following note.
    The below is an explenation to convert b to half_b
    double b = 2.0 * dot(oc, r.direction());
    double discriminant = b * b - 4 * a * c;
    else return (-b - sqrt(discriminant)) / (2.0 * a);
    
    notice how the equation for b has a factor of two in it.
    Consider what happens to the quadratic equation if b=2h:
        = ( −b ± sqrt(b2−4ac) ) / 2a
        = ( −2h ± sqrt((2h)2−4ac) ) / 2a
        = ( −2h ± 2 sqrt(h2−ac) ) / 2a
        = ( −h ± sqrt(h2−ac) ) / a

    It's like getting rid of the '2' and simplifying the final calculations.
    */
    double half_b = dot(oc, r.direction());;
    //double c = dot(oc, oc) - radius * radius; //a vector dotted with itself is equal to the squared length of that vector.
    double c = oc.length_squared() - radius * radius;
    double discriminant = half_b * half_b - a * c;
    //return (discriminant > 0); //This works in case we want to know there's a intersection
    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-half_b - sqrt(discriminant)) / a;
    }
}

/// <summary>
/// linearly blends white and blue depending on the height of the y coordinate after scaling the ray direction to unit length(so −1.0 < y < 1.0).
/// blendedValue=(1−t)⋅startValue+t⋅endValue,
/// </summary>
/// <param name="r"></param>
/// <returns></returns>
color ray_color(const ray& r)
{
    double t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if(t>0.0) //in case we hit the sphere
    {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main()
{
    // Image
    const float aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width/aspect_ratio);

    // Camera
    const float viewport_height = 2.0f;
    const float viewport_width = aspect_ratio * viewport_height;
    const float focal_length = 1.0f;

    point3 origin = point3(0, 0, 0);
    vec3 horizontal = vec3(viewport_width, 0, 0);
    vec3 vertical = vec3(0, viewport_height, 0);
    vec3 lower_left_corner = origin - (horizontal / 2) - (vertical / 2) - vec3(0, 0, focal_length);


    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) 
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) 
        {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + (u * horizontal) + (v * vertical) - origin);
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::cerr << "\nDone.\n";
}