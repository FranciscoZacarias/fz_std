#ifndef ART_H
#define ART_H

typedef Vec4f32 Color;
#define color(x,y,z,a) vec4f32(x,y,z,a)

#define RED(a)       color(1.0f,  0.0f,  0.0f,  a)
#define GREEN(a)     color(0.0f,  1.0f,  0.0f,  a)
#define BLUE(a)      color(0.0f,  0.0f,  1.0f,  a)
#define YELLOW(a)    color(1.0f,  1.0f,  0.0f,  a)
#define CYAN(a)      color(0.0f,  1.0f,  1.0f,  a)
#define MAGENTA(a)   color(1.0f,  0.0f,  1.0f,  a)
#define WHITE(a)     color(1.0f,  1.0f,  1.0f,  a)
#define BLACK(a)     color(0.0f,  0.0f,  0.0f,  a)
#define GRAY(a)      color(0.5f,  0.5f,  0.5f,  a)
#define ORANGE(a)    color(1.0f,  0.5f,  0.0f,  a)
#define PURPLE(a)    color(0.5f,  0.0f,  0.5f,  a)
#define BROWN(a)     color(0.6f,  0.4f,  0.2f,  a)
#define PINK(a)      color(1.0f,  0.75f, 0.8f,  a)

function Color color_lerp(Vec4f32 a, Vec4f32 b, f32 t);

#endif // ART_H

function Color
color_lerp(Color a, Color b, f32 t)
{
  Color result = vec4f32_lerp(a, b, t);
  return result;
}