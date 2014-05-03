require 'curses'
require 'net/http'
require 'json'
require 'uri'
def get_json(location)
  text = `curl -sL #{location}`
  return JSON.parse(text)
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
  $grid[i][j].attrset(Curses::color_pair(id) | Curses::A_BOLD) if id > 0
  $grid[i][j].setpos(2,1)
  $grid[i][j].addstr(str)
  $grid[i][j].attroff(Curses::A_COLOR | Curses::A_BOLD)
  $grid[i][j].refresh
end

class Game
  def start
    json_data = get_json('http://ring:2048/hi/start/json')
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
      Curses::setpos(19,0)
      Curses::addstr("move: " + move.to_s)
      json_data = get_json("http://ring:2048/hi/state/#{@session_id}/move/#{move}/json")
      for i in 0...4
        for j in 0...4
          set_num(i, j, json_data["grid"][i][j])
        end
      end
      Curses::setpos(20,0)
      Curses::addstr("Score: #{json_data["score"]}")
      Curses::refresh
      break if json_data["over"]
    end
  end
end

Curses::init_screen

Curses::init_pair(1  ,Curses::COLOR_YELLOW ,Curses::COLOR_GREEN  )
Curses::init_pair(2  ,Curses::COLOR_RED    ,Curses::COLOR_BLUE   )
Curses::init_pair(3  ,Curses::COLOR_BLUE   ,Curses::COLOR_WHITE  )
Curses::init_pair(4  ,Curses::COLOR_CYAN   ,Curses::COLOR_WHITE  )
Curses::init_pair(5  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(6  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(7  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(8  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(9  ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(10 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(11 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(12 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(13 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(14 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(15 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(16 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )
Curses::init_pair(17 ,Curses::COLOR_BLACK  ,Curses::COLOR_WHITE  )

Curses::setpos(0, 0)
Curses::addstr("great AI - 2048 AI")
Curses::refresh
$grid = Array.new(4)
for i in 0...4
  $grid[i] = Array.new
  for j in 0...4
    $grid[i][j] = Curses::stdscr.subwin(4, 8, i * 4 + 2, j * 9 + 1)
    $grid[i][j].box(?|,?-,?+)
    $grid[i][j].refresh
  end
end
loop do
  break if Curses::getch == ?q
  game = Game.new
  game.start
end
Curses::close_screen
