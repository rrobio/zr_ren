project('ren', 'cpp', 'c',
  default_options: [ 'warning_level=2',
                     'werror=false',
                     'cpp_std=c++17',
                   ],)

ren_sources = [
  'libs/glad/src/glad.c',

  'src/main.cpp',
  'src/shader.cpp',
  'src/material.cpp',
  'src/object.cpp',
  'src/color.cpp',
  'src/resource_manager.cpp',
  'src/log.cpp',
  'src/scene.cpp',
  'src/renderers/solid.cpp',
  'src/renderers/shadow_mapping.cpp',
  'src/renderers/material.cpp',
  'src/renderers/raytracing.cpp',

  'libs/imgui/imgui.cpp',
  'libs/imgui/imgui_draw.cpp',
  'libs/imgui/imgui_impl_glfw.cpp',
  'libs/imgui/imgui_impl_opengl3.cpp',
  'libs/imgui/imgui_widgets.cpp',
  'libs/imgui/imgui_tables.cpp',
]

ren_includes = [
  'libs/glad/include',
  'libs/stb',
  'libs/imgui',
]

ren_deps = [
  dependency('glfw3'),
  dependency('glm'),
  #dependency('glad', fallback : ['glad', 'glad_dep']),
]

executable('ren', ren_sources,
  dependencies: ren_deps,
  include_directories: ren_includes,
  install: true,
)
