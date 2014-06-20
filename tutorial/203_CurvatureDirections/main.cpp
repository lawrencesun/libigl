#include <igl/readOFF.h>
#include <igl/viewer/Viewer.h>
#include <igl/per_vertex_normals.h>
#include <igl/per_face_normals.h>
#include <igl/per_corner_normals.h>
#include <igl/avg_edge_length.h>
#include <igl/barycenter.h>
#include <igl/principal_curvature.h>
#include <igl/jet.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>

Eigen::MatrixXd V;
Eigen::MatrixXi F;

int main(int argc, char *argv[])
{
  using namespace Eigen;
  // Load a mesh in OFF format
  igl::readOFF("../shared/fertility.off", V, F);

  // Alternative discrete mean curvature
  MatrixXd HN;
  SparseMatrix<double> L,M,Minv;
  igl::cotmatrix(V,F,L);
  igl::massmatrix(V,F,igl::MASSMATRIX_VORONOI,M);
  igl::invert_diag(M,Minv);
  HN = -Minv*(L*V);
  H = (HN.rowwise().squaredNorm()).array().sqrt();

  // Compute curvature directions via quadric fitting
  MatrixXd PD1,PD2;
  VectorXd PV1,PV2,H;
  igl::principal_curvature(V,F,PD1,PD2,PV1,PV2);
  // mean curvature
  H = 0.5*(PV1+PV2);

  igl::Viewer viewer;
  viewer.set_mesh(V, F);


  // Compute pseudocolor
  MatrixXd C;
  igl::jet(H,true,C);
  viewer.set_colors(C);

  // Average edge length for sizing
  const double avg = igl::avg_edge_length(V,F);
  
  // Draw a red segment parallel to the minimal curvature direction
  const RowVector3d red(1,0,0),blue(0,0,1);
  viewer.add_edges(V + PD1*avg, V - PD1*avg, red);

  // Draw a blue segment parallel to the maximal curvature direction
  viewer.add_edges(V + PD2*avg, V - PD2*avg, blue);

  // Hide wireframe
  viewer.options.show_lines = false;

  viewer.launch();
}
