var service_url = 'http://2048.semantics3.com';
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

function calc(grid) {
  console.log("grid_begin: "+dump(grid));
  var maxp = 0;
  var maxi = -1;
  for (var i = 0; i < 4; i++) {
    var ret = move(grid, i);
    console.log(i+': point='+ret.point+", movable: "+ret.movable);
    console.log("table: "+dump(ret.grid));
    if (!ret.movable) continue;
    if (maxp <= ret.point) {
      maxp = ret.point;
      maxi = i;
    }
  }
  console.log("grid_end: "+dump(grid));
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
