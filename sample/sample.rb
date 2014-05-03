require 'json'
require 'open-uri'

HOSTNAME = 'http://ring:2048'
START_URI = HOSTNAME + '/hi/start/json'

# 移動方向を表す定数
module Direction
  UP = 0
  RIGHT = 1
  DOWN = 2
  LEFT = 3
end

# 指定したURLからjsonデータを取ってきて、パースしてオブジェクトにして返す
def get_json_object(uri)
  OpenURI.open_uri(uri) do |string_io|
    json = string_io.read
    return JSON.parse(json)
  end
end

# セッションIDと移動方向からURIを生成
def get_next_uri(session_id, move_direction)
  return HOSTNAME + '/hi/state/' + session_id + '/move/' + move_direction.to_s + '/json'
end

# 盤面を人間に分かるように出力する
def output_grid(grid)
  for i in 0..3
    for j in 0..3
      printf("%7d" ,grid[i][j])
    end
    puts
    puts
  end
end

# 盤面をdirectionの数だけ半時計回りに回転させる
def rotate(grid, direction)
  direction.times do
    # 退避先の配列を用意
    tmp_ary = Array.new(4)
    for i in 0..3
      tmp_ary[i] = Array.new(4)
      for j in 0..3
        tmp_ary[i][j] = grid[j][3-i]
      end
    end
    grid = tmp_ary
  end
  return grid
end

# 盤面が上に動かせるか判定する
def movable_up?(grid)
  for i in 0..3
    # それぞれの列について動かせるかどうか判定する
    for j in 0..2
      # 上のタイルが開いているタイルがあったら上に動かせる
      return true if grid[j][i] == 0 && grid[j+1][i] != 0
      # 同じ数字のタイルが縦に並んでいたら上に動かせる
      return true if grid[j][i] == grid[j+1][i] && grid[j][i] != 0
    end
  end
  return false
end

# 盤面が特定の方向に動かせるか判定する
# rotation := 0:上, 1:右, 2:下, 3:左
def movable?(grid, rotation)
  # 方向の分だけ半時計回りに回転させる
  rotate_grid = rotate(grid, rotation)
  return movable_up?(rotate_grid)
end

# 動くべき方向を計算する
def calculate(grid)
  # ここがAIの本体
  # 下の内容は「動かせる方向の中からランダムに選ぶ」AI
  movable_directions = []
  for i in 0..3
    movable_directions.push(i) if movable?(grid, i)
  end
  # sampleメソッドは配列の中からランダムに一つ要素を取ってくる
  return movable_directions.sample
end

begin_data = get_json_object(START_URI)
session_id = begin_data["session_id"]
grid = begin_data["grid"]
output_grid(grid)
puts("Score: " + begin_data["score"].to_s)
loop do
  direction = calculate(grid)
  uri = get_next_uri(session_id, direction)
  data = get_json_object(uri)
  grid = data["grid"]
  output_grid(grid)
  puts("Score: " + data["score"].to_s)
  break if data["over"]
  # 動作する様子が分かるように0.2秒毎に1手動かす　本番では不要
  sleep(0.2)
end
puts "Game over!!!"
