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

def set_num(i, j, t_id, num)
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
  $grid[t_id][i][j].attrset(Curses::color_pair(id) | Curses::A_BOLD) if id > 0
  $grid[t_id][i][j].setpos(1,1)
  $grid[t_id][i][j].addstr(str)
  $grid[t_id][i][j].attroff(Curses::A_COLOR | Curses::A_BOLD)
  $grid[t_id][i][j].refresh
end

class Game
  def start(thread_id)
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
      Curses::setpos(19,thread_id*40)
      Curses::addstr("move: " + $move_direction_str[move])
      json_data = get_json("#{HOST}/hi/state/#{@session_id}/move/#{move}/json")
      for i in 0...4
        for j in 0...4
          set_num(i, j, thread_id, json_data["grid"][i][j])
        end
      end
      Curses::setpos(20,thread_id * 40)
      Curses::addstr("Score: #{sprintf("%7d",json_data["score"])}")
      Curses::refresh
      break if json_data["over"]
    end
  end
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

Curses::setpos(0, 0)
Curses::addstr("great AI - 2048 AI")
Curses::refresh
$grid = Array.new(2)
for i in 0...2
  $grid[i] = Array.new(4)
  for j in 0...4
    $grid[i][j] = Array.new(4)
    for k in 0...4
      $grid[i][j][k] = Curses::stdscr.subwin(3, 8, j * 3 + 2, i * 40 + k * 9 + 1)
      $grid[i][j][k].box(?|,?-,?+)
      $grid[i][j][k].refresh
    end
  end
end
loop do
  break if Curses::getch == ?q
  threads = Array.new
  for i in 0..1
    threads.push(Thread.start(i) {|i|
      game = Game.new
      game.start(i)
    })
  end
  threads.each{|th| th.join}
end
Curses::close_screen
