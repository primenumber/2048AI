# encoding : utf-8
require 'curses'
require 'open-uri'
require 'json'

HOST = 'http://ring:2048'

$move_direction_str = ["↑","→","↓","←"]

def get_json(location)
  text = ""
  OpenURI.open_uri(location) {|sio|
    text = sio.read
  }
  return JSON.parse(text)
end

class Game
  def init_grid
    @grid = Array.new(4)
    for i in 0...4
      @grid[i] = Array.new(4)
    end
    cnt = 0
    @window.children.each do |child|
      i = cnt / 4
      j = cnt % 4
      @grid[i][j] = child
      cnt += 1
    end
  end
  def set_num(i, j, num)
    if num > 0 then
      str = sprintf("%6d", num)
    else
      str = "      "
    end
    id = 0
    while num > 1
      id += 1
      num /= 2
    end
    @grid[i][j].attrset(Curses::color_pair(id) | Curses::A_BOLD) if id > 0
    @grid[i][j].setpos(1,1)
    @grid[i][j].addstr(str)
    @grid[i][j].attroff(Curses::A_COLOR | Curses::A_BOLD)
    @grid[i][j].refresh
  end
  def start(window)
    @window = window
    init_grid
    json_data = get_json(HOST+'/hi/start/json')
    @session_id = json_data["session_id"]
    loop do
      move = -1
      str = ""
      for i in 0...4
        for j in 0...4
          str += ' ' + json_data["grid"][i][j].to_s
        end
      end
      move = (`echo #{str} | ./2048ai`).to_i
      @window.setpos(19, 1)
      @window.addstr("move: " + $move_direction_str[move])
      json_data = get_json("#{HOST}/hi/state/#{@session_id}/move/#{move}/json")
      for i in 0...4
        for j in 0...4
          set_num(i, j, json_data["grid"][i][j])
        end
      end
      @window.setpos(20, 1)
      @window.addstr("Score: #{sprintf("%7d",json_data["score"])}")
      @window.refresh
      break if json_data["over"]
    end
  end
end

class Window_Wrapper
  def initialize(window = Curses::stdscr, offsety = 0, offsetx = 0)
    @window = window
    @offsety = offsety
    @offsetx = offsetx
    @children = Array.new
  end
  def subwin(height, width, y, x)
    child_raw = @window.subwin(height, width, y + @offsety, x + @offsetx)
    child = Window_Wrapper.new(child_raw, y + @offsety, x + @offsetx)
    children.push(child)
    return child
  end
  def setpos(y, x)
    @window.setpos(y, x)
  end
  def addstr(str)
    @window.addstr(str)
  end
  def refresh
    @window.refresh
  end
  def box(ch1,ch2,ch3)
    @window.box(ch1,ch2,ch3)
  end
  def attrset(arg)
    @window.attrset(arg)
  end
  def attroff(arg)
    @window.attroff(arg)
  end
  def children
    return @children
  end
end

def init_window(window)
  grid = Array.new(4)
  for i in 0...4
    grid[i] = Array.new(4)
    for j in 0...4
      grid[i][j] = window.subwin(3, 8, i * 3 + 2, j * 9 + 1)
      grid[i][j].box(?|,?-,?+)
      grid[i][j].refresh
    end
  end
  return grid
end

Curses::init_screen
Curses::start_color

Curses::init_pair(1  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(2  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(3  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(4  ,Curses::COLOR_WHITE  ,Curses::COLOR_RED    )
Curses::init_pair(5  ,Curses::COLOR_WHITE  ,Curses::COLOR_RED    )
Curses::init_pair(6  ,Curses::COLOR_WHITE  ,Curses::COLOR_RED    )
Curses::init_pair(7  ,Curses::COLOR_WHITE  ,Curses::COLOR_YELLOW )
Curses::init_pair(8  ,Curses::COLOR_WHITE  ,Curses::COLOR_YELLOW )
Curses::init_pair(9  ,Curses::COLOR_WHITE  ,Curses::COLOR_YELLOW )
Curses::init_pair(10 ,Curses::COLOR_WHITE  ,Curses::COLOR_YELLOW )
Curses::init_pair(11 ,Curses::COLOR_WHITE  ,Curses::COLOR_YELLOW )
Curses::init_pair(12 ,Curses::COLOR_WHITE  ,Curses::COLOR_BLACK  )
Curses::init_pair(13 ,Curses::COLOR_WHITE  ,Curses::COLOR_BLACK  )
Curses::init_pair(14 ,Curses::COLOR_WHITE  ,Curses::COLOR_BLACK  )
Curses::init_pair(15 ,Curses::COLOR_WHITE  ,Curses::COLOR_BLACK  )
Curses::init_pair(16 ,Curses::COLOR_WHITE  ,Curses::COLOR_BLACK  )
Curses::init_pair(17 ,Curses::COLOR_BLACK  ,Curses::COLOR_CYAN   )

$root_window = Window_Wrapper.new
$root_window.setpos(0, 0)
$root_window.addstr("great AI - 2048 AI")
$root_window.refresh
$window = Array.new(2)
$grids = Array.new(2)
for i in 0...2
  $window[i] = $root_window.subwin(22, 49, 1, 50 * i)
  $window[i].box(?|,?-,?+)
  $grids[i] = init_window($window[i])
  $window[i].refresh
end
loop do
  break if Curses::getstr == "quit"
  threads = Array.new
  for i in 0..1
    threads.push(Thread.start(i) {|i|
      game = Game.new
      game.start($window[i])
    })
  end
  threads.each{|th| th.join}
end
Curses::close_screen
