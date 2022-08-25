project('ren', 'cpp', 'c',
  default_options: [ 'warning_level=2',
                     'werror=false',
                     'cpp_std=gnu++2a',
                   ],)

ren_sources = [
  'libs/glad/src/glad.c',

  'src/main.cpp',
  'src/shader.cpp',
  'src/object.cpp',
  'src/resource_manager.cpp',
  'src/log.cpp',
  'src/scene.cpp',
  'src/renderers/solid.cpp',
  'src/renderers/shadow_mapping.cpp',
  'src/renderers/material.cpp',

  'libs/imgui/imgui.cpp',
  'libs/imgui/imgui_draw.cpp',
  'libs/imgui/imgui_impl_glfw.cpp',
  'libs/imgui/imgui_impl_opengl3.cpp',
  'libs/imgui/imgui_widgets.cpp',
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
