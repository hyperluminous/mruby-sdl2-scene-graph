#!/usr/bin/env ruby

if __FILE__ == $0
  repository, dir = 'https://github.com/mruby/mruby.git', 'tmp/mruby'

  build_args = ARGV

  Dir.mkdir 'tmp'  unless File.exist?('tmp')
  unless File.exist?(dir)
    system "git clone -b 1.4.0 #{repository} #{dir}"
  end

  exit system(%Q[cd #{dir}; MRUBY_CONFIG=#{File.expand_path __FILE__} ruby minirake #{build_args.join(' ')}])
end

MRuby::Build.new do |conf|
  if ENV['CC'].to_s.start_with? "clang"
    toolchain :clang
  else
    toolchain :gcc
  end

  conf.cc.command = ENV['CC'] || 'gcc'
  conf.enable_test
  conf.gem github: 'ecirmoa/mruby-sdl2'
  conf.gembox 'default'
  conf.gem File.expand_path(File.dirname(__FILE__))

  # conf.cc.defines += %w(INVERT_4x4_SSE)
  # conf.cc.defines += %w(INVERT_4x4_LLVM)
  conf.cc.defines += %w(MRB_UTF8_STRING)
  conf.cc.defines += %w(MRB_32BIT)
  conf.cc.defines += %w(MRB_METHOD_TABLE_INLINE MRB_METHOD_CACHE) # for 1.4.0
  conf.cc.flags = %w(-O2 -std=gnu99 -Wall -Werror-implicit-function-declaration -Wwrite-strings)
end
