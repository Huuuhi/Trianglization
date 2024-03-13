#include <iostream>
#include <vector>
#include <tuple>
#include <optional>

// 三维点的结构体
struct Point3D
{
  double x, y, z;
  Point3D(double x, double y, double z) : x(x), y(y), z(z)
  {}
  bool operator==(const Point3D &another) const
  {
    return this->x == another.x && this->y == another.y && this->z == another.z;
  }
  bool operator!=(const Point3D &another) const
  {
    return this->x != another.x || this->y != another.y || this->z != another.z;
  }
};

// 三维多边形的顶点列表
std::vector<Point3D> polygon = {
    Point3D(0.0, 0.0, 0.0),
    Point3D(1.0, 0.0, 0.0),
    Point3D(1.0, 1.0, 0.0),
    Point3D(0.5, 1.5, 0.0),
    Point3D(0.0, 1.0, 0.0)
};

// 初始化三角形列表
std::vector<std::tuple<Point3D, Point3D, Point3D>> triangles;

bool is_convex(const Point3D &a, const Point3D &b, const Point3D &c)
{
  // 用叉积判断三个点是否构成一个凸多边形
  double cross_product = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
  return cross_product >= 0.0; // 如果叉积大于等于0，说明是凸多边形
}

bool is_in_triangle(const Point3D &p, const Point3D &a, const Point3D &b, const Point3D &c)
{
  // 用叉积判断点 p 是否在三角形 abc 内部
  double ab_cross = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
  double bc_cross = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x);
  double ca_cross = (a.x - c.x) * (p.y - c.y) - (a.y - c.y) * (p.x - c.x);
  return ab_cross >= 0.0 && bc_cross >= 0.0 && ca_cross >= 0.0;
}

bool is_ear(const Point3D &vertex, const std::vector<Point3D> &polygon)
{
  // 顶点是否是"耳朵"的条件：它的内角不包含其他点
  size_t num_vertices = polygon.size();
  for (size_t i = 0; i < num_vertices; ++i)
  {
    const Point3D &prev = polygon[(i + num_vertices - 1) % num_vertices];
    const Point3D &current = polygon[i];
    const Point3D &next = polygon[(i + 1) % num_vertices];

    // 检查内角
    if (is_convex(prev, current, next) &&
        is_in_triangle(vertex, prev, current, next))
    {
      return false;
    }
  }
  return true;
}

std::optional<Point3D> find_ear(const std::vector<Point3D> &polygon)
{
  // 找到一个"耳朵"顶点
  for (const Point3D &vertex : polygon)
  {
    if (is_ear(vertex, polygon))
    {
      return vertex;
    }
  }
  return std::nullopt; // 如果找不到耳朵，返回第一个顶点
}

std::vector<Point3D> remove_ear(const Point3D &ear_vertex, const std::vector<Point3D> &polygon)
{
  // 移除一个"耳朵"顶点，并返回分割后的两个子多边形
  size_t num_vertices = polygon.size();
  std::vector<Point3D> new_polygon;

  for (size_t i = 0; i < num_vertices; ++i)
  {
    if (polygon[i] != ear_vertex)
    {
      new_polygon.push_back(polygon[i]);
    } else
    {
      break;
    }
  }

  for (size_t i = 0; i < num_vertices - 2; ++i)
  {
    if (new_polygon[i] == polygon[0])
    {
      new_polygon.push_back(polygon[i + 2]);
    }
  }

  return new_polygon;
}
bool process_polygon() {
  std::optional<Point3D> ear_vertex = find_ear(polygon);

  if (!ear_vertex.has_value()) {
    return false; // 没有找到耳朵，无法继续
  }

  auto new_polygon = remove_ear(ear_vertex.value(), polygon);
  triangles.emplace_back(std::make_tuple(polygon[0], ear_vertex.value(), polygon[1])); // 添加三角形
  polygon = new_polygon;
  return true; // 成功找到并移除耳朵
}

int main()
{
  while (polygon.size() >= 3) {
    bool success = process_polygon();
    if (!success) {
      break; // 找不到耳朵或无法移除，结束三角化
    }
  }

  // 输出三角化结果
  for (const auto& triangle : triangles) {
    std::cout << "Triangle: ";
    std::cout << "(" << std::get<0>(triangle).x << ", " << std::get<0>(triangle).y << ", " << std::get<0>(triangle).z << ") ";
    std::cout << "(" << std::get<1>(triangle).x << ", " << std::get<1>(triangle).y << ", " << std::get<1>(triangle).z << ") ";
    std::cout << "(" << std::get<2>(triangle).x << ", " << std::get<2>(triangle).y << ", " << std::get<2>(triangle).z << ")\n";
  }
  return 0;
}
