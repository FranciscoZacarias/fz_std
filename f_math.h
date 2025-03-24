
#ifndef F_MATH_H
#define F_MATH_H

#define PI 3.14159265358979323846f
#define EPSILON 0.000001f

#define Degrees(r) (r * (180 / PI))
#define Radians(d) (d * (PI / 180))

typedef struct Vec2f32 {
    union {
        f32 data[2];
        struct {
            f32 x;
            f32 y;
        };
    };
} Vec2f32;
#define vec2f32(x,y) (Vec2f32){x,y}

typedef struct Vec3f32 {
  union {
    f32 data[3];
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
  };
} Vec3f32;
#define vec3f32(x,y,z) (Vec3f32){x,y,z}

typedef struct Vec4f32 {
  union {
    f32 data[4];
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };
} Vec4f32;
#define vec4f32(x,y,z,w) (Vec4f32){x,y,z,w}

typedef struct Mat4f32 {
  union {
    f32 data[4][4];
    struct {
      f32 m0, m4, m8,  m12,
          m1, m5, m9,  m13,
          m2, m6, m10, m14,
          m3, m7, m11, m15;
    };
  };
} Mat4f32;
#define mat4f32(diag) (Mat4f32) {diag,0.0f,0.0f,0.0f,0.0f,diag,0.0f,0.0f,0.0f,0.0f,diag,0.0f,0.0f,0.0f,0.0f,diag}

typedef struct Quatf32 {
  union {
    f32 data[4];
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };
} Quatf32;
#define quatf32(x,y,z,w) (Quatf32){x,y,z,w}
#define quatf32_identity() quatf32(0.0f, 0.0f, 0.0f, 1.0f)

typedef struct Transformf32 {
  Vec3f32    translation;
  Quatf32 rotation;
  Vec3f32    scale;
} Transformf32;
#define transform(t,r,s) (Transformf32){t,r,s}

internal f32 f32_min(f32 a, f32 b);
internal f32 f32_max(f32 a, f32 b);
internal f32 f32_abs(f32 value);
internal f32 f32_clamp(f32 value, f32 min, f32 max);
internal f32 f32_lerp(f32 start, f32 end, f32 amount);
internal f32 f32_normalize(f32 value, f32 start, f32 end);
internal f32 f32_remap(f32 value, f32 inputStart, f32 inputEnd, f32 outputStart, f32 outputEnd);
internal f32 f32_wrap(f32 value, f32 min, f32 max);
internal b32 f32_equals(f32 a, f32 b);

internal f32 vector2_distance(Vec2f32 a, Vec2f32 b);
internal f32 vector2_distance_signed(Vec2f32 a, Vec2f32 b, Vec2f32 reference);

internal Vec2f32 vector2_add(Vec2f32 a, Vec2f32 b);
internal Vec2f32 vector2_sub(Vec2f32 a, Vec2f32 b);
internal Vec2f32 vector2_scale(Vec2f32 v, f32 scalar);
internal Vec2f32 vector2_normalize(Vec2f32 v);
internal f32 vector2_dot(Vec2f32 a, Vec2f32 b);
internal f32 vector2_length(Vec2f32 v);

internal Vec3f32 vector3_from_vector4(Vec4f32 v); /* Discards the w value */
internal void print_vector3(Vec3f32 v, const char* label);

internal Vec3f32 vector3_add(Vec3f32 a, Vec3f32 b);
internal Vec3f32 vector3_sub(Vec3f32 a, Vec3f32 b);
internal Vec3f32 vector3_mul(Vec3f32 a, Vec3f32 b);
internal Vec3f32 vector3_div(Vec3f32 a, Vec3f32 b);

internal Vec3f32 vector3_cross(Vec3f32 a, Vec3f32 b);
internal Vec3f32 vector3_scale(Vec3f32 v, f32 scalar);
internal Vec3f32 vector3_scale_xyz(Vec3f32 v, f32 scale_x, f32 scale_y, f32 scale_z);
internal Vec3f32 vector3_normalize(Vec3f32 v);
internal Vec3f32 vector3_rotate_by_axis(Vec3f32 v, Vec3f32 axis, f32 angle);
internal Vec3f32 vector3_lerp(Vec3f32 a, Vec3f32 b, f32 t);
internal Vec3f32 vector3_unproject(Vec3f32 source, Mat4f32 projection, Mat4f32 view);
internal Vec3f32 vector3_mul_matrix4(Vec3f32 v, Mat4f32 m);

internal f32 vector3_dot(Vec3f32 a, Vec3f32 b);
internal f32 vector3_length(Vec3f32 v);
internal f32 vector3_distance(Vec3f32 a, Vec3f32 b);
internal f32 vector3_angle(Vec3f32 a, Vec3f32 b);

internal Vec4f32 vector4_from_vector3(Vec3f32 v);

internal Vec4f32 vector4_add(Vec4f32 a, Vec4f32 b);
internal Vec4f32 vector4_sub(Vec4f32 a, Vec4f32 b);
internal Vec4f32 vector4_mul(Vec4f32 a, Vec4f32 b);
internal Vec4f32 vector4_div(Vec4f32 a, Vec4f32 b);
internal Vec4f32 vector4_mul_matrix4(Vec4f32 v, Mat4f32 m);

internal Vec4f32 vector4_scale(Vec4f32 v, f32 scalar);
internal Vec4f32 vector4_normalize(Vec4f32 v);
internal Vec4f32 vector4_lerp(Vec4f32 a, Vec4f32 b, f32 t);

internal f32 vector4_dot(Vec4f32 a, Vec4f32 b);
internal f32 vector4_length(Vec4f32 v);
internal f32 vector4_distance(Vec4f32 a, Vec4f32 b);

internal Mat4f32 matrix4_mul(Mat4f32 left, Mat4f32 right); /* Apply the left matrix to the right matrix*/ 

internal Mat4f32 matrix4_translate(f32 x, f32 y, f32 z);
internal Mat4f32 matrix4_rotate_axis(Vec3f32 axis, f32 radians);
internal Mat4f32 matrix4_rotate_x(f32 radians);
internal Mat4f32 matrix4_rotate_y(f32 radians);
internal Mat4f32 matrix4_rotate_z(f32 radians);
internal Mat4f32 matrix4_rotate_xyz(Vec3f32 radians);
internal Mat4f32 matrix4_rotate_zyx(Vec3f32 radians);

internal Mat4f32 matrix4_transpose(Mat4f32 m);
internal Mat4f32 matrix4_scale(f32 x, f32 y, f32 z);
internal Mat4f32 matrix4_frustum(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
internal Mat4f32 matrix4_perspective(f64 fovy, f64 window_width, f64 window_height, f64 near_plane, f64 far_plane);
internal Mat4f32 matrix4_ortographic(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
internal Mat4f32 matrix4_look_at(Vec3f32 eye, Vec3f32 target, Vec3f32 up);
internal Mat4f32 matrix_from_quaternion(Quatf32 q);
internal Transformf32 transform_from_matrix4(Mat4f32 mat);

internal Quatf32 quaternion_add(Quatf32 q1, Quatf32 q2);
internal Quatf32 quaternion_add_value(Quatf32 q, f32 value);
internal Quatf32 quaternion_subtract(Quatf32 q1, Quatf32 q2);
internal Quatf32 quaternion_subtract_value(Quatf32 q, f32 value);
internal f32        quaternion_length(Quatf32 q);
internal Quatf32 quaternion_normalize(Quatf32 q);
internal Quatf32 quaternion_invert(Quatf32 q);
internal Quatf32 quaternion_multiply(Quatf32 q1, Quatf32 q2);
internal Quatf32 quaternion_scale(Quatf32 q, f32 scalar);
internal Quatf32 quaternion_divide(Quatf32 q1, Quatf32 q2);
internal Quatf32 quaternion_lerp(Quatf32 q1, Quatf32 q2, f32 amount);
internal Quatf32 quaternion_nlerp(Quatf32 q1, Quatf32 q2, f32 amount);
internal Quatf32 quaternion_slerp(Quatf32 q1, Quatf32 q2, f32 amount);
internal Quatf32 quaternion_cubic_hermit_spline(Quatf32 q1, Quatf32 outTangent1, Quatf32 q2, Quatf32 inTangent2, f32 t);
internal Quatf32 quaternion_from_vector3_to_vector3(Vec3f32 from, Vec3f32 to);
internal Quatf32 quaternion_from_matrix(Mat4f32 mat);
internal Quatf32 quaternion_from_axis_angle(Vec3f32 axis, f32 angle);
internal void       axis_angle_from_quaternion(Quatf32 q, Vec3f32 *axis, f32 *angle);
internal Quatf32 quaternion_from_euler(f32 pitch, f32 yaw, f32 roll);
internal void       euler_from_quaternion(Quatf32 q, f32* pitch, f32* yaw, f32* roll);
internal Quatf32 quaternion_mul_matrix4(Quatf32 q, Mat4f32 mat);
internal b32        quaternion_equals(Quatf32 p, Quatf32 q);

#endif // F_MATH_H