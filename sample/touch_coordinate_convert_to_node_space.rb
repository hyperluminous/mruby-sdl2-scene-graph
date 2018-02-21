
puts [:inversion_mode, SceneGraph::Node::__matrix_inversion_mode__]

class Node < SceneGraph::Node
  attr_accessor :color
  def initialize
    super
    @r = SDL2::Rect.new
    @color = [0xFF,0,0,0xFF]
  end
  def draw(renderer)
    @r.x = 0 # do not self.x
    @r.y = 0 # do not self.y
    @r.w = self.w
    @r.h = self.h
    renderer.__start_draw__ self
    renderer.set_draw_color( *@color )
    renderer.fill_rect @r
    renderer.__pop_matrix__
    renderer.__pop_matrix__
  end
  def include?(wx,wy)
    lx,ly = self.convert_to_node_space(wx,wy)
    0 <= lx and lx <= self.w and 0 <= ly and ly <= self.h
  end
end

def init_sdl
  SDL2::Hints::set SDL2::Hints::SDL_HINT_RENDER_DRIVER, "opengl"
  SDL2::init
  SDL2::Video::init
  w = 640
  h = 480
  x = SDL2::Video::Window::SDL_WINDOWPOS_CENTERED
  y = SDL2::Video::Window::SDL_WINDOWPOS_CENTERED
  flag = SDL2::Video::Window::SDL_WINDOW_OPENGL
  window = SDL2::Video::Window.new "sample", x,y, w,h, flag
  renderer = SDL2::Video::Renderer.new window
  p [window.title, window.width, window.height, renderer.info.name]
  return window, renderer
end

window, renderer = init_sdl

nodes = []
40.times{|x|
  30.times{|y|
    n = Node.new
    n.x = x*16
    n.y = y*16
    n.w = 10
    n.h = 10
    nodes << n
  }
}

running = true
while running

  renderer.set_draw_color( 0,0,0,0 )
  renderer.clear
  nodes.each{|n| n.draw(renderer) }
  renderer.present

  while ev = SDL2::Input::poll()
    case ev.type
    when SDL2::Input::SDL_MOUSEMOTION
      start = Time.now
      nodes.each{|n|
        if n.include?(ev.x,ev.y)
          n.color = [0,0xff,0,0xff]
        end
      }
      elapse = (Time.now - start)*1000
      puts [:elapse, elapse, :msec]
    when SDL2::Input::SDL_QUIT
      running = false
    end
  end

  SDL2::delay 10
end
