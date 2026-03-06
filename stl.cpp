#include <gmsh.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
int main() {
  gmsh::initialize();
  gmsh::model::add("stl");
  std::string stl_file = "doggy.stl";
  gmsh::merge(stl_file);
  gmsh::model::mesh::classifySurfaces(30 * M_PI / 180, true, true, M_PI);
  gmsh::model::mesh::createGeometry();
  std::vector<std::pair<int,int>> surfaces;
  gmsh::model::getEntities(surfaces, 2);
  std::vector<int> tags;
  for (int i = 0; i < static_cast<int>(surfaces.size()); i++) {
    tags.push_back(surfaces[i].second);
  }
  int loop = gmsh::model::geo::addSurfaceLoop(tags);
  std::vector<int> volume = {loop};
  gmsh::model::geo::addVolume(volume);
  gmsh::model::geo::synchronize();
  gmsh::option::setNumber("Mesh.MeshSizeMax", 2.0);
  gmsh::option::setNumber("Mesh.MeshSizeMin", 1.0);
  gmsh::option::setNumber("Mesh.Algorithm3D", 10);
  gmsh::model::mesh::generate(3);
  gmsh::write("dog.msh");
  gmsh::fltk::run();
  gmsh::finalize();
  return 0;
}
