
#include "mruby.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include <mruby/array.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sdl2.h"
#include "sdl2_render.h"
#include "sdl2_video.h"

#ifdef INVERT_4x4_SSE
#include "invert4x4_sse.h"
#define MATRIX_INVERSION_MODE "sse"
#elif INVERT_4x4_LLVM
#include "invert4x4_llvm.h"
#define MATRIX_INVERSION_MODE "llvm"
#else
#include "invert4x4_c.h"
#define MATRIX_INVERSION_MODE "c"
#endif

struct scene_graph_node {
  float scale_x;
  float scale_y;
  float anchor_x;
  float anchor_y;
  float x;
  float y;
  float w;
  float h;
  float angle;
  GLfloat matrix[16];
};

static const struct mrb_data_type mrb_scene_graph_node_type = { "Node", mrb_free };

// ---- ----

static struct scene_graph_node*
scene_graph_node_alloc(mrb_state *mrb)
{
  struct scene_graph_node *node;
  node = (struct scene_graph_node *)mrb_malloc(mrb, sizeof(struct scene_graph_node));
  node->scale_x = 1.0;
  node->scale_y = 1.0;
  node->anchor_x = 0;
  node->anchor_y = 0;
  node->x = 0;
  node->y = 0;
  node->w = 0;
  node->h = 0;
  node->angle = 0;

  for(int i=0;i<16;i++) {
    node->matrix[i] = 0;
  }

  return node;
}

static struct scene_graph_node*
node_get_ptr(mrb_state *mrb, mrb_value obj)
{
  struct scene_graph_node *node;
  node = DATA_GET_PTR(mrb, obj, &mrb_scene_graph_node_type, struct scene_graph_node);
  if (!node) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "uninitialized scene graph node");
  }
  return node;
}

// ---- ---- ---- ---- SDL2::SceneGraph::Node

static mrb_value
mrb_scene_graph_node_initialize(mrb_state *mrb, mrb_value self)
{
  struct scene_graph_node *node;
  node = (struct scene_graph_node*)DATA_PTR(self);

  /* avoid memory leaks */
  if (node) {
    mrb_free(mrb, node);
  }
  mrb_data_init(self, NULL, &mrb_scene_graph_node_type);

  node = scene_graph_node_alloc(mrb);

  mrb_data_init(self, node, &mrb_scene_graph_node_type);
  return self;
}

// ---- setter and getter

// X

static mrb_value
mrb_scene_graph_node_get_x(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->x);
}

static mrb_value
mrb_scene_graph_node_set_x(mrb_state *mrb, mrb_value self)
{
  mrb_float x;
  mrb_get_args(mrb, "f", &x);
  node_get_ptr(mrb, self)->x = x;
  return self;
}

// Y

static mrb_value
mrb_scene_graph_node_get_y(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->y);
}

static mrb_value
mrb_scene_graph_node_set_y(mrb_state *mrb, mrb_value self)
{
  mrb_float y;
  mrb_get_args(mrb, "f", &y);
  node_get_ptr(mrb, self)->y = y;
  return self;
}

// W

static mrb_value
mrb_scene_graph_node_get_w(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->w);
}

static mrb_value
mrb_scene_graph_node_set_w(mrb_state *mrb, mrb_value self)
{
  mrb_float w;
  mrb_get_args(mrb, "f", &w);
  node_get_ptr(mrb, self)->w = w;
  return self;
}

// H

static mrb_value
mrb_scene_graph_node_get_h(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->h);
}

static mrb_value
mrb_scene_graph_node_set_h(mrb_state *mrb, mrb_value self)
{
  mrb_float h;
  mrb_get_args(mrb, "f", &h);
  node_get_ptr(mrb, self)->h = h;
  return self;
}

// Anchor X

static mrb_value
mrb_scene_graph_node_get_anchor_x(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->anchor_x);
}

static mrb_value
mrb_scene_graph_node_set_anchor_x(mrb_state *mrb, mrb_value self)
{
  mrb_float anchor_x;
  mrb_get_args(mrb, "f", &anchor_x);
  node_get_ptr(mrb, self)->anchor_x = anchor_x;
  return self;
}

// Anchor Y

static mrb_value
mrb_scene_graph_node_get_anchor_y(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->anchor_y);
}

static mrb_value
mrb_scene_graph_node_set_anchor_y(mrb_state *mrb, mrb_value self)
{
  mrb_float anchor_y;
  mrb_get_args(mrb, "f", &anchor_y);
  node_get_ptr(mrb, self)->anchor_y = anchor_y;
  return self;
}


// ANGLE

static mrb_value
mrb_scene_graph_node_get_angle(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->angle);
}

static mrb_value
mrb_scene_graph_node_set_angle(mrb_state *mrb, mrb_value self)
{
  mrb_float angle;
  mrb_get_args(mrb, "f", &angle);
  node_get_ptr(mrb, self)->angle = angle;
  return self;
}

// Scale X

static mrb_value
mrb_scene_graph_node_get_scale_x(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->scale_x);
}

static mrb_value
mrb_scene_graph_node_set_scale_x(mrb_state *mrb, mrb_value self)
{
  mrb_float scale_x;
  mrb_get_args(mrb, "f", &scale_x);
  node_get_ptr(mrb, self)->scale_x = scale_x;
  return self;
}

// Scale Y

static mrb_value
mrb_scene_graph_node_get_scale_y(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, node_get_ptr(mrb, self)->scale_y);
}

static mrb_value
mrb_scene_graph_node_set_scale_y(mrb_state *mrb, mrb_value self)
{
  mrb_float scale_y;
  mrb_get_args(mrb, "f", &scale_y);
  node_get_ptr(mrb, self)->scale_y = scale_y;
  return self;
}

// Matrix

static mrb_value
mrb_scene_graph_node_get_matrix(mrb_state *mrb, mrb_value self)
{
  mrb_value result;
  mrb_int matrix_size = 16;
  mrb_int i;
  int ai;

  result = mrb_ary_new_capa(mrb, matrix_size);
  ai = mrb_gc_arena_save(mrb);

  for (i = 0; i < matrix_size; i++) {
    mrb_ary_push(mrb, result, mrb_float_value(mrb, node_get_ptr(mrb, self)->matrix[i]) );
    mrb_gc_arena_restore(mrb, ai);
  }
  return result;
}

static mrb_value
mrb_scene_graph_node_convert_to_node_space(mrb_state *mrb, mrb_value self)
{
  mrb_value result;
  mrb_int result_size = 2;
  int ai;
  float v[4] = {0};
  GLfloat m[16] = {0};
  float x, y;
  mrb_float wx,wy;

  mrb_get_args(mrb, "ff", &wx,&wy);
  result = mrb_ary_new_capa(mrb, result_size);
  ai = mrb_gc_arena_save(mrb);
  invert4x4( node_get_ptr(mrb, self)->matrix, m );

  v[0] = wx;
  v[1] = wy;
  v[2] = 0;
  v[3] = 1;

  x = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12];
  y = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13];
  // float z = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14];
  // float w = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15];

  mrb_ary_push(mrb, result, mrb_float_value(mrb, x) );
  mrb_gc_arena_restore(mrb, ai);
  mrb_ary_push(mrb, result, mrb_float_value(mrb, y) );
  mrb_gc_arena_restore(mrb, ai);

  return result;
}

static mrb_value
mrb_scene_graph_node_convert_to_world_space(mrb_state *mrb, mrb_value self)
{
  mrb_value result;
  mrb_int result_size = 2;
  int ai;
  float v[4];
  GLfloat *m;
  float x,y;
  mrb_float lx,ly;

  mrb_get_args(mrb, "ff", &lx,&ly);
  result = mrb_ary_new_capa(mrb, result_size);
  ai = mrb_gc_arena_save(mrb);
  m = node_get_ptr(mrb, self)->matrix;

  v[0] = lx;
  v[1] = ly;
  v[2] = 0;
  v[3] = 1;

  x = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12];
  y = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13];

  mrb_ary_push(mrb, result, mrb_float_value(mrb, x) );
  mrb_gc_arena_restore(mrb, ai);
  mrb_ary_push(mrb, result, mrb_float_value(mrb, y) );
  mrb_gc_arena_restore(mrb, ai);

  return result;
}

static mrb_value
mrb_scene_graph_node_matrix_inversion_mode(mrb_state *mrb, mrb_value self)
{
  return mrb_str_new_cstr(mrb, MATRIX_INVERSION_MODE);
}

// ---- ---- Renderer ---- ----

static mrb_value
mrb_sdl2_video_renderer_start_draw(mrb_state *mrb, mrb_value self)
{
  mrb_value arg;
  struct scene_graph_node *n;

  mrb_get_args(mrb, "o", &arg);
  n = (struct scene_graph_node *)DATA_PTR(arg);

  glPushMatrix();
    glTranslatef( n->x, n->y, 0 );
    glRotatef( n->angle, (GLfloat)0.0, (GLfloat)0.0, (GLfloat)1.0);
    glScalef( n->scale_x, n->scale_y, 1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, n->matrix);
    glPushMatrix();
      glTranslatef( - n->w * n->anchor_x, - n->h * n->anchor_y, 0);
      // Call draw function
    // glPopMatrix();
    // Render children
  // glPopMatrix();
  return self;
}


static mrb_value
mrb_sdl2_video_renderer_start_draw_with_saved_matrix(mrb_state *mrb, mrb_value self)
{
  mrb_value arg;
  struct scene_graph_node *n;

  mrb_get_args(mrb, "o", &arg);
  n = (struct scene_graph_node *)DATA_PTR(arg);

  glPushMatrix();
    glMatrixMode(GL_MODELVIEW_MATRIX);
    glLoadIdentity();
    glLoadMatrixf(n->matrix);
    glPushMatrix();
      glTranslatef( - n->w * n->anchor_x, - n->h * n->anchor_y, 0);

  return self;
}

static mrb_value
mrb_sdl2_video_renderer_pop_matrix(mrb_state *mrb, mrb_value self)
{
  glPopMatrix();
  return self;
}

// ---- ---- ---- ----

void
mrb_mruby_sdl2_scene_graph_gem_init(mrb_state *mrb)
{
  struct RClass *mod_Video;
  struct RClass *class_Renderer;
  struct RClass *mod_SceneGraph;
  struct RClass *class_Node;

  mod_SceneGraph = mrb_define_module(mrb, "SceneGraph");
  class_Node  = mrb_define_class_under(mrb, mod_SceneGraph, "Node", mrb->object_class);

  mod_Video      = mrb_module_get_under   (mrb, mod_SDL2,  "Video");
  class_Renderer = mrb_class_get_under    (mrb, mod_Video, "Renderer");

  MRB_SET_INSTANCE_TT(class_Node, MRB_TT_DATA);

  mrb_define_class_method(mrb, class_Node, "__matrix_inversion_mode__", mrb_scene_graph_node_matrix_inversion_mode, MRB_ARGS_NONE());

  mrb_define_method(mrb, class_Node, "initialize", mrb_scene_graph_node_initialize, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "x", mrb_scene_graph_node_get_x, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "x=", mrb_scene_graph_node_set_x, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "y", mrb_scene_graph_node_get_y, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "y=", mrb_scene_graph_node_set_y, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "w", mrb_scene_graph_node_get_w, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "w=", mrb_scene_graph_node_set_w, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "h", mrb_scene_graph_node_get_h, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "h=", mrb_scene_graph_node_set_h, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "anchor_x", mrb_scene_graph_node_get_anchor_x, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "anchor_x=", mrb_scene_graph_node_set_anchor_x, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "anchor_y", mrb_scene_graph_node_get_anchor_y, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "anchor_y=", mrb_scene_graph_node_set_anchor_y, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "angle", mrb_scene_graph_node_get_angle, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "angle=", mrb_scene_graph_node_set_angle, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "scale_x", mrb_scene_graph_node_get_scale_x, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "scale_x=", mrb_scene_graph_node_set_scale_x, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "scale_y", mrb_scene_graph_node_get_scale_y, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "scale_y=", mrb_scene_graph_node_set_scale_y, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Node, "matrix", mrb_scene_graph_node_get_matrix, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Node, "convert_to_node_space", mrb_scene_graph_node_convert_to_node_space, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, class_Node, "convert_to_world_space", mrb_scene_graph_node_convert_to_world_space, MRB_ARGS_REQ(2));

  mrb_define_method(mrb, class_Renderer, "__start_draw__", mrb_sdl2_video_renderer_start_draw, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Renderer, "__pop_matrix__", mrb_sdl2_video_renderer_pop_matrix, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Renderer, "__start_draw_with_saved_matrix__", mrb_sdl2_video_renderer_start_draw_with_saved_matrix, MRB_ARGS_REQ(1));
}

void
mrb_mruby_sdl2_scene_graph_gem_final(mrb_state *mrb)
{
}
