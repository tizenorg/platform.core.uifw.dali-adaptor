/*global  */
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
  this.selectedActor = null; // set if actor is touched (left click)
  this.selectedActorPosition = [0, 0, 0];
  //private
  this.firstTouch = false;
  this.mouseIsDown = 0;
  this.mouseDragState = EventState.STOPPED;
  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;
  // handlers; use register("MouseDragStart", func); where for func(eventHandler)
  this.handlersMouseDragStart = []; // valid; selectedActor(can be null), selectedActorPosition
  this.handlersMouseDragStop = []; // valid; selectedActor(can be null), selectedActorPosition
  this.handlersMouseDrag = []; // valid; selectedActor(can be null), selectedActorPosition, drag_dx, drag_dy
  this.handlersMouseMove = []; // valid; selectedActor(can be null), selectedActorPosition, drag_dx, drag_dy
  this.handlersTouchSelect = []; // valid; selectedActor, selectedActorPosition
  this.handlersTouchDeSelect = []; // valid; selectedActor, selectedActorPosition
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
  // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.selectedActor);
  if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
    this.selectedActor = actor;
    this.selectedActorPosition = actor.position;
    this.firstTouch = false;
    this.handleEvent(this.handlersTouchSelect);
  }
};

EventHandler.prototype.resizerOnTouched = function(actor) {
  // on touched seems to run for all moves,ups,downs... in c++ we have access to mode parameter
  // here we have to track it
  // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.selectedActor);
  if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
    this.selectedActor = actor;
    this.selectedActorPosition = actor.position;
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
  // console.log("mouse down " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.selectedActor + " " + this.firstTouch);
  if (this.selectedActor) {
    var p = this.selectedActor.position;
    var s = this.selectedActor.size;
    var w2 = s[0] / 2;
    var h2 = s[1] / 2;
    // click outside and we deselect actor
    var screenToLocal = this.selectedActor.__screenToLocal(this.mouse_x, this.mouse_y);
    var localX = screenToLocal.get(0);
    var localY = screenToLocal.get(1);
    var ok = screenToLocal.get(2) === 1.0;
    if (ok && (localX < 0 || localX > s[0] || localY < 0 || localY > s[1])) {
      // if (this.mouse_x < (p[0] - w2) || this.mouse_x > (p[0] + w2) ||
      //   this.mouse_y < (p[1] - h2) || this.mouse_y > (p[1] + h2)) {
      if (this.mouse_buttons === 0) {
        this.handleEvent(this.handlersTouchDeSelect);
      }
      if (this.mouseDragState === EventState.ACTIVE) {
        this.handleEvent(this.handlersMouseDragStop);
        this.mouseDragState = EventState.STOPPED;
        this.selectedActorPosition = this.selectedActor.position;
      }
      if (this.mouse_buttons === 0) {
        this.selectedActor = null;
      }
    }
    // console.log("local: " + localX + "," + localX + "outside? " + (localX < 0 || localX > s[0] || localY < 0 || localY > s[1]).toString() + " selected?" + this.selectedActor);
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
    // if (this.selectedActor) {
    //   this.handleEvent(this.handlersTouchDeSelect);
    //   this.selectedActor = null;
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
// Cursor adornments
//
function Cursors() {
  this.attachedActor = null;
  this.resizers = []; // array of 8 small square resizers
  this.lines = []; // array of 4 bounding box lines
  this.lineColor = [0.5, 0.5, 0.5, 1];
  this.resizerColor = [0, 0, 0, 1];

  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  for (i = 0; i < 4; i++) { // lines
    this.lines.push(dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0));
  }

  for (i = 0; i < 8; i++) { // squares
    this.resizers.push(dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1));
    this.resizers[i].size = [6, 6, 1];
    //    this.resizers[i].__mouseDrag = this.__mouseDragPositionZ;
  }

  this.__showAll(false);

  // for (i = 0; i < 4; i++) {
  //   stage.add(this.lines[i]);
  // }

  // for (i = 0; i < 8; i++) {
  //   stage.add(this.resizers[i]);
  // }

}

Cursors.prototype.attach = function(actor) {
  if (actor) {

    if (this.attachedActor) {
      this.unattach(this.selectedActor);
    }

    this.__highlightActor(actor);
    for (i = 0; i < 4; i++) {
      actor.add(this.lines[i]);
    }

    for (i = 0; i < 8; i++) {
      actor.add(this.resizers[i]);
    }
    this.attachedActor = actor;
  }
};

Cursors.prototype.unattach = function(actor) {
  if (actor && this.attachedActor) {
    this.__unHighlight();
    for (i = 0; i < 4; i++) {
      actor.remove(this.lines[i]);
    }

    for (i = 0; i < 8; i++) {
      actor.remove(this.resizers[i]);
    }
    this.attachedActor = null;
  }
};

Cursors.prototype.__showAll = function(show) {
  for (i = 0; i < 4; i++) {
    this.lines[0].visible = show;
  }

  for (i = 0; i < 8; i++) {
    this.resizers[0].visible = show;
  }
};

Cursors.prototype.__highlightActor = function(actor) {
  this.selectedActor = actor;

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
  this.__showAll(false);
};



function App(theEventHandler) {
  var self = this;
  this.rootRotation = [0, 0, 0]; // in degrees
  this.cursors = new Cursors();
  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  this.rootLayerIndicator = dali.createSolidColorActor(
    [0, 0, 0, 0.0],
    true, [0, 0, 0, 1],
    1);
  this.rootLayerIndicator.parentOrigin = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.anchorPoint = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.size = [250, 250, 1];
  this.rootLayerIndicator.position = [0, 0, 0];

  this.__handleMouseDrag = function(eventHandler) {
    if (eventHandler.selectedActor) {
      if (eventHandler.mouse_buttons === 0) {
        eventHandler.selectedActor.position = [
          eventHandler.selectedActorPosition[0] + eventHandler.drag_dx,
          eventHandler.selectedActorPosition[1] + eventHandler.drag_dy,
          eventHandler.selectedActorPosition[2]
        ];
      } else if (eventHandler.mouse_buttons === 2) {
        eventHandler.selectedActor.position = [
          eventHandler.selectedActorPosition[0],
          eventHandler.selectedActorPosition[1], -(eventHandler.selectedActorPosition[2] + eventHandler.drag_dy)
        ];
      }
    }

    if (eventHandler.mouse_buttons === 1) { // middle button
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
  };

  this.__handleMouseMove = function(eventHandler) {
    // if (eventHandler.selectedActor === null && eventHandler.mouse_buttons === 1) { // middle button
    //   console.log("mouse move middle" + eventHandler.mouse_buttons);
    //   self.rootRotation = [eventHandler.mouseDownPosition[1] - eventHandler.mouse_y, -(eventHandler.mouseDownPosition[0] - eventHandler.mouse_x),
    //     self.rootRotation[2]
    //   ];
    //   dali.__theStage.rootActor.rotation = self.rootRotation;
    // }
  };

  this.__handleTouchSelect = function(eventHandler) {
    self.cursors.attach(eventHandler.selectedActor);
    var stage = dali.getStage();
    stage.add(self.rootLayerIndicator);
  };

  this.__handleTouchDeSelect = function(eventHandler) {
    self.cursors.unattach(eventHandler.selectedActor);
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


//
// Old cursors and events
//

function CursorsAndEvents() {
  this.selectedActor = null;

  this.resizers = []; // array of 8 small square resizers
  this.lines = []; // array of 4 bounding box lines
  this.lineColor = [0.5, 0.5, 0.5, 1];
  this.resizerColor = [0, 0, 0, 1];
  this.attached = false;
  this.firstTouch = false;
  this.mouseIsDown = 0; //  1 == down

  // this.mouseDownX = 0;
  // this.mouseDownY = 0;

  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;

  this.mouseDownPosition = [0, 0, 0];
  // this.mouseDownRotation = [0, 0, 0, 0];
  // this.mouseDownSize = [0, 0, 0];

  this.selectedActorPosition = [0, 0, 0];

  this.mouseDrag = this.__mouseDownMoved;
  this.selectActor = this.__selectActor;
  this.deselectActor = this.__deSelectActor;

  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  for (i = 0; i < 4; i++) { // lines
    this.lines.push(dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0));
  }

  for (i = 0; i < 8; i++) { // squares
    this.resizers.push(dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1));
    this.resizers[i].size = [6, 6, 1];
    //    this.resizers[i].__mouseDrag = this.__mouseDragPositionZ;
  }

  this.__showAll(false);

  for (i = 0; i < 4; i++) {
    stage.add(this.lines[i]);
  }

  for (i = 0; i < 8; i++) {
    stage.add(this.resizers[i]);
  }
}

CursorsAndEvents.prototype.__mouseDragPositionZ = function(dx, dy, mouseState) {
  if (this.selectedActor) {
    this.selectedActor.position = [
      this.selectedActorPosition[0],
      this.selectedActorPosition[1],
      this.selectedActorPosition[2] + dx + dy
    ];
  }
};

CursorsAndEvents.prototype.onTouched = function(actor) {
  // on touched runs for all moves,ups,downs... in c++ we have access to mode parameter
  // here we have to track it
  if (this.firstTouch && this.mouseIsDown) {

    if (actor.hasOwnProperty("__mouseDrag")) { // for dragging resizers etc
      this.mouseDrag = actor.__mouseDrag;
      this.selectActor = actor.__selectActor;
      this.deSelectActor = actor.__deSelectActor;
    } else {
      this.mouseDrag = this.__mouseDrag;
      this.selectActor = this.__selectActor;
      this.deSelectActor = this.__deSelectActor;
    }

    this.selectActor(actor);
    console.log("select " + actor.$$.ptr + " " + this.selectedActorPosition);

    this.firstTouch = false;
  }
};

CursorsAndEvents.prototype.__selectActor = function(actor) {
  this.__highlightActor(actor);
  this.selectedActorPosition = actor.position;
};

CursorsAndEvents.prototype.__deselectActor = function() {
  if (this.selectedActor) {
    console.log("unselect " + this.selectedActor.$$.ptr);
    this.__unHighlight();
    this.selectedActor.delete();
    this.selectedActor = null;
  }
};

CursorsAndEvents.prototype.mouseDown = function(canvas, e) {
  this.mouseIsDown = 1;
  var rect = canvas.getBoundingClientRect(); // in window
  var x = e.clientX - rect.left; // from window
  var y = e.clientY - (rect.bottom - rect.height);
  dali.sendMouseEvent(x, y, 0); // 0 == down, 1 == up, 2 == motion
  // this.mouseDownX = x;
  // this.mouseDownY = y;
  this.firstTouch = true;
  this.mouseDownPosition = [x, y, 0];
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
};

CursorsAndEvents.prototype.mouseUp = function(canvas, e) {
  this.mouseIsDown = 0;
  var rect = canvas.getBoundingClientRect(); // in window
  var x = e.clientX - rect.left; // from window
  var y = e.clientY - (rect.bottom - rect.height);
  var dx = (x - this.mouseDownPosition[0]);
  var dy = (y - this.mouseDownPosition[1]);
  dali.sendMouseEvent(x, y, 1); // 0 == down, 1 == up, 2 == motion
  this.mouseDrag(dx, dy, 0);
};

CursorsAndEvents.prototype.__mouseDownMoved = function(dx, dy, dz) {
  if (this.selectedActor) {
    this.selectedActor.position = [dx + this.selectedActorPosition[0],
      dy + this.selectedActorPosition[1],
      this.selectedActorPosition[2]
    ];
  }

  // if (this.shiftKey) {
  //   var p = this.selectedActor.rotation;
  //   // console.log(touchedActor.position);
  //   // console.log([p[0] + dx, p[1] + dy, p[2]]);
  //   this.selectedActor.rotation = [p[0] + dx, p[1] + dy, p[2]];
  // } else {
  //   // this.mouseDownRotation = this.selectedActor.rotation;
  //   // this.mouseDownSize = this.selectedActor.size;

  //   //var p = this.selectedActor.position;
  //   // console.log(touchedActor.position);
  //   // console.log([p[0] + dx, p[1] + dy, p[2]]);
  //   // this.selectedActor.position = [p[0] + dx, p[1] + dy, p[2]];
  //   //touchedActor.position = [p[0] + 5, p[1] + 5, p[2]];
  //   // console.log(touchedActor.position);
  //   // console.log(dx + "," + dy);
  // }

}

CursorsAndEvents.prototype.mouseMove = function(canvas, e) {
  var rect = canvas.getBoundingClientRect(); // in window
  var x = e.clientX - rect.left; // from window
  var y = e.clientY - (rect.bottom - rect.height);

  dali.sendMouseEvent(x, y, 2); // 0 == down, 1 == up, 2 == motion

  var dx = (x - this.mouseDownPosition[0]);
  var dy = (y - this.mouseDownPosition[1]);

  if (cursors.selectedActor && this.mouseIsDown) {
    console.log("mouse active move " + this.mouseIsDown + " " + this.shiftKey);

    this.mouseDrag(dx, dy, 0);
  }
};

CursorsAndEvents.prototype.__getOffsets = function(e, actor) {
  var rect = canvas.getBoundingClientRect(); // in window
  var x = e.clientX - rect.left; // from window
  var y = e.clientY - (rect.bottom - rect.height);
  return [this.mouseDownX - x, this.mouseDownY - y];
};

CursorsAndEvents.prototype.onKeyDown = function(e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

CursorsAndEvents.prototype.onKeyUp = function(e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

CursorsAndEvents.prototype.__showAll = function(show) {
  for (i = 0; i < 4; i++) {
    this.lines[0].visible = show;
  }

  for (i = 0; i < 8; i++) {
    this.resizers[0].visible = show;
  }
};

CursorsAndEvents.prototype.__attach = function(actor) {
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

CursorsAndEvents.prototype.__unattach = function(actor) {
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

CursorsAndEvents.prototype.__highlightActor = function(actor) {
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

  // drag functions presumes order here is clockwise from top left
  this.resizers[0].position = [-halfw, +halfh, 0];
  this.resizers[1].position = [0, +halfh, 0];
  this.resizers[2].position = [+halfw, +halfh, 0];
  this.resizers[3].position = [+halfw, 0, 0];

  this.resizers[4].position = [+halfw, -halfh, 0];
  this.resizers[5].position = [0, -halfh, 0];
  this.resizers[6].position = [-halfw, -halfh, 0];
  this.resizers[7].position = [-halfw, 0, 0];

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

CursorsAndEvents.prototype.__unHighlight = function() {
  this.__unattach(this.selectedActor);
  this.__showAll(false);
};

//
// Global
//

var eventHandler = new EventHandler();
var app = new App(eventHandler);
// var cursors = new CursorsAndEvents();
var canvas = document.getElementById("canvas");

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

// function debugEventDragStart(eventHandler) {
//   console.log("debugEventDragStart" + eventHandler.selectedActor);
// }

// function debugEventDragStop(eventHandler) {
//   console.log("debugEventDragStop" + eventHandler.selectedActor);
// }

// function debugEventDrag(eventHandler) {
//   console.log("debugEventDrag" + eventHandler.selectedActor + eventHandler.drag_dx + eventHandler.drag_dy);
// }

// eventHandler.register("MouseDragStart", debugEventDragStart);
// eventHandler.register("MouseDragStop", debugEventDragStop);
// eventHandler.register("MouseDrag", debugEventDrag);

//
//
//

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
