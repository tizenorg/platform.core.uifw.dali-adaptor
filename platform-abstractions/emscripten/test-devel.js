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

    dali.getStage().addActor(i);


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

document.onmousemove = mouseMove;
document.onmousedown = mouseDown;
document.onmouseup = mouseUp;
var mouseIsDown = 0; // 1 == up

function mouseDown(e) {
  mouseIsDown = 1;
  var x = e.clientX; // from window
  var y = e.clientY;
  var canvas = document.getElementById("canvas");
  var rect = canvas.getBoundingClientRect(); // in window
  dali.sendTouchEvent(x - rect.left, y - (rect.bottom - rect.height), mouseIsDown);
}

function mouseUp(e) {
  mouseIsDown = 0;
  var x = e.clientX;
  var y = e.clientY;
  var canvas = document.getElementById("canvas");
  var rect = canvas.getBoundingClientRect(); // in window
  dali.sendTouchEvent(x - rect.left, y - (rect.bottom - rect.height), mouseIsDown);
}

function mouseMove(e) {
  var x = e.clientX;
  var y = e.clientY;
  var canvas = document.getElementById("canvas");
  var rect = canvas.getBoundingClientRect(); // in window
  dali.sendTouchEvent(x - rect.left, y - (rect.bottom - rect.height), mouseIsDown);
}

document.getElementById("canvas").onmousemove = mouseMove;
document.getElementById("canvas").onmousedown = mouseDown;
document.getElementById("canvas").onmouseup = mouseUp;

//
//
//

var actor = new dali.TextActor();
actor.parentOrigin = [0.5, 0.5, 0.5];
actor.anchorPoint = [0.5, 0.5, 0.5];
actor.text = "actor";
actor.size = [100, 100, 1];
actor.position = [0, 0, 10];
dali.getStage().addActor(actor);

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


actor.position = [100, 100, 0];

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

var touched = null;

function onTouched(actor) {
  console.log("touched" + actor.position);
  touched = actor;
}

actor.connectSignal("touched", onTouched);
