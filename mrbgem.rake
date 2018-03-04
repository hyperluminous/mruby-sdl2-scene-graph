MRuby::Gem::Specification.new('mruby-sdl2-scene-graph') do |spec|
  spec.license = 'MIT'
  spec.authors = 'kabies'
  spec.version = '0.1.0'

  spec.add_dependency 'mruby-sdl2'

  if build.kind_of?(MRuby::CrossBuild)
    spec.mruby.linker.libraries << 'opengl32'
  else
    # spec.cc.command = 'clang'
    # spec.cc.defines += %w(INVERT_4x4_LLVM)
    # spec.cc.defines += %w(INVERT_4x4_SSE)
    spec.cc.flags << '`sdl2-config --cflags`'
    spec.linker.flags_before_libraries << '`sdl2-config --libs` -framework OpenGL'
  end
end
