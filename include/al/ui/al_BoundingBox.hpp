
#ifndef _AL_BOUNDINGBOX_HPP__
#define _AL_BOUNDINGBOX_HPP__

#include "al/graphics/al_Graphics.hpp"
#include "al/graphics/al_Mesh.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Vec.hpp"

#include <limits>

namespace al {

/// @defgroup UI UI

struct BoundingBoxData {
  Vec3f min, max;
  Vec3f cen, dim;

  BoundingBoxData() { reset(); }
  BoundingBoxData(const Vec3f &min_, const Vec3f &max_) : min(min_), max(max_) {
    dim = max - min;
    Vec3f halfDim = dim / 2;
    cen = min + halfDim;
  }

  void set(const Vec3f &min_, const Vec3f &max_) {
    min.set(min_);
    max.set(max_);
    dim = max - min;
    Vec3f halfDim = dim / 2;
    cen = min + halfDim;
  }

  void setCenterDim(const Vec3f &cen_, const Vec3f &dim_) {
    cen.set(cen_);
    dim.set(dim_);
    min = cen - dim / 2;
    max = cen + dim / 2;
  }

  bool contains(const Vec3d &p) {
    if (p.x < min.x || p.x > max.x) return false;
    if (p.y < min.y || p.y > max.y) return false;
    if (p.z < min.z || p.z > max.z) return false;
    return true;
  }

  inline void reset() {
    min = {0, 0, 0};
    max = {0, 0, 0};
  }

  inline void resetInv() {
    min = {std::numeric_limits<float>::min(), std::numeric_limits<float>::min(),
           std::numeric_limits<float>::min()};
    max = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
           std::numeric_limits<float>::max()};
  }

  inline void includePoint(al::Vec3f &pos) {
    if (pos.x > max.x) {
      max.x = pos.x;
    } else if (pos.x < min.x) {
      min.x = pos.x;
    }
    if (pos.y > max.y) {
      max.y = pos.y;
    } else if (pos.y < min.y) {
      min.y = pos.y;
    }
    if (pos.z > max.z) {
      max.z = pos.z;
    } else if (pos.z < min.z) {
      min.z = pos.z;
    }
  }
};

/// BoundingBox
/// @ingroup UI
struct BoundingBox : public BoundingBoxData {
  Mesh mesh, tics, gridMesh[2];
  float glUnitLength;
  float ticsPerUnit;

  BoundingBox() : glUnitLength(1.0), ticsPerUnit(10.0) {}

  BoundingBox(const Vec3f &min_, const Vec3f &max_)
      : BoundingBoxData(min_, max_) {
    getMesh();
    getTics();
  }

  // set bounds from mesh
  void set(const Mesh &mesh) {
    Vec3f bbMin, bbMax;
    mesh.getBounds(bbMin, bbMax);
    set(bbMin, bbMax);
  }

  void set(const Vec3f &min_, const Vec3f &max_) {
    BoundingBoxData::set(min, max);
    getMesh();
    getTics();
  }

  void setCenterDim(const Vec3f &cen_, const Vec3f &dim_) {
    BoundingBoxData::setCenterDim(cen, dim);
    getMesh();
    getTics();
  }

  Mesh &getMesh() {
    mesh.reset();
    Vec3f halfDim = dim / 2;
    addWireBox(mesh, halfDim.x, halfDim.y, halfDim.z);
    mesh.translate(cen);
    return mesh;
  }

  void draw(Graphics &g, bool drawTics = false) {
    g.draw(mesh);
    if (drawTics) g.draw(tics);
  }

  // tic marks
  Mesh &getTics() {
    tics.reset();
    Vec3f halfDim = dim / 2;
    float ticLen = 0.1;  // tic length multiplier
    // x tics
    for (int i = 0; i < dim.x * ticsPerUnit; i++) {
      for (int z = 0; z < 2; z++) {
        for (int y = -1; y < 2; y++) {
          if (y != 0) {
            float len = ticLen;
            if (i % 5 == 0) len *= 2;
            if (i % 10 == 0) len *= 1.5;
            tics.vertex((i / ticsPerUnit) - halfDim.x, y * halfDim.y,
                        z * dim.z);
            tics.vertex((i / ticsPerUnit) - halfDim.x,
                        y * halfDim.y - (len * y), z * dim.z);

            tics.vertex((i / ticsPerUnit) - halfDim.x, y * halfDim.y,
                        z * dim.z);
            tics.vertex((i / ticsPerUnit) - halfDim.x, y * halfDim.y,
                        z * dim.z - (((z * 2) - 1) * len));
          }
        }
      }
    }
    // y tics
    for (int i = 0; i < dim.y * ticsPerUnit; i++) {
      for (int z = 0; z < 2; z++) {
        for (int x = -1; x < 2; x++) {
          if (x != 0) {
            float len = ticLen;
            if (i % 5 == 0) len *= 2;
            if (i % 10 == 0) len *= 1.5;
            tics.vertex(x * halfDim.x, (i / ticsPerUnit) - halfDim.y,
                        z * dim.z);
            tics.vertex(x * halfDim.x - (len * x),
                        (i / ticsPerUnit) - halfDim.y, z * dim.z);

            tics.vertex(x * halfDim.x, (i / ticsPerUnit) - halfDim.y,
                        z * dim.z);
            tics.vertex(x * halfDim.x, (i / ticsPerUnit) - halfDim.y,
                        z * dim.z - (((z * 2) - 1) * len));
          }
        }
      }
    }
    // z tics
    for (int i = 0; i < dim.z * ticsPerUnit; i++) {
      for (int z = 0; z < 2; z++) {
        for (int x = -1; x < 2; x++) {
          if (x != 0) {
            float len = ticLen;
            if (i % 5 == 0) len *= 2;
            if (i % 10 == 0) len *= 1.5;
            tics.vertex(z * dim.x - halfDim.x, x * halfDim.y,
                        (i / ticsPerUnit));
            tics.vertex(z * dim.x - halfDim.x, x * halfDim.y - (len * x),
                        (i / ticsPerUnit));

            tics.vertex(z * dim.x - halfDim.x, x * halfDim.y,
                        (i / ticsPerUnit));
            tics.vertex((z * dim.x - halfDim.x) - (((z * 2) - 1) * len),
                        x * halfDim.y, (i / ticsPerUnit));
          }
        }
      }
    }
    tics.color(.4, .4, .4);
    tics.primitive(Mesh::LINES);
    tics.translate(cen.x, cen.y, cen.z - (dim.z / 2));
    return tics;
  }
};

}  // namespace al

#endif  //_AL_BOUNDINGBOX_HPP__
