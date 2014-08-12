#!/usr/bin/ruby
# encoding : utf-8
require 'curses'
require 'open-uri'
require 'json'
require 'yaml'

if File.exist?('host.yml') then
  config = YAML.load_file('host.yml')
  HOST = config["host"]
else
  HOST = 'http://2048.semantics3.com'
end

$move_direction_str = ["^",">","v","<"]

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
    @window.setpos(9, 1)
    @window.addstr("move: " + $move_direction_str[move_direction])
    json_data = get_json("#{HOST}/hi/state/#{@session_id}/move/#{move_direction}/json")
    for i in 0...4
      for j in 0...4
        set_num(i, j, json_data["grid"][i][j])
      end
    end
    @window.setpos(10, 1)
    @window.addstr("Score: #{sprintf("%7d",json_data["score"])}")
    @window.refresh
    return json_data
  end
  def start(window)
    start_time = Time.now
    @window = window
    init_grid
    json_data = get_json(HOST+'/hi/start/json')
    @session_id = json_data["session_id"]
    @window.setpos(11, 1)
    @window.addstr("Game Start!!")
    str = ""
    for i in 0...4
      for j in 0...4
        str += ' ' if i > 0 || j > 0
        str += json_data["grid"][i][j].to_s
      end
    end
    cnt = 0
    IO.popen('./2048ai 2> log.txt',"r+") do |io|
      io.puts str
      for line in io
        difftime = Time.now - start_time
        @window.setpos(8, 1)
        @window.addstr(sprintf("%6d moves, %5d seconds", cnt, difftime))
        @window.refresh
        data = JSON.parse(line)
        case data["type"]
        when "move" then
          move_direction = data["direction"]
          json_data = move(move_direction)
          break if json_data["over"]
          str = ""
          for i in 0...4
            for j in 0...4
              str += ' ' if i > 0 || j > 0
              str += json_data["grid"][i][j].to_s
            end
          end
          io.puts str
        end
        cnt += 1
      end
      @window.setpos(11, 1)
      @window.addstr("Game Over!!!")
      @window.refresh
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
      grid[i][j] = window.subwin(1, 6, i * 2 + 1, j * 7 + 1)
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
$window = $root_window.subwin(13, 29, 1, 0)
$window.box(?|,?-,?+)
$grids = init_window($window)
$window.refresh
loop do
  Curses::setpos(3, 30)
  Curses::addstr("command> ")
  line = Curses::getstr
  case line
  when "quit" then
    break
  when "go" then
    Curses::setpos(4, 30)
    Curses::addstr("                 ");
    game = Game.new
    game.start($window)
  else
    Curses::setpos(4, 30)
    Curses::addstr("command not found")
  end
end
Curses::close_screen
