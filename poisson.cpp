#include <dolfin.h>
#include "Poisson.h"
#include <vector>

using namespace dolfin;

class Source : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    double dx = x[0] - 0.8;
    double dy = x[1] - 0.8;
    values[0] = 10*std::exp(-(dx*dx + dy*dy) / 0.02);
  }
};

class dUdN : public Expression
{
  void eval(Array<double>& values, const Array<double>& x) const
  {
    values[0] = std::sin(5*x[0]);
  }
};

class LeftBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    return x[0] < DOLFIN_EPS && on_boundary;
  }
};

class DownBoundary : public SubDomain
{
  bool inside(const Array<double>& x, bool on_boundary) const
  {
    return x[1] < DOLFIN_EPS && on_boundary;
  }
};

int main()
{
  auto mesh = std::make_shared<Mesh>(
    UnitSquareMesh::create({{32, 32}}, CellType::Type::triangle));
  auto V = std::make_shared<Poisson::FunctionSpace>(mesh);
  auto u_l = std::make_shared<Constant>(0.5);
  auto u_d = std::make_shared<Constant>(0.0);
  auto boundary_l = std::make_shared<LeftBoundary>();
  auto boundary_d = std::make_shared<DownBoundary>();
  DirichletBC bc_l(V, u_l, boundary_l);
  DirichletBC bc_d(V, u_d, boundary_d);
  Poisson::BilinearForm a(V, V);
  Poisson::LinearForm L(V);
  auto f = std::make_shared<Source>();
  auto g = std::make_shared<dUdN>();
  L.f = f;
  L.g = g;
  Function u(V);
  bc_l.apply(*u.vector());
  bc_d.apply(*u.vector());
  solve(a == L, u);
  File file("poisson.pvd");
  file << u;
  return 0;
}
