/*global  */
/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

var dali;

var stage = dali.getStage();

var fileInput = document.getElementById("fileInput");
var fileDisplayArea = document.getElementById("fileDisplayArea");
var textInput = document.getElementById("textInput");

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
    i.size = [100, 100, 1];
    image = i;

    i.connectSignal("touched", function(actor) {
      eventHandler.onTouched(actor);
    });

    dali.getStage().add(i);

  };

  reader.readAsArrayBuffer(file);
}

fileInput.addEventListener('change', function(e) {
  "use strict";

  var file = fileInput.files[0];
  console.log(file);

  addImageActor(file);
});

textInput.addEventListener('change', function(e) {
  "use strict";

  var text = textInput.value;
  console.log(text);

  addTextImage(text);

});


function writeChar(buffer, c, x, y) {
  "use strict";

  var ctx = buffer.getContext("2d");

  ctx.font = "Normal 10px Arial";

  ctx.fillText(c, x, y);

}

//
// Class EventHandler
//
// Manages mouse state changes, passes on to dali
// Manages dali's ontouched as a way to select an actor
// Use:
//   From browser window or canvas events call mouseDown/Move/Up
//   From every actor that can be selected set "touched" signal to OnTouched
//   register app logic to the handler events ie register("MouseMove", funcApp)
//
var EventState = {
  STOPPED: {
    value: 0,
    name: "Stopped"
  },
  TRIGGER_START: {
    value: 1,
    name: "TriggerStart"
  },
  ACTIVE: {
    value: 2,
    name: "Active"
  },
};

function EventHandler() {
  // public
  this.mouse_x = 0;
  this.mouse_y = 0;
  this.mouse_buttons = 0;
  this.mouseDownPosition = [0, 0, 0];
  this.drag_dx = 0;
  this.drag_dy = 0;
  this.touchedActor = null; // set if actor is touched (left click)
  this.touchedActorPosition = [0, 0, 0];
  this.touchedActorSize = [0, 0, 0];
  //private
  this.firstTouch = false;
  // this.selectedActor = null;
  // this.selectedActorPosition = [0, 0, 0];
  this.mouseIsDown = 0;
  this.mouseDragState = EventState.STOPPED;
  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;
  // handlers; use register("MouseDragStart", func); where for func(eventHandler)
  this.handlersMouseDragStart = []; // valid; touchedActor(can be null), touchedActorPosition
  this.handlersMouseDragStop = []; // valid; touchedActor(can be null), touchedActorPosition
  this.handlersMouseDrag = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
  this.handlersMouseMove = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
  this.handlersTouchSelect = []; // valid; touchedActor, touchedActorPosition
  this.handlersTouchDeSelect = []; // valid; touchedActor, touchedActorPosition
}

EventHandler.prototype.register = function(name, handler) {
  var handlers = this["handlers" + name];
  if (!(handler in handlers)) {
    handlers.push(handler);
  }
};

EventHandler.prototype.unRegister = function(name, handler) {
  var handlers = this["handlers" + name];
  var index = handlers.index(handler);
  if (index >= 0) {
    handlers.splice(index, 1);
  }
};

EventHandler.prototype.onTouched = function(actor) {
  // on touched seems to run for all moves,ups,downs... in c++ we have access to mode parameter
  // here we have to track it
  // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor);
  if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
    this.touchedActor = actor;
    this.touchedActorPosition = actor.position;
    this.touchedActorSize = actor.size;
    this.handleEvent(this.handlersTouchSelect);
    this.firstTouch = false;
  }
};

EventHandler.prototype.resizerOnTouched = function(actor) {
  // on touched seems to run for all moves,ups,downs... in c++ we have access to mode parameter
  // here we have to track it
  // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor);
  if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
    this.touchedActor = actor;
    this.touchedActorPosition = actor.position;
    this.touchedActorSize = actor.size;
    this.firstTouch = false;
    this.handleEvent(this.handlersTouchSelect);
  }
};

EventHandler.prototype.mouseDown = function(canvas, e) {
  this.mouseIsDown = 1;
  var rect = canvas.getBoundingClientRect(); // in window
  this.mouse_x = e.clientX - rect.left; // from window
  this.mouse_y = e.clientY - (rect.bottom - rect.height);
  this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
  dali.sendMouseEvent(this.mouse_x, this.mouse_y, 0); // 0 == down, 1 == up, 2 == motion
  // this.mouseDownX = x;
  // this.mouseDownY = y;
  this.mouseDownPosition = [this.mouse_x, this.mouse_y, 0];
  // if (this.mouseDragState === EventState.STOPPED) {
  this.firstTouch = true;
  this.mouseDragState = EventState.TRIGGER_START;
  // }
  // console.log("mouse down " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor + " " + this.firstTouch);
  if (this.touchedActor) {
    var p = this.touchedActor.position;
    var s = this.touchedActor.size;
    var w2 = s[0] / 2;
    var h2 = s[1] / 2;
    // click outside and we deselect actor
    var screenToLocal = this.touchedActor.__screenToLocal(this.mouse_x, this.mouse_y);
    var localX = screenToLocal.get(0);
    var localY = screenToLocal.get(1);
    var ok = screenToLocal.get(2) === 1.0;
    if (ok && (localX < 0 || localX > s[0] || localY < 0 || localY > s[1])) {
      // if (this.mouse_x < (p[0] - w2) || this.mouse_x > (p[0] + w2) ||
      //   this.mouse_y < (p[1] - h2) || this.mouse_y > (p[1] + h2)) {
      // if (this.mouse_buttons === 0) {
      //   this.handleEvent(this.handlersTouchDeSelect);
      // }
      if (this.mouseDragState === EventState.ACTIVE) {
        this.handleEvent(this.handlersMouseDragStop);
        this.mouseDragState = EventState.STOPPED;
        this.touchedActorPosition = this.touchedActor.position;
        this.touchedActorSize = this.touchedActor.size;
      }
      if (this.mouse_buttons === 0) {
        this.handleEvent(this.handlersTouchDeSelect);
        this.touchedActor = null;
      }
    }
    // console.log("local: " + localX + "," + localX + "outside? " + (localX < 0 || localX > s[0] || localY < 0 || localY > s[1]).toString() + " selected?" + this.touchedActor);
  }
};

EventHandler.prototype.mouseMove = function(canvas, e) {
  var rect = canvas.getBoundingClientRect(); // in window
  this.mouse_x = e.clientX - rect.left; // from window
  this.mouse_y = e.clientY - (rect.bottom - rect.height);
  dali.sendMouseEvent(this.mouse_x, this.mouse_y, 2); // 0 == down, 1 == up, 2 == motion
  this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
  this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);

  // do this before setting mouseDragState so we actually drag on 2nd mouseMove where mouseIsDown
  if (this.mouseIsDown && this.mouseDragState === EventState.ACTIVE) {
    //console.log("mouse active move " + this.mouseIsDown + " " + this.shiftKey);
    this.handleEvent(this.handlersMouseDrag);
  }

  if (this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
    this.handleEvent(this.handlersMouseDragStart);
    this.mouseDragState = EventState.ACTIVE;
  }

  this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
  this.handleEvent(this.handlersMouseMove);

};

EventHandler.prototype.mouseUp = function(canvas, e) {
  this.mouseIsDown = 0;
  var rect = canvas.getBoundingClientRect(); // in window
  this.mouse_x = e.clientX - rect.left; // from window
  this.mouse_y = e.clientY - (rect.bottom - rect.height);
  this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
  this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
  dali.sendMouseEvent(this.mouse_x, this.mouse_y, 1); // 0 == down, 1 == up, 2 == motion
  if (this.mouseDragState == EventState.ACTIVE) {
    this.handleEvent(this.handlersMouseDragStop);
    // if (this.touchedActor) {
    //   this.handleEvent(this.handlersTouchDeSelect);
    //   this.touchedActor = null;
    // }
    this.mouseDragState = EventState.STOPPED;
  }

  this.mouse_buttons = e.button;
  // this.handleEvent(this.handlersMouseEvent);
};

EventHandler.prototype.mouseWheel = function(canvas, e) {
  // multiples of +120 for up, -120 for down
  var clicks = e.wheelDelta / 120;

  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  var task = taskList.getTask(0);
  var cameraActor = task.getCameraActor();

  var fov = cameraActor.getFieldOfView();
  cameraActor.setFieldOfView(fov + (clicks / 180.0 * Math.PI));

  taskList.delete();
  task.delete();
  cameraActor.delete();
  // console.log(e.wheelDelta);
}

EventHandler.prototype.handleEvent = function(handlers) {
  for (var i = 0, len = handlers.length; i < len; i++) {
    var handler = handlers[i];
    handler(this);
  }
};

EventHandler.prototype.keyDown = function(canvas, e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

EventHandler.prototype.keyUp = function(canvas, e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

//
// Actor list helpers
//
var ActorList = {
  doAll: function(actors, func) {
    for (var i = 0, len = actors.length; i < len; i++) {
      func(actors[i]);
    }
  },
  show: function(actors, visible) {
    this.doAll(actors, function(actor) {
      actor.visible = visible;
    });
  },
  delete: function(actors, visible) {
    this.doAll(actors, function(actor) {
      actor.delete = visible;
    });
  },
  add: function(actors, actor) {
    this.doAll(actors, function(a) {
      actor.add(a);
    });
  },
  remove: function(actors, actor) {
    this.doAll(actors, function(a) {
      actor.remove(a);
    });
  },
  contains: function(actors, actor) {
    if (!actor) {
      return false;
    }
    for (var i = 0, len = actors.length; i < len; i++) {
      if (actors[i].getId() == actor.getId()) {
        return true;
      }
      return false;
    }
  }
};


//
// Depth visualization adornments
//
function DepthAdornment() {
  this.attachedTo = null;
  this.actors = []; // array of 5 square attaced around and to back of actor to show depth

  for (var i = 0; i < 5; i++) { //
    // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
    this.actors.push(dali.createSolidColorActor(
      [0.5, 0.5, 0.5, 1],
      false, [0, 0, 0, 1],
      0));
  }

  ActorList.show(this.actors, false);
}

DepthAdornment.prototype.delete = function() {
  ActorList.delete(this.actors);
  this.actors = [];
}

DepthAdornment.prototype.attach = function(actor) {
  var newActor = true;
  if (this.attachedTo) {
    newActor = (actor.getId() !== this.attachedTo.getId());
  }

  if (actor && newActor) {
    this.unattach(this.attachedTo);

    this.__positionActors(actor);

    ActorList.add(this.actors, actor);

    ActorList.show(this.actors, true);

    this.attachedTo = actor;
  }
};

DepthAdornment.prototype.unattach = function() {
  if (this.attachedTo) {
    ActorList.show(this.actors, false);
    ActorList.remove(this.actors, this.attachedTo);
    this.attachedTo = null;
  }
};

DepthAdornment.prototype.__positionActors = function(actor) {
  var s = actor.size;
  s[2] = Math.max(s[2], 20); // pretend the actor is at least 20
  var p = actor.position;
  var halfw = s[0] / 2;
  var halfh = s[1] / 2;
  var halfd = s[2] / 2;

  this.actors[0].position = [-halfw, 0, -halfd];
  this.actors[0].rotation = [0, -90, 0];
  this.actors[0].size = [s[2], s[1], 1];

  this.actors[1].position = [+halfw, 0, -halfd];
  this.actors[1].rotation = [0, 90, 0];
  this.actors[1].size = [s[2], s[1], 1];

  this.actors[2].position = [0, -halfh, -halfd];
  this.actors[2].rotation = [90, 0, 0];
  this.actors[2].size = [s[0], s[2], 1];

  this.actors[3].position = [0, halfh, -halfd];
  this.actors[3].rotation = [-90, 0, 0];
  this.actors[3].size = [s[0], s[2], 1];
  // back
  // this.actors[4].position = [0, 0, -s[2]];
  // this.actors[4].rotation = [0, 0, 0];
  // this.actors[4].size = s;

};



//
// Cursor adornments
//
function Cursors() {
  this.attachedTo = null;
  this.resizers = []; // array of 8 small square resizers
  this.lines = []; // array of 4 bounding box lines
  this.lineColor = [0.5, 0.5, 0.5, 1];
  this.resizerColor = [0, 0, 0, 1];
  this.depthAdornments = new DepthAdornment();

  this.onTouched = function(actor) {
    eventHandler.onTouched(actor);
    console.log("resizer touched");
  };

  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  for (i = 0; i < 4; i++) { // lines
    this.lines.push(dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0));
  }

  for (i = 0; i < 8; i++) { // squares
    this.resizers.push(dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1));
    this.resizers[i].size = [6, 6, 1];
    //    this.resizers[i].__mouseDrag = this.__mouseDragPositionZ;
    this.resizers[i].connectSignal("touched", this.onTouched);
  }

  ActorList.show(this.lines, false);
  ActorList.show(this.resizers, false);

}


Cursors.prototype.attach = function(actor) {
  if (actor) {

    if (this.attachedTo) {
      this.unattach(this.attachedTo);
    }

    this.__positionActors(actor);

    ActorList.add(this.lines, actor);
    ActorList.add(this.resizers, actor);

    ActorList.show(this.lines, true);
    ActorList.show(this.resizers, true);

    this.depthAdornments.attach(actor);

    this.attachedTo = actor;
  }
};

Cursors.prototype.unattach = function(actor) {
  if (actor && this.attachedTo) {
    ActorList.show(this.lines, false);
    ActorList.show(this.resizers, false);

    ActorList.remove(this.lines, actor);
    ActorList.remove(this.resizers, actor);

    this.depthAdornments.unattach(actor);

    this.attachedTo = null;
  }
};

Cursors.prototype.__positionActors = function(actor) {
  var s = actor.size;
  var p = actor.position;
  var halfw = s[0] / 2;
  var halfh = s[1] / 2;

  this.lines[0].position = [0 - halfw, 0, 0];
  this.lines[0].size = [1, halfh * 2, 1];

  this.lines[1].position = [0, 0 - halfh, 0];
  this.lines[1].size = [halfw * 2, 1, 1];

  this.lines[2].position = [0 + halfw, 0, 0];
  this.lines[2].size = [1, halfh * 2, 1];

  this.lines[3].position = [0, 0 + halfh, 0];
  this.lines[3].size = [halfw * 2, 1, 1];

  // drag functions presumes order here is clockwise from top left
  this.resizers[0].position = [-halfw, +halfh, 0];
  this.resizers[1].position = [0, +halfh, 0];
  this.resizers[2].position = [+halfw, +halfh, 0];
  this.resizers[3].position = [+halfw, 0, 0];

  this.resizers[4].position = [+halfw, -halfh, 0];
  this.resizers[5].position = [0, -halfh, 0];
  this.resizers[6].position = [-halfw, -halfh, 0];
  this.resizers[7].position = [-halfw, 0, 0];
};



function App(theEventHandler) {
  var self = this;
  this.rootRotation = [0, 0, 0]; // in degrees
  this.cursors = new Cursors();
  // ActorWrapper CreateSolidColorActor(const Vector4 & color, bool border,
  //   const Vector4 & borderColor,
  //     const unsigned int borderSize)
  this.rootLayerIndicator = dali.createSolidColorActor(
    [0, 0, 0, 0.0],
    true, [0, 0, 0, 1],
    1);
  this.rootLayerIndicator.parentOrigin = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.anchorPoint = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.size = [250, 250, 1];
  this.rootLayerIndicator.position = [0, 0, 0];

  this.__handleMouseDrag = function(eventHandler) {
    if (self.resizing) {
      if (eventHandler.touchedActor) {
        eventHandler.touchedActor.size = [
          eventHandler.touchedActorSize[0] + eventHandler.drag_dx,
          eventHandler.touchedActorSize[1] + eventHandler.drag_dy,
          eventHandler.touchedActorSize[2]
        ];
      }
    } else {
      // move the touched actor
      if (eventHandler.touchedActor) {
        if (eventHandler.mouse_buttons === 0) {
          eventHandler.touchedActor.position = [
            eventHandler.touchedActorPosition[0] + eventHandler.drag_dx,
            eventHandler.touchedActorPosition[1] + eventHandler.drag_dy,
            eventHandler.touchedActorPosition[2]
          ];
        } else if (eventHandler.mouse_buttons === 2) {
          eventHandler.touchedActor.position = [
            eventHandler.touchedActorPosition[0],
            eventHandler.touchedActorPosition[1], -(eventHandler.touchedActorPosition[2] + eventHandler.drag_dy)
          ];
        }
      }

    }
    // rotate on middle button
    if (eventHandler.mouse_buttons === 1) {
      self.rootRotation = [eventHandler.mouseDownPosition[1] - eventHandler.mouse_y, -(eventHandler.mouseDownPosition[0] - eventHandler.mouse_x),
        self.rootRotation[2]
      ];
      dali.__theStage.rootActor.rotation = self.rootRotation;
    }

  };

  this.__handleMouseDragStart = function(eventHandler) {};

  this.__handleMouseDragStop = function(eventHandler) {
    // handle drag again onto the final position
    self.__handleMouseDrag(eventHandler);
    self.resizing = false;
  };

  this.__handleMouseMove = function(eventHandler) {
    // if (eventHandler.touchedActor === null && eventHandler.mouse_buttons === 1) { // middle button
    //   console.log("mouse move middle" + eventHandler.mouse_buttons);
    //   self.rootRotation = [eventHandler.mouseDownPosition[1] - eventHandler.mouse_y, -(eventHandler.mouseDownPosition[0] - eventHandler.mouse_x),
    //     self.rootRotation[2]
    //   ];
    //   dali.__theStage.rootActor.rotation = self.rootRotation;
    // }
  };

  this.__handleTouchSelect = function(eventHandler) {
    if (ActorList.contains(self.cursors.resizers, eventHandler.touchedActor)) {
      self.resizing = true;
    } else {
      self.resizing = false;
      // select the actor
      if (eventHandler.mouse_buttons === 0) {
        self.cursors.attach(eventHandler.touchedActor);
        var stage = dali.getStage();
        stage.add(self.rootLayerIndicator);
      }
    }
  };

  this.__handleTouchDeSelect = function(eventHandler) {
    self.cursors.unattach(eventHandler.touchedActor);
    var stage = dali.getStage();
    stage.remove(self.rootLayerIndicator);
  };


  // initialize
  eventHandler.register("MouseDrag", this.__handleMouseDrag);
  eventHandler.register("MouseDragStart", this.__handleMouseDragStart);
  eventHandler.register("MouseDragStop", this.__handleMouseDragStop);
  eventHandler.register("MouseMove", this.__handleMouseMove);
  eventHandler.register("TouchSelect", this.__handleTouchSelect);
  eventHandler.register("TouchDeSelect", this.__handleTouchDeSelect);

  //
  // Monkey patch the stage object (to add to root object not the root layer for rotations)
  //
  dali.__theStage.rootActor = new dali.Actor();
  dali.__theStage.rootActor.parentOrigin = [0.5, 0.5, 0.5];
  dali.__theStage.rootActor.anchorPoint = [0.5, 0.5, 0.5];
  dali.__theStage.add(dali.__theStage.rootActor);
  dali.__theStage.add = function(actor) {
    this.rootActor.add(actor);
  };

  dali.__theStage.remove = function(actor) {
    this.rootActor.remove(actor);
  };

  dali.__theStage.getRootLayer = function() {
    return dali.__theStage.rootActor;
  };

}

function addTextImage(text) {
  // console.log(sFontStyle + " " + fFontSize + "px " + sFontFamily + " char:" + iChar);
  // console.log("GetGlyphImage" + imageData + " size " + imageData.width + "," + imageData.height);

  var testCanvas = document.getElementById("testCanvas");
  var ctx = testCanvas.getContext("2d");
  var pixelHeight = 30;

  ctx.clearRect(0, 0, testCanvas.width, testCanvas.height);

  ctx.font = pixelHeight + "px Verdana";
  // Create gradient
  var gradient = ctx.createLinearGradient(0, 0, testCanvas.width, 0);
  gradient.addColorStop("0", "magenta");
  gradient.addColorStop("0.5", "blue");
  gradient.addColorStop("1.0", "red");
  // Fill with gradient
  ctx.fillStyle = gradient;
  ctx.fillText(text, 0, pixelHeight);

  var m = ctx.measureText(text);

  var a = dali.ImageActor();

  a.setImageData(ctx.getImageData(0, 0, m.width, pixelHeight));

  function onTouched(actor) {
    // console.log("touched " + actor.$$.ptr + " " + actor.position);
    eventHandler.onTouched(actor);
  }

  a.connectSignal("touched", onTouched);

  stage.add(a);

  return a;
}



//
// Global
//

var eventHandler = new EventHandler();
var app = new App(eventHandler);
// var cursors = new CursorsAndEvents();
var canvas = document.getElementById("canvas");

stage.setBackgroundColor([0.3, 0.3, 0.3, 1]);

function mouseDown(e) {
  eventHandler.mouseDown(canvas, e);
}

function mouseUp(e) {
  eventHandler.mouseUp(canvas, e);
}

function mouseMove(e) {
  eventHandler.mouseMove(canvas, e);
}

function mouseWheel(e) {
  eventHandler.mouseWheel(canvas, e);
}

function keyDown(e) {
  eventHandler.keyDown(canvas, e);
}

function keyUp(e) {
  eventHandler.keyUp(canvas, e);
}

canvas.onmousemove = mouseMove;
canvas.onmousedown = mouseDown;
canvas.onmouseup = mouseUp;
canvas.onwheel = mouseWheel;

canvas.onkeydown = keyDown;
canvas.onkeyup = keyUp;

function runTests() {
  var stage = dali.getStage();

  function onTouched(actor) {
    // console.log("touched " + actor.$$.ptr + " " + actor.position);
    eventHandler.onTouched(actor);
  }

  var actor = new dali.TextActor();
  actor.parentOrigin = [0.5, 0.5, 0.5];
  actor.anchorPoint = [0.5, 0.5, 0.5];
  actor.text = "actor";
  actor.size = [100, 100, 1];
  actor.position = [0, 0, 10];
  stage.add(actor);

  var hello = new dali.TextActor();
  hello.connectSignal("touched", onTouched);
  hello.text = "hello";
  actor.add(hello);

  var hellochild = new dali.TextActor();
  hellochild.connectSignal("touched", onTouched);
  hellochild.text = "hello-child";
  hello.add(hellochild);

  var hellochild2 = new dali.TextActor();
  hellochild2.connectSignal("touched", onTouched);
  hellochild2.text = "hello-child2";
  hello.add(hellochild2);

  var hellochildchild = new dali.TextActor();
  hellochildchild.connectSignal("touched", onTouched);
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

  actor.connectSignal("touched", onTouched);

  var another = new dali.TextActor();
  another.parentOrigin = [0.5, 0.5, 0.5];
  another.anchorPoint = [0.5, 0.5, 0.5];
  another.text = "peppa";
  another.size = [100, 100, 1];
  another.position = [-50, 100, 0];
  another.connectSignal("touched", onTouched);
  stage.add(another);

  var imageText = addTextImage("Colour");
  imageText.connectSignal("touched", onTouched);

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

function runRandomAnimation(actor) {
  var a = new dali.Animation();
  a.setDuration(6);
  a.setLooping(true);
  dali.animateTo(a, actor, "position", [300 * Math.random() - 150, 300 * Math.random() - 150, 0],
    "linear", 0, 3);
  dali.animateTo(a, actor, "position", [0, 0, 0], "linear", 3, 3);
  dali.animateTo(a, actor, "rotation", [0, 0, 1, 720 * Math.random() - 360], "linear", 0, 3);
  dali.animateTo(a, actor, "rotation", [0, 0, 1, 0], "linear", 3, 3);

  a.play();
  //    a.delete(); // delete the animtion object but a.Play() keeps it running in Dali
  return anim;
}

function animate3DView() {
  var stage = dali.getStage();
  var root = stage.getRootLayer();
  var rotator = stage.create("Actor");
  var actors = [];
  var a;
  // first child is the camera
  for (var i = 1, len = root.getChildCount(); i < len; i++) {
    a = root.getChildAt(i);
    actors.push(a);
    root.remove(a);
    rotator.add(a);
    a.delete();
  }

  stage.add(rotator);

  var anim = dali.Animation();
  var toRotation = [60, 0, 0];
  anim.setDuration(4);

  dali.animateBy(anim, rotator, "rotation", toRotation, "linear", 0, 2);
  dali.animateBy(anim, rotator, "rotation", actors[i].rotation, "linear", 2, 2);

  dali.animateBy(anim, rotator, "position", [0, 0, -800], "linear", 0, 2);
  dali.animateBy(anim, rotator, "position", [0, 0, 0], "linear", 2, 2);

  anim.play();
  anim.delete();

}

runTests();
