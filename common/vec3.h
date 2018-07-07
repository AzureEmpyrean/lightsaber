#ifndef COMMON_VEC3_H
#define COMMON_VEC3_H

// Simple 3D vector.
class Vec3 {
public:
  Vec3(){}
  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

  enum ByteOrder {
    BYTEORDER_MSB = 0,
    BYTEORDER_LSB = 1,
  };
  enum Orientation {
    ORIENTATION_NORMAL = 0x00,
    ORIENTATION_FETS_TOWARDS_BLADE = 0x00,
    ORIENTATION_USB_TOWARDS_BLADE = 0x10,
    ORIENTATION_SDA_TOWARDS_BLADE = 0x02,
    ORIENTATION_SERIAL_TOWARDS_BLADE = 0x12,
    ORIENTATION_TOP_TOWARDS_BLADE = 0x04,
    ORIENTATION_BOTTOM_TOWARDS_BLADE = 0x14,
  };
  static int16_t GetShort(const unsigned char *data, int LSB) {
    return (data[LSB] << 8) | data[1-LSB];
  }
  static Vec3 FromData(const unsigned char* data, float mul, int LSB,
		       Orientation ORIENTATION) {
    Vec3 ret;
    ret.x = mul * GetShort(data + (0 + (ORIENTATION & 7)) % 6, LSB);
    ret.y = mul * GetShort(data + (2 + (ORIENTATION & 7)) % 6, LSB);
    ret.z = mul * GetShort(data + (4 + (ORIENTATION & 7)) % 6, LSB);
    if (ORIENTATION & 0x10) {
      ret.x = - ret.x;
      ret.y = - ret.y;
    }
    return ret;
  }

  Vec3 operator-(const Vec3& o) const {
    return Vec3(x - o.x, y - o.y, z - o.z);
  }
  Vec3 operator+(const Vec3& o) const {
    return Vec3(x + o.x, y + o.y, z + o.z);
  }
  void operator+=(const Vec3& o)  {
    x += o.x;
    y += o.y;
    z += o.z;
  }
  Vec3 operator*(float f) const {
    return Vec3(x * f, y * f, z * f);
  }
  Vec3 operator/(int i) const {
    return Vec3(x / i, y / i, z / i);
  }
  Vec3 dot(const Vec3& o) const {
    return Vec3(x * o.x, y * o.y, z * o.z);
  }
  float len2() const { return x*x + y*y + z*z; }
  float len() const { return sqrt(len2()); }

  static void Rotate90(float& a, float& b) {
    float tmp = b;
    b = -a;
    a = tmp;
  }
  static void Rotate180(float& a, float& b) {
    a = -a;
    b = -b;
  }
  void RotateX90() { Rotate90(y, z); }
  void RotateZ90() { Rotate90(x, y); }
  void RotateY90() { Rotate90(z, x); }

  void RotateX180() { Rotate180(y, z); }
  void RotateZ180() { Rotate180(x, y); }
  void RotateY180() { Rotate180(z, x); }
  float x, y, z;
};

#endif
