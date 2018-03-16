MRuby::Gem::Specification.new('mruby-sdl2-scene-graph') do |spec|
  spec.license = 'MIT'
  spec.authors = 'kabies'
  spec.version = '0.1.0'

  spec.add_dependency 'mruby-sdl2'

  if build.kind_of?(MRuby::CrossBuild) && %w(x86_64-w64-mingw32 i686-w64-mingw32).include?(build.host_target)
    spec.linker.libraries << 'opengl32'
  elsif not build.kind_of?(MRuby::CrossBuild) and RUBY_PLATFORM.include? "darwin"
    spec.linker.flags << '-framework OpenGL'
  else
    spec.cc.flags << '`sdl2-config --cflags`'
    spec.linker.libraries << 'GL'
  end
end
