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

    // i.connectSignal("touched", function(actor) {
    //   eventHandler.onTouched(actor);
    // });

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
// var EventState = {
//   STOPPED: {
//     value: 0,
//     name: "Stopped"
//   },
//   TRIGGER_START: {
//     value: 1,
//     name: "TriggerStart"
//   },
//   ACTIVE: {
//     value: 2,
//     name: "Active"
//   },
// };

// function EventHandler() {
//   // public
//   this.mouse_x = 0;
//   this.mouse_y = 0;
//   this.mouse_buttons = 0;
//   this.mouseDownPosition = [0, 0, 0];
//   this.drag_dx = 0;
//   this.drag_dy = 0;
//   this.touchedActor = null; // set if actor is touched (left click)
//   this.touchedActorPosition = [0, 0, 0];
//   this.touchedActorSize = [0, 0, 0];
//   //private
//   this.firstTouch = false;
//   // this.selectedActor = null;
//   // this.selectedActorPosition = [0, 0, 0];
//   this.mouseIsDown = 0;
//   this.mouseDragState = EventState.STOPPED;
//   this.metaKey = 0;
//   this.altKey = 0;
//   this.ctrlKey = 0;
//   this.shiftKey = 0;
//   // handlers; use register("MouseDragStart", func); where for func(eventHandler)
//   this.handlersMouseDragStart = []; // valid; touchedActor(can be null), touchedActorPosition
//   this.handlersMouseDragStop = []; // valid; touchedActor(can be null), touchedActorPosition
//   this.handlersMouseDrag = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
//   this.handlersMouseMove = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
//   this.handlersTouchSelect = []; // valid; touchedActor, touchedActorPosition
//   this.handlersTouchDeSelect = []; // valid; touchedActor, touchedActorPosition
// }

// EventHandler.prototype.register = function(name, handler) {
//   var handlers = this["handlers" + name];
//   if (!(handler in handlers)) {
//     handlers.push(handler);
//   }
// };

// EventHandler.prototype.unRegister = function(name, handler) {
//   var handlers = this["handlers" + name];
//   var index = handlers.index(handler);
//   if (index >= 0) {
//     handlers.splice(index, 1);
//   }
// };

// EventHandler.prototype.onTouched = function(actor) {
//   // on touched seems to run for all moves,ups,downs... in c++ we have access to mode parameter
//   // here we have to track it
//   // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor);
//   if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
//     this.touchedActor = actor;
//     this.touchedActorPosition = actor.position;
//     this.touchedActorSize = actor.size;
//     this.handleEvent(this.handlersTouchSelect);
//     this.firstTouch = false;
//   }
// };

// EventHandler.prototype.resizerOnTouched = function(actor) {
//   // on touched seems to run for all moves,ups,downs... in c++ we have access to mode parameter
//   // here we have to track it
//   // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor);
//   if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
//     this.touchedActor = actor;
//     this.touchedActorPosition = actor.position;
//     this.touchedActorSize = actor.size;
//     this.firstTouch = false;
//     this.handleEvent(this.handlersTouchSelect);
//   }
// };

// EventHandler.prototype.mouseDown = function(canvas, e) {
//   this.mouseIsDown = 1;
//   var rect = canvas.getBoundingClientRect(); // in window
//   this.mouse_x = e.clientX - rect.left; // from window
//   this.mouse_y = e.clientY - (rect.bottom - rect.height);
//   this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
//   dali.sendMouseEvent(this.mouse_x, this.mouse_y, 0); // 0 == down, 1 == up, 2 == motion
//   // this.mouseDownX = x;
//   // this.mouseDownY = y;
//   this.mouseDownPosition = [this.mouse_x, this.mouse_y, 0];
//   // if (this.mouseDragState === EventState.STOPPED) {
//   this.firstTouch = true;
//   this.mouseDragState = EventState.TRIGGER_START;
//   // }
//   // console.log("mouse down " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor + " " + this.firstTouch);
//   if (this.touchedActor) {
//     var p = this.touchedActor.position;
//     var s = this.touchedActor.size;
//     var w2 = s[0] / 2;
//     var h2 = s[1] / 2;
//     // click outside and we deselect actor
//     var screenToLocal = this.touchedActor.__screenToLocal(this.mouse_x, this.mouse_y);
//     var localX = screenToLocal.get(0);
//     var localY = screenToLocal.get(1);
//     var ok = screenToLocal.get(2) === 1.0;
//     if (ok && (localX < 0 || localX > s[0] || localY < 0 || localY > s[1])) {
//       // if (this.mouse_x < (p[0] - w2) || this.mouse_x > (p[0] + w2) ||
//       //   this.mouse_y < (p[1] - h2) || this.mouse_y > (p[1] + h2)) {
//       // if (this.mouse_buttons === 0) {
//       //   this.handleEvent(this.handlersTouchDeSelect);
//       // }
//       if (this.mouseDragState === EventState.ACTIVE) {
//         this.handleEvent(this.handlersMouseDragStop);
//         this.mouseDragState = EventState.STOPPED;
//         this.touchedActorPosition = this.touchedActor.position;
//         this.touchedActorSize = this.touchedActor.size;
//       }
//       if (this.mouse_buttons === 0) {
//         this.handleEvent(this.handlersTouchDeSelect);
//         this.touchedActor = null;
//       }
//     }
//     // console.log("local: " + localX + "," + localX + "outside? " + (localX < 0 || localX > s[0] || localY < 0 || localY > s[1]).toString() + " selected?" + this.touchedActor);
//   }
// };

// EventHandler.prototype.mouseMove = function(canvas, e) {
//   var rect = canvas.getBoundingClientRect(); // in window
//   this.mouse_x = e.clientX - rect.left; // from window
//   this.mouse_y = e.clientY - (rect.bottom - rect.height);
//   dali.sendMouseEvent(this.mouse_x, this.mouse_y, 2); // 0 == down, 1 == up, 2 == motion
//   this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
//   this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);

//   var hover = dali.hitTest(this.mouse_x, this.mouse_y);
//   if(hover)
//   {
//     console.log(hover.getId());
//   }

//   // do this before setting mouseDragState so we actually drag on 2nd mouseMove where mouseIsDown
//   if (this.mouseIsDown && this.mouseDragState === EventState.ACTIVE) {
//     //console.log("mouse active move " + this.mouseIsDown + " " + this.shiftKey);
//     this.handleEvent(this.handlersMouseDrag);
//   }

//   if (this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
//     this.handleEvent(this.handlersMouseDragStart);
//     this.mouseDragState = EventState.ACTIVE;
//   }

//   this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
//   this.handleEvent(this.handlersMouseMove);

// };

// EventHandler.prototype.mouseUp = function(canvas, e) {
//   this.mouseIsDown = 0;
//   var rect = canvas.getBoundingClientRect(); // in window
//   this.mouse_x = e.clientX - rect.left; // from window
//   this.mouse_y = e.clientY - (rect.bottom - rect.height);
//   this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
//   this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
//   dali.sendMouseEvent(this.mouse_x, this.mouse_y, 1); // 0 == down, 1 == up, 2 == motion
//   if (this.mouseDragState == EventState.ACTIVE) {
//     this.handleEvent(this.handlersMouseDragStop);
//     // if (this.touchedActor) {
//     //   this.handleEvent(this.handlersTouchDeSelect);
//     //   this.touchedActor = null;
//     // }
//     this.mouseDragState = EventState.STOPPED;
//   }

//   this.mouse_buttons = e.button;
//   // this.handleEvent(this.handlersMouseEvent);
// };

// EventHandler.prototype.mouseWheel = function(canvas, e) {
//   // multiples of +120 for up, -120 for down
//   var clicks = e.wheelDelta / 120;

//   var stage = dali.getStage();
//   var taskList = stage.getRenderTaskList();
//   var task = taskList.getTask(0);
//   var cameraActor = task.getCameraActor();

//   var fov = cameraActor.getFieldOfView();
//   cameraActor.setFieldOfView(fov + (clicks / 180.0 * Math.PI));

//   taskList.delete();
//   task.delete();
//   cameraActor.delete();
//   // console.log(e.wheelDelta);
// }

// EventHandler.prototype.handleEvent = function(handlers) {
//   for (var i = 0, len = handlers.length; i < len; i++) {
//     var handler = handlers[i];
//     handler(this);
//   }
// };

// EventHandler.prototype.keyDown = function(canvas, e) {
//   this.metaKey = e.metaKey;
//   this.altKey = e.altKey;
//   this.ctrlKey = e.ctrlKey;
//   this.shiftKey = e.shiftKey;
// };

// EventHandler.prototype.keyUp = function(canvas, e) {
//   this.metaKey = e.metaKey;
//   this.altKey = e.altKey;
//   this.ctrlKey = e.ctrlKey;
//   this.shiftKey = e.shiftKey;
// };

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
  // this.selectedActor = null;
  // this.selectedActorPosition = [0, 0, 0];
  this.mouseIsDown = 0;
  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;
  // handlers; use register("MouseDragStart", func); where for func(eventHandler)
  this.handlersMouseDrag = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
  this.handlersSelect = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
  this.handlersDeselect = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
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

EventHandler.prototype.updateMouseState = function(canvas, e) {
  var rect = canvas.getBoundingClientRect(); // in window
  this.mouse_x = e.clientX - rect.left; // from window
  this.mouse_y = e.clientY - (rect.bottom - rect.height);
  this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
}

EventHandler.prototype.mouseDown = function(canvas, e) {
  this.mouseIsDown = 1;
  this.updateMouseState(canvas, e);
  var hitActor = dali.hitTest(this.mouse_x, this.mouse_y);
  var tid = -1;
  var hid = -2;
  if( this.touchedActor ) { tid = this.touchedActor.getId(); }
  if( hitActor ) { tid = hitActor.getId(); }

  if( hid != tid )
  {
    if( tid > 0 ) {
      this.handleEvent(this.handlersDeselect);
      this.touchedActor = null;
    }
  }

  if( hitActor.getId() == dali.__theStage.rootActor.getId() )
  {
    hitActor = null;
  }

  if( hitActor )
  {
    this.mouseDownPosition = [this.mouse_x, this.mouse_y, 0];
    this.touchedActor = hitActor;
    this.touchedActorPosition = hitActor.position;
    this.touchedActorSize = hitActor.size;

    this.handleEvent(this.handlersSelect);
  }

};

EventHandler.prototype.mouseMove = function(canvas, e) {
  this.updateMouseState(canvas, e);

  if(this.touchedActor && this.mouseIsDown)
  {
    this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
    this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
    this.handleEvent(this.handlersMouseDrag);
  }
};

EventHandler.prototype.mouseUp = function(canvas, e) {
  this.mouseIsDown = 0;
  this.updateMouseState(canvas, e);
  this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
  this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
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
    }
    return false;
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

DepthAdornment.prototype.reposition = function() {
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
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

  // this.onTouched = function(actor) {
  //   eventHandler.onTouched(actor);
  //   console.log("resizer touched");
  // };

  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  for (i = 0; i < 4; i++) { // lines
    this.lines.push(dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0));
  }

  for (i = 0; i < 8; i++) { // squares
    this.resizers.push(dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1));
    this.resizers[i].size = [10, 10, 1];
    //    this.resizers[i].__mouseDrag = this.__mouseDragPositionZ;
    // this.resizers[i].connectSignal("touched", this.onTouched);
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

Cursors.prototype.reposition = function() {
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
    this.depthAdornments.reposition();
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
  var offsetZ = 5;
  this.resizers[0].position = [-halfw, +halfh, offsetZ];
  this.resizers[1].position = [0, +halfh, offsetZ];
  this.resizers[2].position = [+halfw, +halfh, offsetZ];
  this.resizers[3].position = [+halfw, 0, offsetZ];

  this.resizers[4].position = [+halfw, -halfh, offsetZ];
  this.resizers[5].position = [0, -halfh, offsetZ];
  this.resizers[6].position = [-halfw, -halfh, offsetZ];
  this.resizers[7].position = [-halfw, 0, offsetZ];
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

  this.selectedActor = null;
  this.selectedActorPosition = [0, 0, 0];
  this.selectedActorSize = [0, 0, 0];

  this.__handleMouseDrag = function(eventHandler) {
    if (self.resizing) {
      if (self.selectedActor) {
        self.selectedActor.size = [
          self.selectedActorSize[0] + eventHandler.drag_dx,
          self.selectedActorSize[1] + eventHandler.drag_dy,
          self.selectedActorSize[2]
        ];
        self.cursors.reposition();
      }
    } else {
      // move the touched actor
      if (self.selectedActor) {
        if (eventHandler.mouse_buttons === 0) {
          self.selectedActor.position = [
            self.selectedActorPosition[0] + eventHandler.drag_dx,
            self.selectedActorPosition[1] + eventHandler.drag_dy,
            self.selectedActorPosition[2]
          ];
        } else if (eventHandler.mouse_buttons === 2) {
          self.selectedActor.position = [
            self.selectedActorPosition[0],
            self.selectedActorPosition[1], -(self.selectedActorPosition[2] + eventHandler.drag_dy)
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

  this.__selectActor = function(actor) {
    // select the actor
    self.selectedActor = eventHandler.touchedActor;
    self.selectedActorPosition = eventHandler.touchedActorPosition;
    self.selectedActorSize = eventHandler.touchedActorSize;
    self.cursors.attach(self.selectedActor);
    var stage = dali.getStage();
    stage.add(self.rootLayerIndicator);
  }

  this.__deselectActor = function() {
    self.cursors.unattach(self.selectedActor);
    var stage = dali.getStage();
    stage.remove(self.rootLayerIndicator);
  }

  this.__handleSelect = function(eventHandler) {
    if( eventHandler.mouse_buttons == 0 ) // left click only selects actor
    {
      self.__selectActor(eventHandler.touchedActor);
    }
  };

  this.__handleDeselect = function(eventHandler) {
    self.__deselectActor();
  };

  // initialize
  eventHandler.register("MouseDrag", this.__handleMouseDrag);
  eventHandler.register("Select", this.__handleSelect);
  eventHandler.register("Deselect", this.__handleDeselect);

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

  a.name = text;

  // function onTouched(actor) {
  //   // console.log("touched " + actor.$$.ptr + " " + actor.position);
  //   eventHandler.onTouched(actor);
  // }
  // a.connectSignal("touched", onTouched);

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

  // function onTouched(actor) {
  //   // console.log("touched " + actor.$$.ptr + " " + actor.position);
  //   eventHandler.onTouched(actor);
  // }

  var actor = new dali.TextActor();
  actor.parentOrigin = [0.5, 0.5, 0.5];
  actor.anchorPoint = [0.5, 0.5, 0.5];
  actor.text = "actor";
  actor.name = actor.text;
  actor.size = [100, 100, 1];
  actor.position = [0, 0, 10];
  stage.add(actor);

  var hello = new dali.TextActor();
  // hello.connectSignal("touched", onTouched);
  hello.text = "hello";
  hello.name = hello.text;
  actor.add(hello);

  var hellochild = new dali.TextActor();
  // hellochild.connectSignal("touched", onTouched);
  hellochild.text = "hello-child";
  hellochild.name = hellochild.text;
  hello.add(hellochild);

  var hellochild2 = new dali.TextActor();
  // hellochild2.connectSignal("touched", onTouched);
  hellochild2.text = "hello-child2";
  hellochild2.name = hellochild2.text;
  hello.add(hellochild2);

  var hellochildchild = new dali.TextActor();
  // hellochildchild.connectSignal("touched", onTouched);
  hellochildchild.text = "hello-child-child1";
  hellochildchild.name = "hello-child-child1";
  hellochildchild.name = hellochildchild.text;
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

  // actor.connectSignal("touched", onTouched);

  var inserted = new dali.TextActor();
  inserted.parentOrigin = [0.5, 0.5, 0.5];
  inserted.anchorPoint = [0.5, 0.5, 0.5];
  inserted.text = "inserted";
  inserted.name = inserted.text;
  inserted.size = [100, 100, 1];
  inserted.position = [0, 0, 50];
  actor.insert(1, inserted);
  assert(actor.getChildAt(1).text === "inserted");

  var another = new dali.TextActor();
  another.parentOrigin = [0.5, 0.5, 0.5];
  another.anchorPoint = [0.5, 0.5, 0.5];
  another.text = "peppa";
  another.name = another.text;
  another.size = [100, 100, 1];
  another.position = [-50, 100, 0];


  var imageText = addTextImage("Colour");
  // imageText.connectSignal("touched", onTouched);

  var s = dali.getStage();
  var root = s.getRootLayer();
  var c = root.getChildAt(1);
  var n = c.getChildCount();
  var p = c.getParent();
  assert( p.getId() == root.getId() );

  var matrix = c.worldMatrix;

  assert( matrix.length === 16 );


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

//------------------------------------------------------------------------------
// scratch
//------------------------------------------------------------------------------

function clear()
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();
  var children = root.getChildren();

  for(var i =0, len = children.length; i < len; i++)
  {
    root.remove(children[i]);
    children[i].delete(); // delete the wrapper
  }
//  root.delete(); // wrapper
}

function square(color, size)
{
  var a = dali.createSolidColorActor(color, 0, [0,0,0,1], 0);
  a.size = size;
  return a;
}

function threeSquares()
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();

  var a = square([1,0,0,1], [200,200,0]);
  a.name = "red";
  a.position = [-100,0,-20];
  root.add(a);
  a.delete();

  a = square([0,1,0,1], [200,200,0]);
  a.name = "green";
  a.position = [0,-100,-10];
  root.add(a);
  a.delete();

  a = square([0,0,1,1], [200,200,0]);
  a.name = "blue";
  a.position = [0,-100,0];
  root.add(a);
  a.delete();

//  root.delete();
}

function threeSquares2()
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();

  var red = square([1,0,0,1], [200,200,0]);
  red.name = "red";
  red.position = [-100,0,20];
  red.size = [10,10,0];
  root.add(red);

  var green = square([0,1,0,1], [200,200,0]);
  green.name="green";
  green.position = [0,-100,0];
  green.rotation = [0,0,20];
  green.size = [200,200,0];

  var blue = square([0,0,1,1], [200,200,0]);
  blue.name="blue";
  blue.position = [0,0,10];
  blue.parentOrigin = [0,0,0];
  blue.size = [100,100,0];
  green.add(blue);
  root.add(green);

  red.delete(); // wrapper
  green.delete(); // wrapper
  blue.delete(); // wrapper

  //  root.delete();
}

function collectByName(collection)
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();

  var op = function(actor)
  {
    if(actor.name)
    {
      collection[actor.name] = actor;
    }
    return true;
  };

  dali.debugDepthVisit(root, op, true);
}

function degree(radians)
{
  return (radians*180.0)/Math.PI;
}

function radian(degrees)
{
  return (degrees/180.0)*Math.PI;
}

function onePane()
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();

  for(var i =1, len =taskList.getTaskCount(); i < len;i++)
  {
    var task = taskList.getTask(1)
    taskList.removeTask(task);
    task.delete();
  }

  var task = taskList.getTask(0)
  var w = 800;
  var h = 800;
  task.setViewportSize([w,h]);
  task.setViewportPosition([0,0]);
  task.delete();

  taskList.delete();
}

function threePane()
{
  onePane();

  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  var w = 800;
  var h = 800;
  var first = taskList.getTask(0);
  first.setViewportSize([w/2,h]);
  var camera = first.getCameraActor(); //  second.setCameraActor(second_ca);
  first.delete(); //wrapper

  var second = taskList.createTask();
  second.setViewportSize([w/2,h/2]);
  second.setViewportPosition([w/2,0]);
  var second_ca = new dali.CameraActor();
  second.setCameraActor(second_ca);
  second_ca.position = [ camera.position[2], 0, 0 ];
  second_ca.rotation = [0,1,0, 90];
  second_ca.delete(); // wrapper
  second.delete(); // wrapper

  var third = taskList.createTask();
  third.setViewportSize([w/2,h/2]);
  third.setViewportPosition([w/2,h/2]);
  // var third_ca = new dali.CameraActor();
  // third.setCameraActor(third_ca);
  // third_ca.position = [ camera.position[2], 0, 0 ];
  // third_ca.rotation = [0,1,0, 90];
  // third_ca.delete(); // wrapper
  third.delete(); // wrapper

  taskList.delete(); // wrapper

}


function first()
{
  return dali.getStage().getRenderTaskList().getTask(0);
}
function second()
{
  return dali.getStage().getRenderTaskList().getTask(1);
}
function third()
{
  return dali.getStage().getRenderTaskList().getTask(2);
}
function firstCamera()
{
  return dali.getStage().getRenderTaskList().getTask(0).getCameraActor();
}
function secondCamera()
{
  return dali.getStage().getRenderTaskList().getTask(1).getCameraActor();
}
function thirdCamera()
{
  return dali.getStage().getRenderTaskList().getTask(2).getCameraActor();
}

function resize(w,h)
{
  dali.setCanvasSize(w,h);
}

function matrixByMatrix(A,B)
{
  var a11 = A[ 0 ], a12 = A[ 4 ], a13 = A[ 8 ], a14 = A[ 12];
  var a21 = A[ 1 ], a22 = A[ 5 ], a23 = A[ 9 ], a24 = A[ 13];
  var a31 = A[ 2 ], a32 = A[ 6 ], a33 = A[ 10], a34 = A[ 14];
  var a41 = A[ 3 ], a42 = A[ 7 ], a43 = A[ 11], a44 = A[ 15];

  var b11 = B[ 0 ], b12 = B[ 4 ], b13 = B[ 8 ], b14 = B[ 12];
  var b21 = B[ 1 ], b22 = B[ 5 ], b23 = B[ 9 ], b24 = B[ 13];
  var b31 = B[ 2 ], b32 = B[ 6 ], b33 = B[ 10], b34 = B[ 14];
  var b41 = B[ 3 ], b42 = B[ 7 ], b43 = B[ 11], b44 = B[ 15];

  ret = [0,0,0,0,
         0,0,0,0,
         0,0,0,0,
         0,0,0,0];

  ret[0 ] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
  ret[4 ] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
  ret[8 ] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
  ret[12] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;

  ret[1 ] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
  ret[5 ] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
  ret[9 ] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
  ret[13] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;

  ret[2 ] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
  ret[6 ] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
  ret[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
  ret[14] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;

  ret[3 ] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
  ret[7 ] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
  ret[11] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
  ret[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;

  return ret;
}

function matrixByVector(A,v)
{
  var x = v[0];
  var y = v[1];
  var z = v[2];
  var w = 1;

  if(v.length == 4)
  {
    var w = v[3];
  }

  return [
    A[0 ] * x + A[4 ] * y + A[8 ] * z + A[12] * w,
    A[1 ] * x + A[5 ] * y + A[9 ] * z + A[13] * w,
    A[2 ] * x + A[6 ] * y + A[10] * z + A[14] * w,
    A[3 ] * x + A[7 ] * y + A[11] * z + A[15] * w
  ];
}


function matrixDeterminant(A)
{
  var n11 = A[0], n12 = A[4], n13 = A[8 ], n14 = A[12];
  var n21 = A[1], n22 = A[5], n23 = A[9 ], n24 = A[13];
  var n31 = A[2], n32 = A[6], n33 = A[10], n34 = A[14];
  var n41 = A[3], n42 = A[7], n43 = A[11], n44 = A[15];

  var m0 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
  var m4 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
  var m8 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
  var m12= n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

  return n11 * m0 + n21 * m4 + n31 * m8 + n41 * m12;
}

function matrixByScalar(A, s)
{
  ret = [0,0,0,0,
         0,0,0,0,
         0,0,0,0,
         0,0,0,0];

  ret[ 0 ] *= s; ret[ 4 ] *= s; ret[ 8 ] *= s; ret[ 12 ] *= s;
  ret[ 1 ] *= s; ret[ 5 ] *= s; ret[ 9 ] *= s; ret[ 13 ] *= s;
  ret[ 2 ] *= s; ret[ 6 ] *= s; ret[ 10 ] *= s; ret[ 14 ] *= s;
  ret[ 3 ] *= s; ret[ 7 ] *= s; ret[ 11 ] *= s; ret[ 15 ] *= s;

  return ret;
}

function matrixInverse(A)
{
  var n11 = A[0], n12 = A[4], n13 = A[8 ], n14 = A[12];
  var n21 = A[1], n22 = A[5], n23 = A[9 ], n24 = A[13];
  var n31 = A[2], n32 = A[6], n33 = A[10], n34 = A[14];
  var n41 = A[3], n42 = A[7], n43 = A[11], n44 = A[15];

  ret = [0,0,0,0,
         0,0,0,0,
         0,0,0,0,
         0,0,0,0];

  ret[0 ] = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
  ret[4 ] = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
  ret[8 ] = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
  ret[12] = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;
  ret[1 ] = n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44;
  ret[5 ] = n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44;
  ret[9 ] = n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44;
  ret[13] = n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34;
  ret[2 ] = n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44;
  ret[6 ] = n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44;
  ret[10] = n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44;
  ret[14] = n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34;
  ret[3 ] = n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43;
  ret[7 ] = n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43;
  ret[11] = n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43;
  ret[15] = n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33;

  var det = n11 * ret[ 0 ] + n21 * ret[ 4 ] + n31 * ret[ 8 ] + n41 * ret[ 12 ];

  if ( det == 0 ) {
    throw "no inverse";
    return nil;
  }

  matrixByScalar(ret, 1 / det );

  return ret;
}

function worldToScreen(camera, viewPortWidth, viewPortHeight, position)
{
  var mat = matrixByMatrix( camera.projectionMatrix, camera.viewMatrix );
  var p = matrixByVector( mat, position );
  p[0] += viewPortWidth/2;
  p[1] += viewPortHeight/2;
  return p;
}

function listProperties(actor)
{
  var props = actor.getProperties();
  for(var i=0, len = props.size(); i< len; i++)
  {
    console.log(props.get(i));
  }
}

function testit()
{
  clear();
  threeSquares2();
  col = {};
  collectByName(col);
  col.red.registerProperty("pasdf", 3.4);
}
