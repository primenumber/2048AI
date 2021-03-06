var service_url = 'http://ring:2048';
var session_id = "";
var interval = 100;
var stop = false;

function view(obj) {
  for (var i = 0; i < 4; i++) {
    for (var j = 0; j < 4; j++) {
      var num = obj.grid[i][j];
      if (num != 0) {
        $('#tb'+i+j).html("<p class='c2048'><span id='sp"+i+j+"'>"+num+"</span></p>");
        $('#sp'+i+j).css('color',tx_colors[num]);
        $('#sp'+i+j).css('font-size',tx_sizes[num]+'px');
        $('#tb'+i+j).css('background-color',colors[num]);
      } else {
        $('#tb'+i+j).html("");
        $('#tb'+i+j).css('background-color',"#FFF");
      }
    }
  }
  $('#score').html(obj.score);
}

function run() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', service_url+'/hi/start/json');
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4 && xhr.status === 200) {
      data = xhr.responseText;
      obj = JSON.parse(data);
      view(obj);
      session_id = obj.session_id;
      console.log(session_id);
      setTimeout(function() {
        ai_run(obj.grid);
      },interval);
    }
  };
  xhr.send(null);
}

function dump(grid){
  var str = "[";
  for (var i = 0; i < 4; i++) {
    if (i) str += ", ";
    str += "[";
    for (var j = 0; j < 4; j++) {
      if (j) str += ", ";
      str += grid[i][j];
    }
    str += "]";
  }
  str += "]";
  return str;
}

function rotate(grid, direction) {
  for (var i = 0; i < direction; i++) {
    var new_grid = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]];
    for (var j = 0; j < 4; j++) {
      for (var k = 0; k < 4; k++) {
        new_grid[j][k] = grid[k][4 - j - 1];
      }
    }
    grid = new_grid;
  }
  return grid;
}

function move_impl(ary) {
  var joined = false;
  var point = 0;
  var moved = false;
  for (var i = 0; i < 4; i++) {
    if (ary[i] == 0) continue;
    var hit = false;
    for (var j = i - 1; j >= 0; j--) {
      if (ary[j] == 0) continue;
      if (ary[j] == ary[i] && !joined) {
        ary[j] *= 2;
        ary[i] = 0;
        joined = true;
        moved = true;
        point += ary[j];
      } else {
        if (j+1 != i) {
          ary[j+1] = ary[i];
          ary[i] = 0;
          moved = true;
        }
        joined = false;
      }
      hit = true;
      break;
    }
    if (i != 0 && !hit) {
      ary[0] = ary[i];
      ary[i] = 0;
      moved = true;
      joined = false;
    }
  }
  return {point:point,ary:ary,movable:moved};
}

function moveUp(grid) {
  var newgrid = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]];
  var point = 0;
  var movable = false;
  for (var i = 0; i < 4; i++) {
    var ary = [0,0,0,0];
    for (var j = 0; j < 4; j++) {
      ary[j] = grid[j][i];
    }
    var ret = move_impl(ary);
    point += ret.point;
    if (ret.movable) movable = true;
    for (var j = 0; j < 4; j++) {
      newgrid[j][i] = ret.ary[j];
    }
  }
  return {point:point,grid:newgrid,movable:movable};
}

function move(grid, direction) {
  var newgrid = rotate(grid, direction);
  var ret = moveUp(newgrid);
  newgrid = rotate(ret.grid, 4 - direction - 1);
  return {point:ret.point,grid:newgrid,movable:ret.movable};
}

function zero_places(grid) {
  zeros = [];
  for (var i = 0; i < 4; i++) {
    for (var j = 0; j < 4; j++) {
      if (grid[i][j] == 0) {
        zeros.push({i:i,j:j});
      }
    }
  }
  return zeros;
}

function create_new_number(grid) {
  var zeros = zero_places(grid);
  var zero_number = zeros.length;
  var r = Math.random();
  var n = Math.floor(r * zero_number);
  grid[zeros[n].i][zeros[n].j] = Math.random() >= 0.5 ? 2 : 4;
  return grid;
}

function play(grid, direction) {
  var ret = move(grid, direction);
  create_new_number(ret.grid);
  return {point:ret.point, grid:ret.grid, movable:ret.movable};
}

function static_score(grid) {
  var values = [
    [180, -20, -20, 180],
    [-20, -80, -80, -20],
    [-20, -80, -80, -20],
    [180, -20, -20, 180]
  ];
  var sum = 0;
  var nums = [];
  for (var i = 0; i < 4; i++) {
    for (var j = 0; j < 4; j++) {
      nums.push({num:grid[i][j],i:i,j:j});
      sum += values[i][j] * grid[i][j];
    }
  }
  nums.sort(function(lhs, rhs) {
    if (lhs.num > rhs.num) {
      return -1;
    } else if (lhs.num < rhs.num) {
      return 1;
    } else {
      return 0;
    }
  });
  for (var i = 0; i < nums.length - 1; i++) {
    var smaller_num = nums[i+1].num;
    for (var j = 1; i + j < nums.length; j++) {
      if (nums[i+j] != smaller_num) break;
      var di = nums[i].i - nums[i+j].i;
      var dj = nums[i].j - nums[i+j].j;
      var d = di * di + dj * dj;
      if (d == 1) {
        sum += 60 * smaller_num;
      } else {
        sum -= 60 * smaller_num;
      }
    }
  }
  return sum;
}

function get_score(grid, direction, depth) {
  var ret = move(grid, direction);
  if (!ret.movable) return Number.NEGATIVE_INFINITY;
  if (depth == 0) return ret.point * 100 + static_score(ret.grid);
  var zeros = zero_places(ret.grid);
  var minscore = Number.POSITIVE_INFINITY;
  var numbers = [2, 4];
  var unmovable = 0;
  for (var i = 0; i < zeros.length; i++) {
    for (var j = 0; j < 2; j++) {
      var num = numbers[j];
      var gi = zeros[i].i;
      var gj = zeros[i].j;
      ret.grid[gi][gj] = num;
      var maxscore = Number.NEGATIVE_INFINITY;
      for (var k = 0; k < 4; k++) {
        maxscore = Math.max(maxscore, get_score(ret.grid, k, depth - 1));
      }
      if (maxscore != Number.NEGATIVE_INFINITY) {
        minscore = Math.min(minscore, maxscore);
      } else {
        unmovable++;
      }
    }
    ret.grid[gi][gj] = 0;
  }
  var maxn = 0;
  for (var i = 0; i < 4; i++) {
    for (var j = 0; j < 4; j++) {
      maxn = Math.max(maxn, ret.grid[i][j]);
    }
  }
  var offset = 0;
  if (unmovable == 3) {
    offset = -500 * maxn;
  } else if (unmovable == 2) {
    offset = -50 * maxn;
  }
  return minscore + ret.point * 100 + offset;
}

function calc(grid) {
  var maxp = Number.NEGATIVE_INFINITY;
  var maxi = -1;
  for (var i = 0; i < 4; i++) {
    var score = get_score(grid, i, 2);
    if (maxp <= score) {
      maxp = score;
      maxi = i;
    }
  }
  return maxi;
}

function ai_run(grid) {
  var move_dir = calc(grid);
  console.log("moveto: "+move_dir);
  var xhr = new XMLHttpRequest();
  var url = service_url+'/hi/state/'+session_id+'/move/'+move_dir+'/json';
  console.log(url);
  xhr.open('GET', url);
  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4 && xhr.status == 200) {
      var data = xhr.responseText;
      obj = JSON.parse(data);
      view(obj);
      if (!stop && !obj.over) {
        setTimeout(function() {
          ai_run(obj.grid);
        },interval);
      }
      if (obj.over) {
        alert("Game over");
      }
    }
  };
  xhr.send(null);
}
