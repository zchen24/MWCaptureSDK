#ifndef MW_OPENGL_SHADER_H
#define MW_OPENGL_SHADER_H


static const char* g_vertex_shader_str =
"#version 130\n"
"in vec2 vertexIn;\n"
"in vec2 textureIn;\n"
"out vec2 textureOut;\n"
"void main(void)\n"
"{\n"
"   gl_Position = vec4(vertexIn,0.0,1.0);\n"
"   textureOut = textureIn;\n"
"}\n";

static const char* g_rgb_fragment_shader_str =
"#version 130\n"
"uniform sampler2D tex_rgb;\n"
"uniform vec3 rgb_rng;\n"
"in vec2 textureOut;\n"
"void main(void)\n"
"{\n"
"   float r,g,b;\n"
"   vec4 t_color = texture2D(tex_rgb,textureOut);\n"
"   b = (t_color.r-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   g = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   r = (t_color.b-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   gl_FragColor = vec4(r,g,b,1.0f);\n"
"}\n";
static char *g_bgr_fragment_shader_str1 =
"#version 130 \n"
"uniform sampler2D tex_rgb; \n"
"in vec2 textureOut; \n"
"void main(void) \n"
"{ \n"
"  float r, g, b; \n"
"  r = texture2D(tex_rgb, textureOut).r; \n"
"  g = texture2D(tex_rgb, textureOut).g; \n"
"  b = texture2D(tex_rgb, textureOut).b; \n"
"  gl_FragColor=vec4(r,g,b,1.0); \n"
"} \n";
static const char* g_bgr_fragment_shader_str =
"#version 130\n"
"uniform sampler2D tex_rgb;\n"
"uniform vec3 rgb_rng;\n"
"in vec2 textureOut;\n"
"void main(void)\n"
"{\n"
"   float r,g,b;\n"
"   vec4 t_color = texture2D(tex_rgb,textureOut);\n"
"   b = (t_color.b-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   g = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   r = (t_color.r-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   gl_FragColor = vec4(r,g,b,1.0f);\n"
"}\n";

static const char* g_argb_fragment_shader_str =
"#version 130\n"
"uniform sampler2D tex_rgb;\n"
"uniform vec3 rgb_rng;\n"
"in vec2 textureOut;\n"
"void main(void)\n"
"{\n"
"   float r,g,b;\n"
"   vec4 t_color = texture2D(tex_rgb,textureOut);\n"
"   r = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   g = (t_color.b-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   b = (t_color.a-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   gl_FragColor = vec4(r,g,b,1.0f);\n"
"}\n";

static const char* g_abgr_fragment_shader_str =
"#version 130\n"
"uniform sampler2D tex_rgb;\n"
"uniform vec3 rgb_rng;\n"
"in vec2 textureOut;\n"
"void main(void)\n"
"{\n"
"   float r,g,b;\n"
"   vec4 t_color = texture2D(tex_rgb,textureOut);\n"
"   r = (t_color.a-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   g = (t_color.b-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   b = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n"
"   gl_FragColor = vec4(r,g,b,1.0f);\n"
"}\n";

static const char* g_yuy2_fragment_shader_str =
"#version 130 \n"
"uniform sampler2D tex_yuv_rg; \n"
"uniform sampler2D tex_yuv_rgba; \n"
"uniform mat3 cspmat;\n"
"uniform vec3 cspconst;\n"
"in vec2 textureOut; \n"
"void main(void) \n"
"{ \n"
"   vec3 rgb; \n"
"   vec3 yuv; \n"
"   yuv.r = texture2D(tex_yuv_rg, textureOut).r; \n"
"   yuv.g = texture2D(tex_yuv_rgba, textureOut).g; \n"
"   yuv.b = texture2D(tex_yuv_rgba, textureOut).a; \n"
"   rgb = yuv*cspmat+cspconst;\n"
"   gl_FragColor=vec4(rgb,1.0); \n"
"} \n";

static const char* g_nv12_fragment_shader_str = 
"#version 130 \n"
"uniform sampler2D tex_y; \n"
"uniform sampler2D tex_uv; \n"
"uniform mat3 cspmat;\n"
"uniform vec3 cspconst;\n"
"in vec2 textureOut; \n"
"void main(void) \n"
"{ \n"
"   vec3 rgb; \n"
"   vec3 yuv; \n"
"   yuv.r = texture2D(tex_y, textureOut).r; \n"
"   yuv.g = texture2D(tex_uv, textureOut).r; \n"
"   yuv.b = texture2D(tex_uv, textureOut).g; \n"
"   rgb = yuv*cspmat+cspconst;\n"
"   gl_FragColor=vec4(rgb,1.0); \n"
"} \n";

static const char* g_nv21_fragment_shader_str =
"#version 130 \n"
"uniform sampler2D tex_y; \n"
"uniform sampler2D tex_uv; \n"
"uniform mat3 cspmat;\n"
"uniform vec3 cspconst;\n"
"in vec2 textureOut; \n"
"void main(void) \n"
"{ \n"
"   vec3 rgb; \n"
"   vec3 yuv; \n"
"   yuv.r = texture2D(tex_y, textureOut).r; \n"
"   yuv.g = texture2D(tex_uv, textureOut).g; \n"
"   yuv.b = texture2D(tex_uv, textureOut).r; \n"
"   rgb = yuv*cspmat+cspconst;\n"
"   gl_FragColor=vec4(rgb,1.0); \n"
"} \n";

static const char* g_i420_fragment_shader_str =
"#version 130 \n"
"uniform sampler2D tex_y; \n"
"uniform sampler2D tex_u; \n"
"uniform sampler2D tex_v; \n"
"uniform mat3 cspmat;\n"
"uniform vec3 cspconst;\n"
"in vec2 textureOut; \n"
"void main(void) \n"
"{ \n"
"   vec3 rgb; \n"
"   vec3 yuv; \n"
"   yuv.r = texture2D(tex_y, textureOut).r; \n"
"   yuv.g = texture2D(tex_u, textureOut).r; \n"
"   yuv.b = texture2D(tex_v, textureOut).r; \n"
"   rgb = yuv*cspmat+cspconst;\n"
"   gl_FragColor=vec4(rgb,1.0); \n"
"} \n";

static const char* g_yv12_fragment_shader_str =
"#version 130 \n"
"uniform sampler2D tex_y; \n"
"uniform sampler2D tex_u; \n"
"uniform sampler2D tex_v; \n"
"uniform mat3 cspmat;\n"
"uniform vec3 cspconst;\n"
"in vec2 textureOut; \n"
"void main(void) \n"
"{ \n"
"   vec3 rgb; \n"
"   vec3 yuv; \n"
"   yuv.r = texture2D(tex_y, textureOut).r; \n"
"   yuv.b = texture2D(tex_u, textureOut).r; \n"
"   yuv.g = texture2D(tex_v, textureOut).r; \n"
"   rgb = yuv*cspmat+cspconst;\n"
"   gl_FragColor=vec4(rgb,1.0); \n"
"} \n";

static const char* g_p010_fragment_shader_str =
"#version 130 \n"
"uniform sampler2D tex_y; \n"
"uniform sampler2D tex_uv; \n"
"uniform mat3 cspmat;\n"
"uniform vec3 cspconst;\n"
"uniform int val_ctrl; \n"
"uniform int threshold; \n"
"in vec2 textureOut; \n"
"float A = 0.15, B = 0.50, C = 0.10, D = 0.20, E = 0.02, F = 0.30; \n"
"float hable(float x) { \n"
" return (x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F)  - E/F; \n"
"} \n"
"void main(void) \n"
"{ \n"
"   vec3 rgb; \n"
"   vec3 yuv; \n"
"   float max_rgb=0.0; \n"
"   float fval = float(val_ctrl); \n"
"   yuv.r = texture2D(tex_y, textureOut).r; \n"
"   yuv.g = texture2D(tex_uv, textureOut).r; \n"
"   yuv.b = texture2D(tex_uv, textureOut).g; \n"
"   rgb = yuv*cspmat+cspconst;\n"
"   rgb.r = fval*pow(rgb.r,2.4); \n"
"   rgb.g = fval*pow(rgb.g,2.4); \n"
"   rgb.b = fval*pow(rgb.b,2.4); \n"
"   if(rgb.r>max_rgb) max_rgb = rgb.r; \n"
"   if(rgb.g>max_rgb) max_rgb = rgb.g; \n"
"   if(rgb.b>max_rgb) max_rgb = rgb.b; \n"
"   float ratio = hable(max_rgb) / hable(fval); \n"
"   if(val_ctrl <= threshold) \n"
"   { \n"
"       ratio = hable(rgb.r) / hable(fval); \n"
"   } \n"
"   rgb.r = rgb.r * ratio / max_rgb; \n"
"   if(val_ctrl <= threshold) \n"
"   { \n"
"       ratio = hable(rgb.g) / hable(fval); \n"
"   } \n"
"   rgb.g = rgb.g * ratio / max_rgb; \n"
"   if(val_ctrl <= threshold) \n"
"   { \n"
"       ratio = hable(rgb.b) / hable(fval); \n"
"   } \n"
"   rgb.b = rgb.b * ratio / max_rgb; \n"
"   gl_FragColor=vec4(rgb,1.0); \n"
"} \n";

#endif
