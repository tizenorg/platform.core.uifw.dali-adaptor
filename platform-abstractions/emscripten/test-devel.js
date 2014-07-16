/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

var dali;

var stage = dali.getStage();

var fileInput = document.getElementById("fileInput");
var fileDisplayArea = document.getElementById("fileDisplayArea");

var BAKED = true;
var NOT_BAKED = false;

var image;
var anim;

function addImageActor(file) {
  "use strict";

  if (!file) {
    return;
  }
  var reader = new FileReader();

  reader.onload = function(e) {

    var i = new dali.ImageActor();

    image = i;

    var typedBuffer = new Uint8Array(reader.result); // convert ArrayBuffer into a typed array?

    //
    // Setting image isn't property based as browser cannot load from a file
    //
    i.setImage(typedBuffer);
    // i.image         = {"filename":"dali-logo.png"};

    i.visible = true;
    i.anchorPoint = [0.5, 0.5, 0.5];
    i.parentOrigin = [0.5, 0.5, 0.5];
    i.position = [0, 0, -10];
    i.size = [10, 10, 1];
    image = i;

    dali.getStage().add(i);


    var a = new dali.Animation();
    a.setDuration(6);
    a.setLooping(true);
    dali.animateTo(a, i, "position", [300 * Math.random() - 150, 300 * Math.random() - 150, 0],
      "linear", 0, 3);
    dali.animateTo(a, i, "position", [0, 0, 0], "linear", 3, 3);
    dali.animateTo(a, i, "rotation", [0, 0, 1, 720 * Math.random() - 360], "linear", 0, 3);
    dali.animateTo(a, i, "rotation", [0, 0, 1, 0], "linear", 3, 3);

    a.play();
    //    a.delete(); // delete the animtion object but a.Play() keeps it running in Dali
    anim = a;
  };

  reader.readAsArrayBuffer(file);

}

fileInput.addEventListener('change', function(e) {
  "use strict";

  var file = fileInput.files[0];
  console.log(file);

  addImageActor(file);
});


function writeChar(buffer, c, x, y) {
  "use strict";

  var ctx = buffer.getContext("2d");

  ctx.font = "Normal 10px Arial";

  ctx.fillText(c, x, y);

}

//
// Class Cursors
//

// var STATE = {
//   NOSELECTION: {
//     value: 0,
//     name: "NoSelection"
//   },
//   SELECTED: {
//     value: 1,
//     name: "Selected"
//   },
//   LARGE: {
//     value: 2,
//     name: "Large",
//     code: "L"
//   }
// };


function Cursors() {
  this.selectedActor = null;

  this.resizers = []; // array of 8 small square resizers
  this.lines = []; // array of 4 bounding box lines
  this.lineColor = [0.5, 0.5, 0.5, 1];
  this.resizerColor = [0, 0, 0, 1];
  this.attached = false;
  this.firstTouch = false;
  this.mouseIsDown = 0; //  1 == down
  this.mouseOffsetX = 0;
  this.mouseOffsetY = 0;

  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;

  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  for (i = 0; i < 4; i++) { // lines
    this.lines.push(dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0));
  }

  for (i = 0; i < 8; i++) { // squares
    this.resizers.push(dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1));
    this.resizers[i].size = [3, 3, 1];
  }

  this.__showAll(false);

  for (i = 0; i < 4; i++) {
    stage.add(this.lines[i]);
  }

  for (i = 0; i < 8; i++) {
    stage.add(this.resizers[i]);
  }
}

Cursors.prototype.onTouched = function(actor) {
  // on touched runs for all moves,ups,downs... in c++ we have access to mode parameter
  // here we have to track it
  if (this.firstTouch && this.mouseIsDown) {
    this.selectActor(actor);
    console.log("select " + actor.$$.ptr);
    this.firstTouch = false;
  }
};

Cursors.prototype.selectActor = function(actor) {
  this.__highlightActor(actor);
};

Cursors.prototype.deselectActor = function() {
  if (this.selectedActor) {
    console.log("unselect " + this.selectedActor.$$.ptr);
    this.__unHighlight();
    this.selectedActor.delete();
    this.selectedActor = null;
  }
};

Cursors.prototype.mouseDown = function(canvas, e) {
  this.mouseIsDown = 1;
  var rect = canvas.getBoundingClientRect(); // in window
  var x = e.clientX - rect.left; // from window
  var y = e.clientY - (rect.bottom - rect.height);
  dali.sendTouchEvent(x, y, 0); // 0 == down, 1 == up, 2 == motion
  this.mouseOffsetX = x;
  this.mouseOffsetY = y;
  this.firstTouch = true;
  if (this.selectedActor) {
    var p = this.selectedActor.position;
    var s = this.selectedActor.size;
    var w2 = s[0] / 2;
    var h2 = s[1] / 2;
    // click outside and we deselect actor
    if (x < (p[0] - w2) || x > (p[0] + w2) || y < (p[1] - h2) || y > (p[1] + h2)) {
      this.deselectActor();
    }
  }
  console.log("mouse down " + this.mouseIsDown);
}

Cursors.prototype.mouseUp = function(canvas, e) {
  this.mouseIsDown = 0;
  var rect = canvas.getBoundingClientRect(); // in window
  var x = e.clientX - rect.left; // from window
  var y = e.clientY - (rect.bottom - rect.height);
  dali.sendTouchEvent(x, y, 1); // 0 == down, 1 == up, 2 == motion
  this.mouseOffsetX = x;
  this.mouseOffsetY = y;
}

Cursors.prototype.mouseMove = function(canvas, e) {
  var rect = canvas.getBoundingClientRect(); // in window
  var x = e.clientX - rect.left; // from window
  var y = e.clientY - (rect.bottom - rect.height);

  dali.sendTouchEvent(x, y, 2); // 0 == down, 1 == up, 2 == motion

  var dx = (x - this.mouseOffsetX);
  var dy = (y - this.mouseOffsetY);

  if (cursors.selectedActor && this.mouseIsDown) {
    console.log("mouse active move " + this.mouseIsDown + " " + this.shiftKey);

    if (this.shiftKey) {
      var p = this.selectedActor.rotation;
      // console.log(touchedActor.position);
      // console.log([p[0] + dx, p[1] + dy, p[2]]);
      this.selectedActor.rotation = [p[0] + dx, p[1] + dy, p[2]];
    } else {
      var p = this.selectedActor.position;
      // console.log(touchedActor.position);
      // console.log([p[0] + dx, p[1] + dy, p[2]]);
      this.selectedActor.position = [p[0] + dx, p[1] + dy, p[2]];
      //touchedActor.position = [p[0] + 5, p[1] + 5, p[2]];
      // console.log(touchedActor.position);
      // console.log(dx + "," + dy);
    }
  }

  this.mouseOffsetX = x;
  this.mouseOffsetY = y;
}

Cursors.prototype.onKeyDown = function(e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

Cursors.prototype.onKeyUp = function(e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

Cursors.prototype.__showAll = function(show) {
  for (i = 0; i < 4; i++) {
    this.lines[0].visible = show;
  }

  for (i = 0; i < 8; i++) {
    this.resizers[0].visible = show;
  }
};

Cursors.prototype.__attach = function(actor) {
  if (actor && !this.attached) {
    for (i = 0; i < 4; i++) {
      actor.add(this.lines[i]);
    }

    for (i = 0; i < 8; i++) {
      actor.add(this.resizers[i]);
    }
    this.attached = true;
  }
};

Cursors.prototype.__unattach = function(actor) {
  if (actor && this.attached) {
    for (i = 0; i < 4; i++) {
      actor.remove(this.lines[i]);
    }

    for (i = 0; i < 8; i++) {
      actor.remove(this.resizers[i]);
    }
    this.attached = false;
  }
};

Cursors.prototype.__highlightActor = function(actor) {
  this.__unattach(this.selectedActor);

  this.selectedActor = actor;

  var s = actor.size;
  var p = actor.position;
  var halfw = s[0] / 2;
  var halfh = s[1] / 2;

  this.__attach(actor);

  this.lines[0].position = [0 - halfw, 0, 0];
  this.lines[0].size = [1, halfh * 2, 1];

  this.lines[1].position = [0, 0 - halfh, 0];
  this.lines[1].size = [halfw * 2, 1, 1];

  this.lines[2].position = [0 + halfw, 0, 0];
  this.lines[2].size = [1, halfh * 2, 1];

  this.lines[3].position = [0, 0 + halfh, 0];
  this.lines[3].size = [halfw * 2, 1, 1];

  //   this.lines[0].position = [p[0] - halfw, p[1], 0];
  // this.lines[0].size = [1, halfh * 2, 1];

  // this.lines[1].position = [p[0], p[1] - halfh, 0];
  // this.lines[1].size = [halfw * 2, 1, 1];

  // this.lines[2].position = [p[0] + halfw, p[1], 0];
  // this.lines[2].size = [1, halfh * 2, 1];

  // this.lines[3].position = [p[0], p[1] + halfh, 0];
  // this.lines[3].size = [halfw * 2, 1, 1];

  this.__showAll(true);

  return this.color + ' ' + this.type + ' apple';
};

Cursors.prototype.__unHighlight = function() {
  this.__unattach(this.selectedActor);
  this.__showAll(false);
}

//
// Global
//

var cursors = new Cursors();
var canvas = document.getElementById("canvas");

function mouseDown(e) {
  cursors.mouseDown(canvas, e);
}

function mouseUp(e) {
  cursors.mouseUp(canvas, e);
}

function mouseMove(e) {
  cursors.mouseMove(canvas, e);
}


document.getElementById("canvas").onmousemove = mouseMove;
document.getElementById("canvas").onmousedown = mouseDown;
document.getElementById("canvas").onmouseup = mouseUp;

document.getElementById("canvas").onkeydown = cursors.onKeyDown;
document.getElementById("canvas").onkeyup = cursors.onKeyUp;

//
//
//

function runTests() {
  var stage = dali.getStage();

  var actor = new dali.TextActor();
  actor.parentOrigin = [0.5, 0.5, 0.5];
  actor.anchorPoint = [0.5, 0.5, 0.5];
  actor.text = "actor";
  actor.size = [100, 100, 1];
  actor.position = [0, 0, 10];
  stage.add(actor);

  var hello = new dali.TextActor();
  hello.text = "hello";
  actor.add(hello);

  var hellochild = new dali.TextActor();
  hellochild.text = "hello-child";
  hello.add(hellochild);

  var hellochild2 = new dali.TextActor();
  hellochild2.text = "hello-child2";
  hello.add(hellochild2);

  var hellochildchild = new dali.TextActor();
  hellochildchild.text = "hello-child-child1";
  hellochildchild.name = "hello-child-child1";
  hellochild.add(hellochildchild);


  var depthfirst = actor.findAllChildren();

  assert(actor.getChildCount() === 1);
  assert(actor.getChildAt(0).text === "hello");
  assert(actor.findChildByName("hello-child-child1").text = "hello-child-child1");
  assert(hello.getParent().text === "actor");
  assert(depthfirst[depthfirst.length - 1].text === "hello-child2");

  var directChildren = actor.directChildren();

  assert(directChildren.length === 1);
  assert(directChildren[0].getId() == hello.getId());

  actor.position = [100, 100, 0];

  var root = stage.getRootLayer();

  actor.remove(hello);
  assert(actor.getChildCount() === 0);

  actor.add(hello);
  assert(actor.getChildCount() === 1);

  var rootLayerCount = root.getChildCount();
  stage.remove(actor); // check these don't assert
  assert(root.getChildCount() === rootLayerCount - 1);

  stage.add(actor);
  assert(root.getChildCount() === rootLayerCount);

  assert(root.findChildByName("none") === null);

  function onTouched(actor) {
    // console.log("touched " + actor.$$.ptr + " " + actor.position);
    cursors.onTouched(actor);
  }

  actor.connectSignal("touched", onTouched);

} // runTests

function runAnimation(actor) {
  var a = new dali.Animation();
  a.setDuration(3);
  dali.animateTo(a, actor, "position", [20, 0, 0], "linear", 0, 3);
  a.play();

  function checkNewPos() {
    assert(actor.position = [120, 100, 0]);
  }

  function checkPos() {
    assert(actor.position = [20, 0, 0]);
    actor.position = [100, 100, 0];

    var a = new dali.Animation();
    a.setDuration(3);
    dali.animateBy(a, actor, "position", [20, 0, 0], "linear", 0, 3);
    a.play();

    window.setTimeout(checkNewPos, 4000);
  }


  window.setTimeout(checkPos, 4000);
}

runTests();
