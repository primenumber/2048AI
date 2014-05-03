require 'net/http'
require 'json'
require 'uri'

def get_json(location)
  text = `curl -sL #{location}`
  return JSON.parse(text)
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
      puts "move: " + move.to_s
      json_data = get_json("http://ring:2048/hi/state/#{@session_id}/move/#{move}/json")
      for i in 0...4
        for j in 0...4
          printf("%5d ",json_data["grid"][i][j])
        end
        puts
      end
      puts "Score: #{json_data["score"]}"
      break if json_data["over"]
    end
  end
end

loop do
  line = STDIN.gets
  break if line == "quit\n"
  game = Game.new
  game.start
end
