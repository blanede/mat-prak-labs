#include <gmsh.h>
#include <iostream>
#include <vector>
int main() {
  gmsh::initialize();
  gmsh::model::add("tor");
  double R = 2.0;
  double r_out = 0.6;
  double r_in = 0.4;
  double tetr_size_min = (r_out - r_in) / 4;
  double tetr_size_max = (r_out - r_in) / 3;
  int tor_out = gmsh::model::occ::addTorus(0, 0, 0, R, r_out);
  int tor_in = gmsh::model::occ::addTorus(0, 0, 0, R, r_in);
  std::vector<std::pair<int,int>> red = {{3, tor_out}};
  std::vector<std::pair<int,int>> ded = {{3, tor_in}};
  std::vector<std::pair<int,int>> res;
  std::vector<std::vector<std::pair<int,int>>> outMap;
  gmsh::model::occ::cut(red, ded, res, outMap);
  gmsh::model::occ::synchronize();
  gmsh::option::setNumber("Mesh.MeshSizeMax", tetr_size_max);
  gmsh::option::setNumber("Mesh.MeshSizeMin", tetr_size_min);
  gmsh::option::setNumber("Mesh.Algorithm3D", 10);
  gmsh::model::mesh::generate(3);
  gmsh::write("tor.msh");
  gmsh::fltk::run();
  gmsh::finalize();
  return 0;
}
