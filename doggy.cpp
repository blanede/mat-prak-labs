#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
int main() {
  auto reader = vtkSmartPointer<vtkSTLReader>::New();
  reader->SetFileName("doggy.stl");
  reader->Update();
  auto surface = reader->GetOutput();
  auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
  grid->DeepCopy(surface);
  int num_points = grid->GetNumberOfPoints();
  std::vector<int> head;
  int ind_c = -1;
  double min_x = 0.0;
  double min_z = 1000.0;
  double max_z = -1000.0;
  for (int i = 0; i < num_points; i++) {
    double point[3];
    grid->GetPoint(i, point);
    double x = point[0];
    double y = point[1];
    double z = point[2];
    if (x >= -29.1 && z >= 46.3) {
      head.push_back(i);
      if (x < min_x) {
        min_x = x;
        ind_c = i;
      }
    }
    if (z > max_z) {
      max_z = z;
    }
    if (z < min_z) {
      min_z = z;
    }
  }
  int frames = 30;
  double time = 2;
  double freq = 2.0;
  double ampl = 0.2;
  system("mkdir -p output");
  for (int i = 0; i < frames; i++) {
    double cur_time = static_cast<double>(i) / (frames - 1) * time;
    double angle = sin(2 * M_PI * freq * cur_time);
    auto cur_frame = vtkSmartPointer<vtkUnstructuredGrid>::New();
    cur_frame->DeepCopy(grid);
    auto points = cur_frame->GetPoints();
    double center[3];
    points->GetPoint(ind_c, center);
    double x_c = center[0];
    double y_c = center[1];
    double z_c = center[2];
    for (int j = 0; j < static_cast<int>(head.size()); j++) {
      double point[3];
      points->GetPoint(head[j], point);
      double x = point[0];
      double y = point[1];
      double z = point[2];
      double dist = sqrt((x - x_c) * (x - x_c) + (y - y_c) * (y - y_c) + (z - z_c) * (z - z_c));
      double new_y = y + angle * ampl * dist;
      points->SetPoint(head[j], x, new_y, z);
    }
    auto velocity = vtkSmartPointer<vtkDoubleArray>::New();
    velocity->SetName("Velocity");
    velocity->SetNumberOfComponents(3);
    auto color = vtkSmartPointer<vtkDoubleArray>::New();
    color->SetName("Color");
    color->SetNumberOfComponents(1);
    for (int k = 0; k < num_points; k++) {
      double point[3];
      points->GetPoint(k, point);
      double x = point[0];
      double y = point[1];
      double z = point[2];
      bool is_head = false;
      for (int t = 0; t < static_cast<int>(head.size()); t++) {
        if (head[t] == k) {
          is_head = true;
          break;
        }
      } 
      if (is_head) {
        double vy = ampl * 2 * M_PI * freq * cos(2 * M_PI * freq * cur_time);
        velocity->InsertNextTuple3(0.0, vy, 0.0);
      } else {
        velocity->InsertNextTuple3(0.0, 0.0, 0.0);
      }
      color->InsertNextValue(0.0 + 10.0 * ((static_cast<int>((z - min_z) * 7 / (max_z - min_z)) + i) % 7));
    }
    cur_frame->GetPointData()->AddArray(velocity);
    cur_frame->GetPointData()->SetActiveVectors("Velocity");
    cur_frame->GetPointData()->AddArray(color);
    cur_frame->GetPointData()->SetActiveScalars("Color");
    std::string filename = "output/frame_" + std::to_string(i) + ".vtu";
    auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
    writer->SetDataModeToAscii();
    writer->SetCompressor(nullptr);
    writer->SetFileName(filename.c_str());
    writer->SetInputData(cur_frame);
    writer->Write();
  }
  return 0;
}
