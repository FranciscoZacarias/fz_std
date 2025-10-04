#ifndef MATH_H
#define MATH_H

#define PI 3.14159265358979323846f
#define EPSILON 0.000001f

#define Degrees(r) (r * (180 / PI))
#define Radians(d) (d * (PI / 180))

// @Section: Vec2
typedef union Vec2u16 { u16 data[2]; struct { u16 x, y; }; } Vec2u16;
#define vec2u16(x,y) ((Vec2u16){x,y})
typedef union Vec2u32 { u32 data[2]; struct { u32 x, y; }; } Vec2u32;
#define vec2u32(x,y) ((Vec2u32){x,y})
typedef union Vec2u64 { u64 data[2]; struct { u64 x, y; }; } Vec2u64;
#define vec2u64(x,y) ((Vec2u64){x,y})
typedef union Vec2s16 { s16 data[2]; struct { s16 x, y; }; } Vec2s16;
#define vec2s16(x,y) ((Vec2s16){x,y})
typedef union Vec2s32 { s32 data[2]; struct { s32 x, y; }; } Vec2s32;
#define vec2s32(x,y) ((Vec2s32){x,y})
typedef union Vec2s64 { s64 data[2]; struct { s64 x, y; }; } Vec2s64;
#define vec2s64(x,y) ((Vec2s64){x,y})
typedef union Vec2f32 { f32 data[2]; struct { f32 x, y; }; } Vec2f32;
#define vec2f32(x,y) ((Vec2f32){x,y})
typedef union Vec2f64 { f64 data[2]; struct { f64 x, y; }; } Vec2f64;
#define vec2f64(x,y) ((Vec2f64){x,y})

// @Section: Vec3
typedef union Vec3u16 { u16 data[3]; struct { u16 x, y, z; }; } Vec3u16;
#define vec3u16(x,y,z) ((Vec3u16){x,y,z})
typedef union Vec3u32 { u32 data[3]; struct { u32 x, y, z; }; } Vec3u32;
#define vec3u32(x,y,z) ((Vec3u32){x,y,z})
typedef union Vec3u64 { u64 data[3]; struct { u64 x, y, z; }; } Vec3u64;
#define vec3u64(x,y,z) ((Vec3u64){x,y,z})
typedef union Vec3s16 { s16 data[3]; struct { s16 x, y, z; }; } Vec3s16;
#define vec3s16(x,y,z) ((Vec3s16){x,y,z})
typedef union Vec3s32 { s32 data[3]; struct { s32 x, y, z; }; } Vec3s32;
#define vec3s32(x,y,z) ((Vec3s32){x,y,z})
typedef union Vec3s64 { s64 data[3]; struct { s64 x, y, z; }; } Vec3s64;
#define vec3s64(x,y,z) ((Vec3s64){x,y,z})
typedef union Vec3f32 { f32 data[3]; struct { f32 x, y, z; }; } Vec3f32;
#define vec3f32(x,y,z) ((Vec3f32){x,y,z})
typedef union Vec3f64 { f64 data[3]; struct { f64 x, y, z; }; } Vec3f64;
#define vec3f64(x,y,z) ((Vec3f64){x,y,z})

// @Section: Vec4
typedef union Vec4u16 { u16 data[4]; struct { u16 x, y, z, w; }; } Vec4u16;
#define vec4u16(x,y,z,w) ((Vec4u16){x,y,z,w})
typedef union Vec4u32 { u32 data[4]; struct { u32 x, y, z, w; }; } Vec4u32;
#define vec4u32(x,y,z,w) ((Vec4u32){x,y,z,w})
typedef union Vec4u64 { u64 data[4]; struct { u64 x, y, z, w; }; } Vec4u64;
#define vec4u64(x,y,z,w) ((Vec4u64){x,y,z,w})
typedef union Vec4s16 { s16 data[4]; struct { s16 x, y, z, w; }; } Vec4s16;
#define vec4s16(x,y,z,w) ((Vec4s16){x,y,z,w})
typedef union Vec4s32 { s32 data[4]; struct { s32 x, y, z, w; }; } Vec4s32;
#define vec4s32(x,y,z,w) ((Vec4s32){x,y,z,w})
typedef union Vec4s64 { s64 data[4]; struct { s64 x, y, z, w; }; } Vec4s64;
#define vec4s64(x,y,z,w) ((Vec4s64){x,y,z,w})
typedef union Vec4f32 { f32 data[4]; struct { f32 x, y, z, w; }; } Vec4f32;
#define vec4f32(x,y,z,w) ((Vec4f32){x,y,z,w})
typedef union Vec4f64 { f64 data[4]; struct { f64 x, y, z, w; }; } Vec4f64;
#define vec4f64(x,y,z,w) ((Vec4f64){x,y,z,w})

// @Section: Rect
typedef struct Rectf32 { Vec2f32 top_left; Vec2f32 size; } Rectf32;
#define rectf32(top_left,size) ((Rectf32){top_left,size})
typedef struct Rects32 { Vec2s32 top_left; Vec2s32 size; } Rects32;
#define rects32(top_left,size) ((Rects32){top_left,size})
typedef struct Rectu32 { Vec2u32 top_left; Vec2u32 size; } Rectu32;
#define rectu32(top_left,size) ((Rectu32){top_left,size})

// @Section: Matrix
// Column major
typedef union Mat4f32 {
  f32 data[4][4];
  struct {
    f32 m0, m4, m8,  m12,
        m1, m5, m9,  m13,
        m2, m6, m10, m14,
        m3, m7, m11, m15;
  };
} Mat4f32;
#define mat4f32(diag) (Mat4f32) {diag,0.0f,0.0f,0.0f,0.0f,diag,0.0f,0.0f,0.0f,0.0f,diag,0.0f,0.0f,0.0f,0.0f,diag}
#define mat4f32_identity() mat4f32(1.0f)

// @Section: Quaternion
typedef union Quatf32 {
  f32 data[4];
  struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
  };
} Quatf32;
#define quatf32(x,y,z,w) (Quatf32){x,y,z,w}
#define quatf32_identity() quatf32(0.0f, 0.0f, 0.0f, 1.0f)

// @Section: Transform
typedef struct Transform3f32 Transform3f32;
struct Transform3f32 {
  Vec3f32 translation;
  Quatf32 rotation;
  Vec3f32 scale;
};
#define transform3f32(t,r,s) (Transform3f32){t,r,s}

typedef struct Transform2f32 Transform2f32;
struct Transform2f32 {
  Vec2f32 translation;
  f32 rotation;
  Vec2f32 scale;
};
#define transform2f32(t,r,s) (Transform2f32){t,r,s}

function f32 f32_min(f32 a, f32 b);
function f32 f32_max(f32 a, f32 b);
function f32 f32_abs(f32 value);
function f32 f32_lerp(f32 start, f32 end, f32 amount);
function f32 f32_normalize(f32 value, f32 start, f32 end);
function f32 f32_remap(f32 value, f32 inputStart, f32 inputEnd, f32 outputStart, f32 outputEnd);
function f32 f32_wrap(f32 value, f32 min, f32 Max);

function f32 vec2f32_distance(Vec2f32 a, Vec2f32 b);
function f32 vec2f32_distance_signed(Vec2f32 a, Vec2f32 b, Vec2f32 reference);

function Vec2f32 vec2f32_add(Vec2f32 a, Vec2f32 b);
function Vec2f32 vec2f32_sub(Vec2f32 a, Vec2f32 b);
function Vec2f32 vec2f32_mul(Vec2f32 a, Vec2f32 b);
function Vec2f32 vec2f32_scale(Vec2f32 v, f32 scalar);
function Vec2f32 vec2f32_rotate(Vec2f32 v, f32 radians);
function Vec2f32 vec2f32_normalize(Vec2f32 v);
function Vec2f32 vec2f32_lerp(Vec2f32 a, Vec2f32 b, f32 t);
function f32 vec2f32_dot(Vec2f32 a, Vec2f32 b);
function f32 vec2f32_length(Vec2f32 v);

function Vec3f32 vec3f32_from_vec4f32(Vec4f32 v); /* Discards the w value */
function Vec3f32 vec3f32_add(Vec3f32 a, Vec3f32 b);
function Vec3f32 vec3f32_sub(Vec3f32 a, Vec3f32 b);
function Vec3f32 vec3f32_mul(Vec3f32 a, Vec3f32 b);
function Vec3f32 vec3f32_div(Vec3f32 a, Vec3f32 b);
function Vec3f32 vec3f32_cross(Vec3f32 a, Vec3f32 b);
function Vec3f32 vec3f32_scale(Vec3f32 v, f32 scalar);
function Vec3f32 vec3f32_scale_xyz(Vec3f32 v, f32 scale_x, f32 scale_y, f32 scale_z);
function Vec3f32 vec3f32_normalize(Vec3f32 v);
function Vec3f32 vec3f32_rotate_by_axis(Vec3f32 v, Vec3f32 axis, f32 angle);
function Vec3f32 vec3f32_lerp(Vec3f32 a, Vec3f32 b, f32 t);
function Vec3f32 vec3f32_unproject(Vec3f32 source, Mat4f32 projection, Mat4f32 view);
function f32     vec3f32_dot(Vec3f32 a, Vec3f32 b);
function f32     vec3f32_length(Vec3f32 v);
function f32     vec3f32_distance(Vec3f32 a, Vec3f32 b);
function f32     vec3f32_angle(Vec3f32 a, Vec3f32 b);

function Vec3s32 vec3s32_add(Vec3s32 a, Vec3s32 b);

function Vec4f32 vec4f32_from_vec3f32(Vec3f32 v);
function Vec4f32 vec4f32_add(Vec4f32 a, Vec4f32 b);
function Vec4f32 vec4f32_sub(Vec4f32 a, Vec4f32 b);
function Vec4f32 vec4f32_mul(Vec4f32 a, Vec4f32 b);
function Vec4f32 vec4f32_div(Vec4f32 a, Vec4f32 b);
function Vec4f32 vec4f32_mul_mat4f32(Vec4f32 v, Mat4f32 m);

function Vec4f32 vec4f32_scale(Vec4f32 v, f32 scalar);
function Vec4f32 vec4f32_normalize(Vec4f32 v);
function Vec4f32 vec4f32_lerp(Vec4f32 a, Vec4f32 b, f32 t);

function f32 vec4f32_dot(Vec4f32 a, Vec4f32 b);
function f32 vec4f32_length(Vec4f32 v);
function f32 vec4f32_distance(Vec4f32 a, Vec4f32 b);

function b32     mat4f32_equals(Mat4f32 a, Mat4f32 b);
function Mat4f32 mat4f32_mul(Mat4f32 left, Mat4f32 right); /* Apply the left matrix to the right matrix*/ 

function Vec3f32 mat4f32_transform_vec3f32(Mat4f32 mat, Vec3f32 vec);
function Mat4f32 mat4f32_translate(f32 x, f32 y, f32 z);
function Mat4f32 mat4f32_rotate_axis(Vec3f32 axis, f32 radians);
function Mat4f32 mat4f32_rotate_x(f32 radians);
function Mat4f32 mat4f32_rotate_y(f32 radians);
function Mat4f32 mat4f32_rotate_z(f32 radians);
function Mat4f32 mat4f32_rotate_xyz(Vec3f32 radians);
function Mat4f32 mat4f32_rotate_zyx(Vec3f32 radians);

function Mat4f32 mat4f32_transpose(Mat4f32 m);
function Mat4f32 mat4f32_scale(f32 x, f32 y, f32 z);
function Mat4f32 mat4f32_inverse(Mat4f32 m);
function Mat4f32 mat4f32_frustum(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
function Mat4f32 mat4f32_perspective(f32 fovy, f32 window_width, f32 window_height, f32 near_plane, f32 far_plane);
function Mat4f32 mat4f32_orthographic(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
function Mat4f32 mat4f32_look_at(Vec3f32 eye, Vec3f32 target, Vec3f32 up);
function Mat4f32 mat4f32_from_quatf32(Quatf32 q);
function Transform3f32 transformf32_from_mat4f32(Mat4f32 mat);

function Quatf32 quatf32_add(Quatf32 q1, Quatf32 q2);
function Quatf32 quatf32_add_value(Quatf32 q, f32 value);
function Quatf32 quatf32_subtract(Quatf32 q1, Quatf32 q2);
function Quatf32 quatf32_subtract_value(Quatf32 q, f32 value);
function f32     quatf32_length(Quatf32 q);
function Quatf32 quatf32_normalize(Quatf32 q);
function Quatf32 quatf32_invert(Quatf32 q);
function Quatf32 quatf32_multiply(Quatf32 q1, Quatf32 q2);
function Quatf32 quatf32_scale(Quatf32 q, f32 scalar);
function Quatf32 quatf32_divide(Quatf32 q1, Quatf32 q2);
function Quatf32 quatf32_lerp(Quatf32 q1, Quatf32 q2, f32 amount);
function Quatf32 quatf32_nlerp(Quatf32 q1, Quatf32 q2, f32 amount);
function Quatf32 quatf32_slerp(Quatf32 q1, Quatf32 q2, f32 amount);
function Quatf32 quatf32_cubic_hermit_spline(Quatf32 q1, Quatf32 outTangent1, Quatf32 q2, Quatf32 inTangent2, f32 t);
function Quatf32 quatf32_from_vec3f32_to_vec3f32(Vec3f32 from, Vec3f32 to);
function Quatf32 quatf32_from_mat4f32(Mat4f32 mat);
function Quatf32 quatf32_from_axis_angle(Vec3f32 axis, f32 angle);
function void    axis_angle_from_quatf32(Quatf32 q, Vec3f32 *axis, f32 *angle);
function Quatf32 quatf32_from_euler(f32 pitch, f32 yaw, f32 roll);
function void    euler_from_quatf32(Quatf32 q, f32* pitch, f32* yaw, f32* roll);
function Quatf32 quatf32_mul_mat4f32(Quatf32 q, Mat4f32 mat);
function b32     quatf32_equals(Quatf32 p, Quatf32 q);
function Vec3f32 quatf32_rotate_vec3f32(Quatf32 q, Vec3f32 v);
function Quatf32 quatf32_conjugate(Quatf32 q);
function f32     quatf32_dot(Quatf32 q1, Quatf32 q2);

///////////////////////
//~ Prints

function void vec2f32_print(Vec2f32 v, const u8 *label);
function void vec3f32_print(Vec3f32 v, const u8 *label);
function void vec4f32_print(Vec4f32 v, const u8 *label);
function void mat4f32_print(Mat4f32 m, const u8 *label);
function void quatf32_print(Quatf32 q, const u8 *label);
function void transformf32_print(Transform3f32 t, const u8 *label);

#endif // MATH_H