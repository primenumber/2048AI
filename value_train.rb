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
    @grid[i][j].setpos(0,0)
    @grid[i][j].addstr(str)
    @grid[i][j].attroff(Curses::A_COLOR | Curses::A_BOLD)
    @grid[i][j].refresh
  end
  def move(move_direction)
    @window.setpos(5, 1)
    @window.addstr("move: " + $move_direction_str[move_direction])
    json_data = get_json("#{HOST}/hi/state/#{@session_id}/move/#{move_direction}/json")
    for i in 0...4
      for j in 0...4
        set_num(i, j, json_data["grid"][i][j])
      end
    end
    @window.setpos(6, 1)
    @window.addstr("Score: #{sprintf("%7d",json_data["score"])}")
    @window.refresh
    return json_data
  end
  def start(window)
    @window = window
    init_grid
    json_data = get_json(HOST+'/hi/start/json')
    @session_id = json_data["session_id"]
    str = ""
    for i in 0...4
      for j in 0...4
        str += ' ' if i > 0 || j > 0
        str += json_data["grid"][i][j].to_s
      end
    end
    IO.popen('./vtrain',"r+") do |io|
      io.puts str
      for line in io
        data = JSON.parse(line)
        case data["type"]
        when "move" then
          move_direction = data["direction"]
          json_data = move(move_direction)
          return json_data["score"] if json_data["over"]
          str = ""
          for i in 0...4
            for j in 0...4
              str += ' ' if i > 0 || j > 0
              str += json_data["grid"][i][j].to_s
            end
          end
          io.puts str
        end
      end
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
      grid[i][j] = window.subwin(1, 6, i + 1, j * 7 + 1)
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
$window = Array.new(3)
$grids = Array.new(3)
for i in 0...5
  $window[i] = Array.new(3)
  $grids[i] = Array.new(3)
  for j in 0...4
    $window[i][j] = $root_window.subwin(8, 29,8 * j, 30 * i)
    $window[i][j].box(?|,?-,?+)
    $grids[i][j] = init_window($window[i][j])
    $window[i][j].refresh
  end
end
loop do
  break if Curses::getstr == "quit"
  threads = Array.new
  score_sum = 0
  locker = Mutex.new
  for i in 0...5
    for j in 0...4
      threads.push(Thread.start(i, j) {|i, j|
        game = Game.new
        score = game.start($window[i][j])
        locker.synchronize do
          score_sum += score
        end
      })
    end
  end
  threads.each{|th| th.join}
  $root_window.setpos(32, 0)
  $root_window.addstr("average score: " + (score_sum / 20.0).to_s)
end
Curses::close_screen
