/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

//
// The javascript counterpart to the C++ DALi wrapper.
//
// Provides helper functionality to make the use of the dali module.
//
// Where possible it creates a natural Javascript API. One unavoidable
// problem is that wrapped C++ objects must be explicitly deleted
// '<obj>.delete()'. This is because javascript in the browser
// has no available hook to watch for an objects destruction.
//

// forward refs for lint
var Module;
var dali = Module;  // upgraded to emscripten 1.34.2 and -s EXPORT_NAME="dali" on emcc command line no longer seems to work?
var document;
var console;

//------------------------------------------------------------------------------
//
// Cache to fix the dali get/set thread issue
//
//------------------------------------------------------------------------------
dali.internalUniqueId = function() {
  "use strict";
};

dali.internalUniqueId.prototype._id = 0;
dali.internalUniqueId.prototype.generateId = function() {
  "use strict";
  return ++dali.internalUniqueId.prototype._id;
};

/** @private */
dali.internalPropertyCacheEnable = true;
dali.internalPropertyCache = {};

//------------------------------------------------------------------------------
//
// math funcitons
//
//------------------------------------------------------------------------------
dali.matrixByMatrix = function(A, B) {
  "use strict";

  var ret = [1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1
            ];

  ret[0] = A[0] * B[0] + A[1] * B[4] + A[2] * B[8] + A[3] * B[12];
  ret[4] = A[4] * B[0] + A[5] * B[4] + A[6] * B[8] + A[7] * B[12];
  ret[8] = A[8] * B[0] + A[9] * B[4] + A[10] * B[8] + A[11] * B[12];
  ret[12] = A[12] * B[0] + A[13] * B[4] + A[14] * B[8] + A[15] * B[12];

  ret[1] = A[0] * B[1] + A[1] * B[5] + A[2] * B[9] + A[3] * B[13];
  ret[5] = A[4] * B[1] + A[5] * B[5] + A[6] * B[9] + A[7] * B[13];
  ret[9] = A[8] * B[1] + A[9] * B[5] + A[10] * B[9] + A[11] * B[13];
  ret[13] = A[12] * B[1] + A[13] * B[5] + A[14] * B[9] + A[15] * B[13];

  ret[2] = A[0] * B[2] + A[1] * B[6] + A[2] * B[10] + A[3] * B[14];
  ret[6] = A[4] * B[2] + A[5] * B[6] + A[6] * B[10] + A[7] * B[14];
  ret[10] = A[8] * B[2] + A[9] * B[6] + A[10] * B[10] + A[11] * B[14];
  ret[14] = A[12] * B[2] + A[13] * B[6] + A[14] * B[10] + A[15] * B[14];

  ret[3] = A[0] * B[3] + A[1] * B[7] + A[2] * B[11] + A[3] * B[15];
  ret[7] = A[4] * B[3] + A[5] * B[7] + A[6] * B[11] + A[7] * B[15];
  ret[11] = A[8] * B[3] + A[9] * B[7] + A[10] * B[11] + A[11] * B[15];
  ret[15] = A[12] * B[3] + A[13] * B[7] + A[14] * B[11] + A[15] * B[15];

  return ret;
};

dali.matrixByVector = function(A, v) {
  "use strict";

  var x = v[0];
  var y = v[1];
  var z = v[2];
  var w = 1;

  if (v.length === 4) {
    w = v[3];
  }

  return [
    A[0] * x + A[4] * y + A[8] * z + A[12] * w,
    A[1] * x + A[5] * y + A[9] * z + A[13] * w,
    A[2] * x + A[6] * y + A[10] * z + A[14] * w,
    A[3] * x + A[7] * y + A[11] * z + A[15] * w
  ];
};

dali.matrixByVector2 = function(A, v) {
  "use strict";

  var x = v[0];
  var y = v[1];
  var z = v[2];
  var w = 1;

  if (v.length === 4) {
    w = v[3];
  }

  return [
    A[0] * x + A[1] * y + A[2] * z + A[3] * w,
    A[4] * x + A[5] * y + A[6] * z + A[7] * w,
    A[8] * x + A[9] * y + A[10] * z + A[11] * w,
    A[12] * x + A[13] * y + A[14] * z + A[15] * w
  ];
};


dali.matrixDeterminant = function(A) {
  "use strict";

  var n11 = A[0],
      n12 = A[4],
      n13 = A[8],
      n14 = A[12];
  var n21 = A[1],
      n22 = A[5],
      n23 = A[9],
      n24 = A[13];
  var n31 = A[2],
      n32 = A[6],
      n33 = A[10],
      n34 = A[14];
  var n41 = A[3],
      n42 = A[7],
      n43 = A[11],
      n44 = A[15];

  var m0 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
  var m4 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
  var m8 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
  var m12 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

  return n11 * m0 + n21 * m4 + n31 * m8 + n41 * m12;
};

dali.matrixByScalar = function(A, s) {
  "use strict";
  return [A[0] * s, A[1] * s, A[2] * s, A[3] * s,
          A[4] * s, A[5] * s, A[6] * s, A[7] * s,
          A[8] * s, A[9] * s, A[10] * s, A[11] * s,
          A[12] * s, A[13] * s, A[14] * s, A[15] * s
         ];
};

dali.matrixInverse = function(A) {
  "use strict";
  var ret = [1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1
            ];

  ret[0] = A[5] * A[10] * A[15] - A[5] * A[11] * A[14] - A[9] * A[6] * A[15] + A[9] * A[7] * A[14] + A[13] * A[6] * A[11] - A[13] * A[7] * A[10];
  ret[4] = -A[4] * A[10] * A[15] + A[4] * A[11] * A[14] + A[8] * A[6] * A[15] - A[8] * A[7] * A[14] - A[12] * A[6] * A[11] + A[12] * A[7] * A[10];
  ret[8] = A[4] * A[9] * A[15] - A[4] * A[11] * A[13] - A[8] * A[5] * A[15] + A[8] * A[7] * A[13] + A[12] * A[5] * A[11] - A[12] * A[7] * A[9];
  ret[12] = -A[4] * A[9] * A[14] + A[4] * A[10] * A[13] + A[8] * A[5] * A[14] - A[8] * A[6] * A[13] - A[12] * A[5] * A[10] + A[12] * A[6] * A[9];

  ret[1] = -A[1] * A[10] * A[15] + A[1] * A[11] * A[14] + A[9] * A[2] * A[15] - A[9] * A[3] * A[14] - A[13] * A[2] * A[11] + A[13] * A[3] * A[10];
  ret[5] = A[0] * A[10] * A[15] - A[0] * A[11] * A[14] - A[8] * A[2] * A[15] + A[8] * A[3] * A[14] + A[12] * A[2] * A[11] - A[12] * A[3] * A[10];
  ret[9] = -A[0] * A[9] * A[15] + A[0] * A[11] * A[13] + A[8] * A[1] * A[15] - A[8] * A[3] * A[13] - A[12] * A[1] * A[11] + A[12] * A[3] * A[9];
  ret[13] = A[0] * A[9] * A[14] - A[0] * A[10] * A[13] - A[8] * A[1] * A[14] + A[8] * A[2] * A[13] + A[12] * A[1] * A[10] - A[12] * A[2] * A[9];

  ret[2] = A[1] * A[6] * A[15] - A[1] * A[7] * A[14] - A[5] * A[2] * A[15] + A[5] * A[3] * A[14] + A[13] * A[2] * A[7] - A[13] * A[3] * A[6];
  ret[6] = -A[0] * A[6] * A[15] + A[0] * A[7] * A[14] + A[4] * A[2] * A[15] - A[4] * A[3] * A[14] - A[12] * A[2] * A[7] + A[12] * A[3] * A[6];
  ret[10] = A[0] * A[5] * A[15] - A[0] * A[7] * A[13] - A[4] * A[1] * A[15] + A[4] * A[3] * A[13] + A[12] * A[1] * A[7] - A[12] * A[3] * A[5];
  ret[14] = -A[0] * A[5] * A[14] + A[0] * A[6] * A[13] + A[4] * A[1] * A[14] - A[4] * A[2] * A[13] - A[12] * A[1] * A[6] + A[12] * A[2] * A[5];

  ret[3] = -A[1] * A[6] * A[11] + A[1] * A[7] * A[10] + A[5] * A[2] * A[11] - A[5] * A[3] * A[10] - A[9] * A[2] * A[7] + A[9] * A[3] * A[6];
  ret[7] = A[0] * A[6] * A[11] - A[0] * A[7] * A[10] - A[4] * A[2] * A[11] + A[4] * A[3] * A[10] + A[8] * A[2] * A[7] - A[8] * A[3] * A[6];
  ret[11] = -A[0] * A[5] * A[11] + A[0] * A[7] * A[9] + A[4] * A[1] * A[11] - A[4] * A[3] * A[9] - A[8] * A[1] * A[7] + A[8] * A[3] * A[5];
  ret[15] = A[0] * A[5] * A[10] - A[0] * A[6] * A[9] - A[4] * A[1] * A[10] + A[4] * A[2] * A[9] + A[8] * A[1] * A[6] - A[8] * A[2] * A[5];

  var det = A[0] * ret[0] + A[1] * ret[4] + A[2] * ret[8] + A[3] * ret[12];

  if (det === 0) {
    throw "no inverse";
  }

  return dali.matrixByScalar(ret, 1 / det);
};

dali.degree = function(radians) {
  "use strict";
  return (radians * 180.0) / Math.PI;
};

dali.radian = function(degrees) {
  "use strict";
  return (degrees / 180.0) * Math.PI;
};

dali.vectorLength = function(array) {
  "use strict";
  var N = 3; // array.length;

  var length = 0;
  for (var i = 0; i < N; ++i) {
    length += array[i] * array[i];
  }
  return Math.sqrt(length);
};

dali.vectorLengthSquared = function(array) {
  "use strict";
  var N = 3; // array.length;

  var length = 0;
  for (var i = 0; i < N; ++i) {
    length += array[i] * array[i];
  }
  return length;
};

dali.normalize = function(array) {
  "use strict";
  var N = 3; // array.length;

  var length = 0;
  for (var i = 0; i < 3; ++i) {
    length += array[i] * array[i];
  }
  length = Math.sqrt(length);

  if (length !== 0) {
    var ret = [];
    for (i = 0; i < N; ++i) {
      ret.push(array[i] / length);
    }
    for (i = N; i < array.length; ++i) {
      ret.push(array[i]);
    }
    return ret;
  } else {
    return array;
  }

};

dali.axisAngleToQuaternion = function(axisAngle) {
  "use strict";
  var an = dali.normalize(axisAngle);
  var angle = axisAngle[axisAngle.length - 1];
  var halfTheta = angle * 0.5;
  var sinThetaByTwo = Math.sin(halfTheta);
  var cosThetaByTwo = Math.cos(halfTheta);
  return [an[0] * sinThetaByTwo,
          an[1] * sinThetaByTwo,
          an[2] * sinThetaByTwo,
          cosThetaByTwo
         ];
};

dali.vectorDot = function(v1, v2) {
  "use strict";
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
};

dali.vectorDot4 = function(v1, v2) {
  "use strict";
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
};

dali.vectorCross = function(v1, v2) {
  "use strict";
  var v1X = v1[0];
  var v1Y = v1[1];
  var v1Z = v1[2];

  var v2X = v2[0];
  var v2Y = v2[1];
  var v2Z = v2[2];

  return [v1Y * v2Z - v1Z * v2Y,
          v1Z * v2X - v1X * v2Z,
          v1X * v2Y - v1Y * v2X
         ];
};

dali.vectorByScalar = function(v1, s) {
  "use strict";
  var ret = [];
  for (var i = 0, len = v1.length; i < len; i++) {
    ret.push(v1[i] * s);
  }
  return ret;
};

dali.vectorAdd = function() {
  "use strict";
  var ret = arguments[0];
  var l = ret.length;
  for (var i = 1, len = arguments.length; i < len; i++) {
    var v = arguments[i];
    for (var j = 0; j < l; j++) {
      ret[j] += v[j];
    }
  }
  return ret;
};

dali.quatByQuat = function(q1, q2) {
  "use strict";
  var q1X = q1[0];
  var q1Y = q1[1];
  var q1Z = q1[2];
  var q1W = q1[3];

  var q2X = q2[0];
  var q2Y = q2[1];
  var q2Z = q2[2];
  var q2W = q2[3];

  return [q1Y * q2Z - q1Z * q2Y + q1W * q2X + q1X * q2W,
          q1Z * q2X - q1X * q2Z + q1W * q2Y + q1Y * q2W,
          q1X * q2Y - q1Y * q2X + q1W * q2Z + q1Z * q2W,
          q1W * q2W - dali.vectorDot(q1, q2)
         ];
};

dali.quaternionToAxisAngle = function(q) {
  "use strict";
  var angle = Math.acos(q[3]);
  var sine = Math.sin(angle);

  if (sine === 0.0) {
    throw "Cannot convert quaternion";
  }

  var sinfThetaInv = 1.0 / sine;

  return [q[0] * sinfThetaInv,
          q[1] * sinfThetaInv,
          q[2] * sinfThetaInv,
          angle * 2.0
         ];
};

dali.quaternionToMatrix = function(q) {
  "use strict";
  var x = q[0];
  var y = q[1];
  var z = q[2];
  var w = q[3];
  var xx = x * x;
  var yy = y * y;
  var zz = z * z;
  var xy = x * y;
  var xz = x * z;
  var wx = w * x;
  var wy = w * y;
  var wz = w * z;
  var yz = y * z;

  var m = [1, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1];

  m[0] = 1.0 - 2.0 * (yy + zz);
  m[1] = 2.0 * (xy + wz);
  m[2] = 2.0 * (xz - wy);
  m[3] = 0.0;

  m[4] = 2.0 * (xy - wz);
  m[5] = 1.0 - 2.0 * (xx + zz);
  m[6] = 2.0 * (yz + wx);
  m[7] = 0.0;

  m[8] = 2.0 * (xz + wy);
  m[9] = 2.0 * (yz - wx);
  m[10] = 1.0 - 2.0 * (xx + yy);
  m[11] = 0.0;

  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;

  return m;
};

dali.worldToScreen = function(position, renderTask) {
  "use strict";
  var useFirstRenderTask = false;

  if (typeof renderTask === "undefined") {
    useFirstRenderTask = true;
  } else if (renderTask === null) { // null is an object
    useFirstRenderTask = true;
  }

  if (useFirstRenderTask) {
    var tasks = dali.stage.getRenderTaskList();
    renderTask = tasks.getTask(0);
    tasks.delete(); // wrapper
  }

  var camera = renderTask.getCameraActor();
  var pos = renderTask.getCurrentViewportPosition();
  var size = renderTask.getCurrentViewportSize();

  var mat = dali.matrixByMatrix(camera.viewMatrix, camera.projectionMatrix);
  var p = dali.matrixByVector(mat, position);
  var depthRange = [0, 1];
  var viewport = [pos[0], pos[1], size[0], size[1]];

  var div = 1 / p[3]; // homogenous divide

  camera.delete(); // wrapper

  return [
    (1 + p[0] * div) * viewport[2] / 2 + viewport[0], (1 - p[1] * div) * viewport[3] / 2 + viewport[1], (p[2] * div) * (depthRange[1] - depthRange[0]) + depthRange[0],
    div
  ];
};


dali.matrixIdentity = function() {
  "use strict";
  return [1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1
         ];
};

dali.matrixTransform = function(x, y, z) {
  "use strict";
  return [1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          x, y, z, 1
         ];
};

dali.screenToPlaneLocal = function(screenX, screenY, renderTask, planeOrientationMatrix, planeWidth, planeHeight) {
  "use strict";

  var camera = renderTask.getCameraActor();

  var pos = renderTask.getCurrentViewportPosition();
  var size = renderTask.getCurrentViewportSize();
  var viewportX = pos[0];
  var viewportY = pos[1];
  var viewportW = size[0];
  var viewportH = size[1];
  var modelView = dali.matrixByMatrix(planeOrientationMatrix, camera.viewMatrix);

  var inverseMvp = dali.matrixInverse(
    dali.matrixByMatrix(modelView, camera.projectionMatrix));

  var screenPos = [screenX - viewportX,
                   viewportH - (screenY - viewportY),
                   0.0, 1.0
                  ];

  screenPos[2] = 0.0;

  var oglScreenPos = [(screenPos[0] / viewportW) * 2 - 1, (screenPos[1] / viewportH) * 2 - 1, (screenPos[2]) * 2 - 1,
                      1
                     ];


  var nearPoint = dali.matrixByVector(inverseMvp, oglScreenPos);

  if (nearPoint[3] === 0.0) {
    throw "Unproject near fails";
  }

  nearPoint[3] = 1 / nearPoint[3];
  nearPoint[0] = nearPoint[0] * nearPoint[3];
  nearPoint[1] = nearPoint[1] * nearPoint[3];
  nearPoint[2] = nearPoint[2] * nearPoint[3];


  oglScreenPos[2] = 1.0 * 2 - 1;

  var farPoint = dali.matrixByVector(inverseMvp, oglScreenPos);

  if (farPoint[3] === 0.0) {
    throw "Unproject far fails";
  }

  farPoint[3] = 1 / farPoint[3];
  farPoint[0] = farPoint[0] * farPoint[3];
  farPoint[1] = farPoint[1] * farPoint[3];
  farPoint[2] = farPoint[2] * farPoint[3];

  if (!(farPoint[2] < 0) && (nearPoint[2] > 0)) {
    throw "ray not crossing xy plane";
  }

  var dist = nearPoint[2] / (nearPoint[2] - farPoint[2]);

  var intersect = [nearPoint[0] + (farPoint[0] - nearPoint[0]) * dist,
                   nearPoint[1] + (farPoint[1] - nearPoint[1]) * dist,
                   0.0
                  ];

  intersect[0] = intersect[0] + planeWidth * 0.5;
  intersect[1] = intersect[1] + planeHeight * 0.5;

  return intersect;
};

dali.screenToLocal = function(screenX, screenY, actor, renderTask) {
  "use strict";
  return dali.screenToPlaneLocal(screenX, screenY, renderTask, actor.worldMatrix, actor.width, actor.height);
};

dali.screenToXY = function(screenX, screenY, actor, renderTask) {
  "use strict";
  var size = dali.stage.getSize();
  return dali.screenToPlaneLocal(screenX, screenY,
                                 renderTask,
                                 dali.matrixIdentity(),
                                 size[0],
                                 size[1]);
};

dali.screenToYZ = function(screenX, screenY, actor, renderTask) {
  "use strict";
  var size = dali.stage.getSize();
  var q = dali.axisAngleToQuaternion( [0, 1, 0, dali.radian(90)] );
  return dali.screenToPlaneLocal(screenX, screenY,
                                 renderTask,
                                 dali.quaternionToMatrix(q),
                                 size[0],
                                 size[1]);
};

dali.screenToXZ = function(screenX, screenY, actor, renderTask) {
  "use strict";
  var size = dali.stage.getSize();
  var q = dali.axisAngleToQuaternion( [1, 0, 0, dali.radian(90)] );
  return dali.screenToPlaneLocal(screenX, screenY,
                                 renderTask,
                                 dali.quaternionToMatrix(q),
                                 size[0],
                                 size[1]);
};


dali.screenCoordinates = function(actor, renderTask) {
  "use strict";
  var size = actor.size;
  var w2 = size[0] / 2;
  var h2 = size[1] / 2;
  var actorWorldMatrix = actor.worldMatrix;
  var actorWorldPosition = actor.worldPosition;

  return {
    topLeft: dali.worldToScreen(dali.matrixByVector(actorWorldMatrix, [-w2, -h2, 0]), renderTask),
    topRight: dali.worldToScreen(dali.matrixByVector(actorWorldMatrix, [+w2, -h2, 0]), renderTask),
    bottomRight: dali.worldToScreen(dali.matrixByVector(actorWorldMatrix, [+w2, +h2, 0]), renderTask),
    bottomLeft: dali.worldToScreen(dali.matrixByVector(actorWorldMatrix, [-w2, +h2, 0]), renderTask),
    centre: dali.worldToScreen(actorWorldPosition, renderTask)
  };
};

dali.screenToActor = function(actor, screenPos, renderTask) {
  "use strict";
  // , function will return coordinates in relation to actor`s anchorPoint (client coordinates).
  var useFirstRenderTask = false;

  if (typeof renderTask === "undefined") {
    useFirstRenderTask = true;
  } else if (renderTask === null) { // null is an object
    useFirstRenderTask = true;
  }

  if (useFirstRenderTask) {
    var tasks = dali.stage.getRenderTaskList();
    renderTask = tasks.getTask(0);
    tasks.delete(); // wrapper
  }

  var camera = renderTask.getCameraActor();
  var vpp = renderTask.getCurrentViewportPosition();
  var vps = renderTask.getCurrentViewportSize();

  var mat = dali.matrixByMatrix(camera.projectionMatrix, camera.viewMatrix);

  var inverseMvp = dali.matrixInverse(mat);

  var x = screenPos[0];
  var y = screenPos[1];
  var z = screenPos[2];

  var objectPos = dali.matrixByVector(inverseMvp,
                                      // normalized +-1
                                      [((x - vpp[0]) / vps[0]) * 2.0 - 1.0, ((y - vpp[1]) / vps[1]) * 2.0 - 1.0, (z * 2.0) - 1.0,
                                       1.0
                                      ]);

  if (objectPos[3] === 0.0) {
    throw "Cannot find screen Position";
  }

  objectPos[3] = 1 / objectPos[3];

  return [objectPos[0] * objectPos[3],
          objectPos[1] * objectPos[3],
          objectPos[2] * objectPos[3],
          objectPos[3]
         ];
};


//------------------------------------------------------------------------------
//
// Callbacks for data requested by dali from the browser environment
//
//------------------------------------------------------------------------------
/** @private */
dali.requestedGlyphImage = function(sFontFamily, sFontStyle, fFontSize, iChar) {
  "use strict";

  var buffer = document.createElement("canvas");
  buffer.width = fFontSize;
  buffer.height = fFontSize;
  var ctx = buffer.getContext("2d");

  ctx.font = sFontStyle + " " + fFontSize + "px " + sFontFamily;
  ctx.fillText(String.fromCharCode(iChar), 0, 0 + fFontSize);

  var imageData = ctx.getImageData(0, 0, fFontSize, fFontSize);

  // emscripten checks only for this type if array in converting to strings
  // (getImageData() returns Uint8CheckedArray or some such)
  //  var uint8array = new Uint8Array( imageData.data );

  return imageData.data; // return uint8array; //
};

/** @private */
dali.requestedImage = function() {
  "use strict";
  console.log("GetImage called:");
};

/** @private */
dali.requestedImageMetadata = function() {
  "use strict";
  console.log("GetImageMetadata called");
};

/** @private */
dali.postRenderFunction = undefined;

dali.requestedRenderFinished = function() {
  "use strict";
  // reset the temporary cache
  dali.internalPropertyCache = {};
  if(dali.postRenderFunction) {
    dali.postRenderFunction();
  }
};


dali.setCallbackGetGlyphImage(dali.requestedGlyphImage);
dali.setCallbackGetImage(dali.requestedImage);
dali.setCallbackGetImageMetadata(dali.requestedImageMetadata);
dali.setCallbackRenderFinished(dali.requestedRenderFinished);


//------------------------------------------------------------------------------
//
// Property handling and JS<->C++ data marshalling
//
// Javascript objects are adorned with dali properties as they are created or
// fetched from the C++ api
//
// Data is marshalled to avoid some of the necessary C++ memory management for
// small property classes.
//
//------------------------------------------------------------------------------

// Heuristic conversion of Javascript value to dali property
dali.DaliPropertyValue = function(object, name, value) {
  "use strict";

  var setProperty = false;
  if (typeof (value) === "boolean") {
    setProperty = new dali.PropertyBoolean(value);
  } else if (typeof (value) === "number") {
    setProperty = new dali.PropertyFloat(value);
  } else if (typeof (value) === "string") {
    setProperty = new dali.PropertyString(value);
  } else if (typeof (value) === "object") {
    if (value.constructor === dali.PropertyValue) {
      setProperty = value;
    } else {
      var type = false;
      if (object) {
        var p = object.getProperty(name);
        type = p.getType();
        p.delete();
      }
      // This shouldnt need .value on the end? Maybe because I'm currently in
      // emscritpen devel branch and it will change
      if (type === dali.PropertyType.ROTATION.value) {
        if (value.length === 3) {
          setProperty = new dali.PropertyEuler(value);
        } else {
          setProperty = new dali.PropertyAxisAngle(value);
        }
      } else if (value.length) {
        if (type === dali.PropertyType.ARRAY.value) {
          setProperty = new dali.PropertyArray(value);
        } else {
          if (value.length === 2) {
            setProperty = new dali.PropertyVector2(value);
          } else if (value.length === 3) {
            //console.log("DaliPropertyValue set property" + value );
            setProperty = new dali.PropertyVector3(value);
          } else if (value.length === 4) {
            setProperty = new dali.PropertyVector4(value);
          } else if (value.length === 9) {
            setProperty = new dali.PropertyMatrix3(value);
          } else if (value.length === 16) {
            setProperty = new dali.PropertyMatrix(value);
          } else {
            throw new Error("Cannot set property");
          }
        }
      } else {
        // todo; I think a map has a length....
        setProperty = new dali.PropertyMap(value);
      }
    }
  } else {
    if (object) {
      throw object.toString() + " " + name.toString() + " " + value.toString();
    } else {
      throw name.toString() + " " + value.toString();
    }
  }

  return setProperty;
};

/** @private */
dali.marshallProperty = function(p) {
  "use strict";

  var ret;
  var type = p.getType();

  if (type === 0) {
    // raise?
    throw new Error("Property has no type?");
  } else if (type === dali.PropertyType.BOOLEAN.value) {
    ret = p.getBoolean();
  } else if (type === dali.PropertyType.FLOAT.value) {
    ret = p.getFloat();
  } else if (type === dali.PropertyType.INTEGER.value) {
    ret = p.getInteger();
  } else if (type === dali.PropertyType.VECTOR2.value) {
    ret = p.getVector2();
  } else if (type === dali.PropertyType.VECTOR3.value) {
    ret = p.getVector3();
  } else if (type === dali.PropertyType.VECTOR4.value) {
    // VECTOR4
    ret = p.getVector4();
  } else if (type === dali.PropertyType.MATRIX3.value) {
    // MATRIX3
    ret = p.getMatrix3();
  } else if (type === dali.PropertyType.MATRIX.value) {
    // MATRIX
    ret = p.getMatrix();
  // } else if( type === dali.PropertyType.RECTANGLE )
  // {
  //   ret = p.getMatrix();
  } else if (type === dali.PropertyType.ROTATION.value) {
    ret = p.getRotation();
  } else if (type === dali.PropertyType.STRING.value) {
    ret = p.getString();
  } else if (type === dali.PropertyType.ARRAY.value) {
    ret = p.getArray();
  } else if (type === dali.PropertyType.MAP.value) {

    ret = p.getMap();
  }
  p.delete();
  return ret;
};

/** @private */
dali.marshallSetProperty = function(object, name, value) {
  "use strict";

  var setProperty = new dali.DaliPropertyValue(object, name, value);

  if (setProperty) {
    object.setProperty(name, setProperty);
    //console.log("marshallSetProperty set property" + setProperty );
    setProperty.delete();

    if(dali.internalPropertyCacheEnable) {
      // set in cache to fix dali get/set problem
      var uniqueId = object._uniqueId;
      if (uniqueId !== undefined) {
        var objectDict = dali.internalPropertyCache[uniqueId];
        if (objectDict === undefined) {
          dali.internalPropertyCache[uniqueId] = {};
        }
        dali.internalPropertyCache[uniqueId][name] = value;
      }
    }
  }
};

/** @private */
dali.marshallGetProperty = function(object, name) {
  "use strict";

  if(dali.internalPropertyCacheEnable) {
    // is the value live in the cache? if so use that value instead
    var uniqueId = object._uniqueId;
    if (uniqueId !== undefined) {
      var objectDict = dali.internalPropertyCache[uniqueId];
      if (objectDict !== undefined) {
        var value = dali.internalPropertyCache[uniqueId][name];
        if (value !== undefined) {
          return value;
        }
      }
    }
  }

  var ret;
  var p = object.getProperty(name);
  if (!p) {
    throw new Error("Property doesnt exist?");
  }
  var type = p.getType();

  if (type === 0) {
    // raise?
    throw new Error("Property has no type?");
  } else if (type === dali.PropertyType.BOOLEAN.value) {
    ret = p.getBoolean();
  } else if (type === dali.PropertyType.FLOAT.value) {
    ret = p.getFloat();
  } else if (type === dali.PropertyType.INTEGER.value) {
    ret = p.getInteger();
  } else if (type === dali.PropertyType.VECTOR2.value) {
    ret = p.getVector2();
  } else if (type === dali.PropertyType.VECTOR3.value) {
    ret = p.getVector3();
  } else if (type === dali.PropertyType.VECTOR4.value) {
    // VECTOR4
    ret = p.getVector4();
  } else if (type === dali.PropertyType.MATRIX3.value) {
    // MATRIX3
    ret = p.getMatrix3();
  } else if (type === dali.PropertyType.MATRIX.value) {
    // MATRIX
    ret = p.getMatrix();
  // } else if( type === dali.PropertyType.RECTANGLE )
  // {
  //   ret = p.getMatrix();
  } else if (type === dali.PropertyType.ROTATION.value) {
    ret = p.getRotation();
  } else if (type === dali.PropertyType.STRING.value) {
    ret = p.getString();
  } else if (type === dali.PropertyType.ARRAY.value) {
    ret = p.getArray();
  } else if (type === dali.PropertyType.MAP.value) {

    ret = p.getMap();
  }
  p.delete();
  return ret;
};


/** @private */
dali.internalSetupProperties = function(handle) {
  "use strict";
  if(handle.ok()) {
    var props = handle.getProperties();

    for (var i = 0; i < props.size(); i++) {
      // anon function because of closure with defineProperty
      // (if just variable in loop then the variable 'address' is captured, not the value
      //  so it becomes last value set)
      (function(name, object) {
        // @todo Dali error?? name lengths should never be zero
        if (name.length) {
          Object.defineProperty(handle, name, {
            enumerable: true,
            configurable: false,
            get: function() {
              return dali.marshallGetProperty(handle, name);
            },
            set: function(newValue) {
              dali.marshallSetProperty(handle, name, newValue);
            }
          });
        }
      })(props.get(i), handle);
    }

    handle._uniqueId = dali.internalUniqueId.prototype.generateId();
  }

  return handle;
};

//------------------------------------------------------------------------------
//
// Stage api
//
//------------------------------------------------------------------------------
dali.StageWrapper.prototype.add = function(actor) {
  "use strict";
  this.__add(actor);
};

dali.StageWrapper.prototype.remove = function(actor) {
  "use strict";
  this.__remove(actor);
};

dali.StageWrapper.prototype.getRootLayer = function() {
  "use strict";
  var wrapper = this.__getRootLayer();
  dali.internalSetupProperties(wrapper);
  return wrapper;
};

dali.StageWrapper.prototype.getLayer = function(index) {
  "use strict";
  var wrapper = this.__getLayer(index);
  dali.internalSetupProperties(wrapper);
  return wrapper;
};


dali.StageWrapper.prototype.getRenderTaskList = function() {
  "use strict";
  var wrapper = this.__getRenderTaskList();
  wrapper.createTask = function() {
    var t = wrapper.__createTask();
    return t;
  };
  wrapper.getTask = function(id) {
    var t = wrapper.__getTask(id);
    return t;
  };
  return wrapper;
};

dali.StageWrapper.prototype.setBackgroundColor = function(vector4) {
  "use strict";
  return this.__setBackgroundColor(vector4);
};

//------------------------------------------------------------------------------
//
// HandleWrapper api
//
//------------------------------------------------------------------------------
dali.HandleWrapper.prototype.registerProperty = function(name, value) {
  var ret = -1;

  var propertyValue = new dali.DaliPropertyValue(null, name, value);
  ret = this.__registerProperty(name, propertyValue);
  propertyValue.delete(); // wrapper
  Object.defineProperty(this, name, {
    enumerable: true,
    configurable: false,
    get: function() {
      return dali.marshallGetProperty(this, name);
    },
    set: function(newValue) {
      dali.marshallSetProperty(this, name, newValue);
    }
  });

  return ret;
};

dali.HandleWrapper.prototype.registerAnimatedProperty = function(name, value) {
  var ret = -1;

  var propertyValue = new dali.DaliPropertyValue(null, name, value);
  ret = this.__registerAnimatedProperty(name, propertyValue);
  propertyValue.delete(); // wrapper
  Object.defineProperty(this, name, {
    enumerable: true,
    configurable: false,
    get: function() {
      return dali.marshallGetProperty(this, name);
    },
    set: function(newValue) {
      dali.marshallSetProperty(this, name, newValue);
    }
  });
  return ret;
};


//------------------------------------------------------------------------------
//
// ActorWrapper api
//
//------------------------------------------------------------------------------
dali.ActorWrapper.prototype.getParent = function() {
  "use strict";
  var bareActorWrapper = this.__getParent();
  if(!bareActorWrapper.ok()) {
    bareActorWrapper.delete(); // wrapper
    bareActorWrapper = null;
  } else {
    // add properties to the bare ActorWrapper
    dali.internalSetupProperties(bareActorWrapper);
  }
  return bareActorWrapper;
};

dali.ActorWrapper.prototype.findChildById = function(index) {
  "use strict";
  var bareActorWrapper = this.__findChildById(index);
  if(!bareActorWrapper.ok()) {
    bareActorWrapper.delete(); // wrapper
    bareActorWrapper = null;
  } else {
    dali.internalSetupProperties(bareActorWrapper);
  }
  return bareActorWrapper;
};

dali.ActorWrapper.prototype.findChildByName = function(name) {
  "use strict";
  var bareActorWrapper = this.__findChildByName(name);
  if(!bareActorWrapper.ok()) {
    bareActorWrapper.delete(); // wrapper
    bareActorWrapper = null;
  } else {
    dali.internalSetupProperties(bareActorWrapper);
  }
  return bareActorWrapper;
};

dali.ActorWrapper.prototype.getChildAt = function(index) {
  "use strict";
  var bareActorWrapper = this.__getChildAt(index);
  if(!bareActorWrapper.ok()) {
    bareActorWrapper.delete(); // wrapper
    bareActorWrapper = null;
  } else {
    dali.internalSetupProperties(bareActorWrapper);
  }
  return bareActorWrapper;
};

/** @private */
dali.internalDepthFirstCollection = function(actor, collection) {
  "use strict";
  for (var i = 0; i < actor.getChildCount(); i++) {
    var a = actor.getChildAt(i); // adds properties in dotted
    collection.push(a);
    dali.internalDepthFirstCollection(a, collection);
  }
};

dali.ActorWrapper.prototype.findAllChildren = function() {
  "use strict";
  var col = [];
  dali.internalDepthFirstCollection(this, col);
  return col;
};

dali.ActorWrapper.prototype.getChildren = function() {
  "use strict";
  var col = [];
  for (var i = 0, len = this.getChildCount(); i < len; i++) {
    var c = this.getChildAt(i);
    col.push(c);
  }
  return col;
};
dali.ActorWrapper.prototype.directChildren = dali.ActorWrapper.prototype.getChildren;

dali.ActorWrapper.prototype.connect = function(signalName, callback) {
  "use strict";
  // wrap in closure so we can setup properties in . notation
  // and add actor methods to c++ raw ActorWrapper
  return this.__connect(
    signalName, (function(cb) {
      return function() {
        var args = [dali.internalSetupProperties(arguments[0])];
        for(var i = 1; i < arguments.length; i++) {
          args.push( arguments[i] );
        }
        cb.apply(null, args);
      };
    })(callback)
  );
};

dali.ActorWrapper.prototype.setImageData = function(imageData) {
  "use strict";
  // imageData as returned from canvas.getImageData
  var uint8array = new Uint8Array(imageData.data);
  this.__setImageRawRGBA(imageData.width, imageData.height, uint8array);
};

dali.ActorWrapper.prototype.setImageDataRGB = function(imageData) {
  "use strict";
  // imageData as returned from canvas.getImageData
  var uint8array = new Uint8Array(imageData.data);
  this.__setImageRawRGB(imageData.width, imageData.height, uint8array);
};

// convenience set
dali.ActorWrapper.prototype.setShader = function(shaderOptions) {
  "use strict";
  var shaderEffect = new dali.ShaderEffect(shaderOptions);
  this.setShaderEffect(shaderEffect);
  shaderEffect.delete(); // the wrapper
};

dali.ActorWrapper.prototype.getShader = function() {
  "use strict";
  var shaderEffect = this.__getShaderEffect();
  if(shaderEffect) {
    dali.internalSetupProperties(shaderEffect);
  }
  return shaderEffect;
};


//------------------------------------------------------------------------------
//
// Animation api
//
//------------------------------------------------------------------------------
dali.AnimationWrapper.prototype.animateTo = function(object, propertyName, value, alphaFunction, delay, duration) {
  "use strict";
  var propertyValue = new dali.DaliPropertyValue(object, propertyName, value);
  if (propertyValue) {
    this.__animateTo(object, propertyName, propertyValue, alphaFunction, delay, duration);
    propertyValue.delete();
  } else {
    throw new Error("Unknown property?");
  }
};

dali.AnimationWrapper.prototype.animateBy = function(object, propertyName, value, alphaFunction, delay, duration) {
  "use strict";
  var propertyValue = new dali.DaliPropertyValue(object, propertyName, value);
  if (propertyValue) {
    this.__animateBy(object, propertyName, propertyValue, alphaFunction, delay, duration);
    propertyValue.delete();
  } else {
    throw new Error("Unknown property?");
  }
};

dali.AnimationWrapper.prototype.animatePath = function(object, pathObject, forward, alphaFunction, delay, duration) {
  "use strict";
  this.__animatePath(object, pathObject, forward, alphaFunction, delay, duration);
};



//------------------------------------------------------------------------------
//
// RenderTask api
//
//------------------------------------------------------------------------------
dali.RenderTaskWrapper.prototype.getCameraActor = function() {
  "use strict";
  var a = this.__getCameraActor();
  if (a.ok()) {
    dali.internalSetupProperties(a);
  }
  return a;
};

Object.defineProperty(dali.RenderTaskWrapper.prototype, "x", {
  enumerable: true,
  configurable: false,
  get: function() {
    return this.getCurrentViewportPosition()[0];
  },
  set: function(v) {
    var pos = this.getCurrentViewportPosition();
    this.setViewportPosition(v, pos[1]);
  }
});

Object.defineProperty(dali.RenderTaskWrapper.prototype, "y", {
  enumerable: true,
  configurable: false,
  get: function() {
    return this.getCurrentViewportPosition()[1];
  },
  set: function(v) {
    var pos = this.getCurrentViewportPosition();
    this.setViewportPosition(pos[0], v);
  }
});

Object.defineProperty(dali.RenderTaskWrapper.prototype, "width", {
  enumerable: true,
  configurable: false,
  get: function() {
    return this.getCurrentViewportSize()[0];
  },
  set: function(v) {
    var pos = this.getCurrentViewportSize();
    this.setViewportSize(v, pos[1]);
  }
});

Object.defineProperty(dali.RenderTaskWrapper.prototype, "height", {
  enumerable: true,
  configurable: false,
  get: function() {
    return this.getCurrentViewportSize()[1];
  },
  set: function(v) {
    var pos = this.getCurrentViewportSize();
    this.setViewportSize(pos[0], v);
  }
});


//------------------------------------------------------------------------------
//
// Solid Actor
//
//------------------------------------------------------------------------------
dali.createSolidColorActor = function(color, border, borderColor, borderSize) {
  "use strict";
  var a = dali.__createSolidColorActor(color, border, borderColor, borderSize);
  dali.internalSetupProperties(a);
  return a;
};



//------------------------------------------------------------------------------
//
// Mesh suppport
//
//------------------------------------------------------------------------------
function ObjectLoader(fileObject) {
  "use strict";
  // cached
  this.self = this;
  this.meshByUUID = {};
  this.geomByUUID = {};
  this.matByUUID = {};

  this.fileObject = fileObject;
}

function __longToArray(v) {
  "use strict";
  return [((v >> 24) & 0xFF) / 255.0, ((v >> 16) & 0xFF) / 255.0, ((v >> 8) & 0xFF) / 255.0, (v & 0xFF) / 255.0];
}

function __isBitSet(value, bit) {
  "use strict";
  return (value & (1 << bit));
}

ObjectLoader.prototype.__getMaterial = function(uuid) {
  "use strict";
  if (!(uuid in this.matByUUID)) {
    for (var i = 0, len = this.fileObject.materials.length; i < len; i++) {
      var f_mat = this.fileObject["materials"][i];
      skewer.log(i + ":" + f_mat["uuid"] + " " + (f_mat["uuid"] === uuid));
      if (f_mat["uuid"] === uuid) {
        assert(f_mat["type"] === "MeshPhongMaterial");
        var mat = new dali.MaterialWrapper(uuid);
        mat.setDiffuseColor(__longToArray(f_mat["color"]));
        mat.setAmbientColor(__longToArray(f_mat["ambient"]));
        mat.setSpecularColor(__longToArray(f_mat["specular"]));
        mat.setEmissiveColor(__longToArray(f_mat["emmissive"]));
        mat.setShininess(f_mat["shininess"]);
        this.matByUUID[uuid] = mat;
        break;
      }
    }
  }
  return this.matByUUID[uuid];
};

ObjectLoader.prototype.__getMeshData = function(uuid, uuid_material) {
  "use strict";
  if (!(uuid in this.meshByUUID)) {
    for (var i = 0, len = this.fileObject["geometries"].length; i < len; i++) {
      var f_geom = this.fileObject["geometries"][i];
      if (f_geom["uuid"] === uuid) {
        var f_indices, // file data
            f_posns,
            f_norms,
            f_uvs,
            f_faces;

        if (!("metadata" in f_geom)) {
          f_geom["metadata"] = {
            "type": ""
          }; // Warning: modified input!?
        }

        if ("formatVersion" in f_geom["metadata"]) // then version 3.1
        {
          f_indices = f_geom["indices"];
          f_posns = f_geom["vertices"];
          f_norms = f_geom["normals"];
          f_uvs = f_geom["uvs"];
          f_faces = f_geom["faces"];
        } else if (f_geom["type"] === "Geometry") // V4 clara io output? not standard???
        {
          f_indices = f_geom["data"]["indices"];
          f_posns = f_geom["data"]["vertices"];
          f_norms = f_geom["data"]["normals"];
          f_uvs = f_geom["data"]["uvs"];
          f_faces = f_geom["data"]["faces"];
        } else if (f_geom["metadata"]["type"] === "Geometry") // V4
        {
          f_indices = f_geom["indices"];
          f_posns = f_geom["vertices"];
          f_norms = f_geom["normals"];
          f_uvs = f_geom["uvs"];
          f_faces = f_geom["faces"];
        } else if (f_geom["metadata"]["type"] === "BufferGeometry") // V4
        {
          f_posns = f_geom["data"]["attributes"]["position"]["array"];
          f_norms = f_geom["data"]["attributes"]["norms"]["array"];
          f_uvs = f_geom["data"]["attributes"]["uv"]["array"];
        }

        var nUvLayers = 0;

        // disregard empty arrays
        for (var i = 0; i < this.fileObject.uvs.length; i++) {
          if (this.fileObject.uvs[i].length)
            nUvLayers++;
        }

        var verts = new dali.VectorVertex();
        var vert = []; //new dali.Vertex();
        for (var i = 0, len = f_posns.length / 3; i < len; i++) {
          vert.push(f_posns[(i * 3) + 0]);
          vert.push(f_posns[(i * 3) + 1]);
          vert.push(f_posns[(i * 3) + 2]);

          vert.push(0); // norm
          vert.push(0);
          vert.push(0);

          vert.push(0); // uvs
          vert.push(0);

          verts.push_back(vert);
        }

        var mesh = new dali.MeshDataWrapper();
        var faces = new dali.VectorFaceIndex();
        var faceSets = {};
        //faceSets.length = this.fileObject.materials;
        for (var i = 0, len = this.fileObject.materials.length; i < len; ++i) {
          // get materials and force them to be loaded up
          var mat = this.__getMaterial(this.fileObject.materials[i]["uuid"]);
        }

        var idx = 0;
        var idx_len = f_faces.length;
        var materialUUID = undefined;
        while (idx < idx_len) {
          var f_type = f_faces[idx++];
          var isQuad = __isBitSet(f_type, 0);
          var hasMaterial = __isBitSet(f_type, 1);
          var hasFaceUv = __isBitSet(f_type, 2);
          var hasFaceVertexUv = __isBitSet(f_type, 3);
          var hasFaceNormal = __isBitSet(f_type, 4);
          var hasFaceVertexNormal = __isBitSet(f_type, 5);
          var hasFaceColor = __isBitSet(f_type, 6);
          var hasFaceVertexColor = __isBitSet(f_type, 7);

          var nVertices = 3;
          var faceVertexIndices;
          if (isQuad) {
            faces.push_back(f_faces[idx]);
            faces.push_back(f_faces[idx + 1]);
            faces.push_back(f_faces[idx + 2]);

            faces.push_back(f_faces[idx]);
            faces.push_back(f_faces[idx + 2]);
            faces.push_back(f_faces[idx + 3]);

            faceVertexIndices = [f_faces[idx],
                                 f_faces[idx + 1],
                                 f_faces[idx + 2]
                                ];

            idx += 4;
            nVertices = 4;
          } else {
            faces.push_back(f_faces[idx]);
            faces.push_back(f_faces[idx + 1]);
            faces.push_back(f_faces[idx + 2]);

            faceVertexIndices = [f_faces[idx],
                                 f_faces[idx + 1],
                                 f_faces[idx + 2]
                                ];

            idx += 3;
          }

          if (hasMaterial) {
            if (materialUUID === undefined) {
              materialUUID = this.fileObject.materials[f_faces[idx]]["uuid"];
            } else {
              // different material per face is bonkers - I'm not going to support it.
              if (this.fileObject.materials[f_faces[idx]]["uuid"] !== materialUUID) {
                throw "Faces with different materials is not supported";
              }
            }
            idx++;
          }


          if (hasFaceUv) {
            for (var i = 0; i < nUvLayers; i++) {
              var uvLayer = self.fileObject.uvs[i];
              var uvIndex = f_faces[idx++];
              var u = uvLayer[uvIndex * 2];
              var v = uvLayer[uvIndex * 2 + 1];
              // discarded - tbd ?
            }
          }

          if (hasFaceVertexUv) {
            for (var i = 0; i < nUvLayers; i++) {
              var uvLayer = f_geom.uvs[i];
              var uvs = [];
              for (var j = 0; j < nVertices; j++) {
                var uvIndex = f_faces[idx++];
                var u = uvLayer[uvIndex * 2];
                var v = uvLayer[uvIndex * 2 + 1];
                // discarded- tbd ?
              }
            }
          }

          if (hasFaceNormal) {
            var normalIndex = f_faces[idx++] * 3;

            var x = f_geom.normals[normalIndex++];
            var y = f_geom.normals[normalIndex++];
            var z = f_geom.normals[normalIndex];

            for (var i = 0; i < faceVertexIndices.length; i++) {
              var v = vert.get(faceVertexIndices[i]);

              v[4] += x;
              v[5] += y;
              v[6] += z;
            }
          }

          if (hasFaceVertexNormal) {
            for (var i = 0; i < nVertices; i++) {
              var normalIndex = faces[idx] * 3;
              var x = f_geom.normals[normalIndex++];
              var y = f_geom.normals[normalIndex++];
              var z = f_geom.normals[normalIndex];

              var v = vert.get(faces[idx]);

              v[4] += x;
              v[5] += y;
              v[6] += z;

              idx += 1;
              // face.vertexNormals.push( normal );
            }
          }

          if (hasFaceColor) {
            var color = f_faces[idx++];
          }

          if (hasFaceVertexColor) {
            for (var i = 0; i < nVertices; i++) {
              var colorIndex = faces[idx++];
              var color = f_geom.colors[colorIndex]; // ??? f_geom.colors?
              // face.vertexColors.push( color );
            }
          }

          var faces = null;
          if (f_faces) {
            for (var i = 1, len = f_faces.length; i < len; i++) {
              faces.push_back(f_faces[i]);
            }
          }

          if (f_indices) {
            faces = new dali.VectorFaceIndex();
            for (var i = 1, len = f_indices.length; i < len; i++) {
              faces.push_back(f_indices[i]);
            }
          }

          if (!faces) {
            faces = [];
            for (var i = 0, len = f_posns.length; i < len; i++) {
              faces.push(i);
            }
          }

          console.log(verts.size() + ":" + faces.size() + ":" + uuid_material);

          var material = this.__getMaterial(uuid_material);
          mesh.setLineData(verts, faces, material);
        }

        this.meshByUUID[uuid] = mesh;
        verts.delete();
        faces.delete();
        break;
      } // if uuid found
    } // for geom in geometries
  } // if uid ! in meshByUUID

  return this.meshByUUID[uuid];
};

ObjectLoader.prototype.delete = function() {
  "use strict";
  for (var a in this.meshByUUID) {
    a.delete();
  }
  this.meshByUUID = {};
  for (var b in this.matByUUID) {
    b.delete();
  }
  this.matByUUID = {};
};

ObjectLoader.prototype.createMeshActors = function() {
  "use strict";
  var ret = [];
  if ("object" in this.fileObject) {
    for (var i = 0, len = this.fileObject["object"]["children"].length; i < len; i++) {
      var child = this.fileObject["children"];
      if (child["type"] === "Mesh") {
        var meshData = this.__getMeshData(child["geometry"],
                                          child["material"]);
        ret.push(dali.__createMeshActor(meshData));
        meshData.delete();
      }
    }
  }

  var parent;

  if (ret) {
    parent = new dali.Actor();
    for (var a in ret) {
      parent.add(a);
      a.delete();
    }
  }

  return parent;
};

dali.createMeshActor = function(threeDjs_formatV4) {
  "use strict";
  var loader = new ObjectLoader(threeDjs_formatV4);
  return loader.createMeshActor();
};



//------------------------------------------------------------------------------
//
// Hit test
//
//------------------------------------------------------------------------------
dali.hitTest = function(x, y) {
  "use strict";
  var a = dali.__hitTest(x, y);
  if (a.ok()) {
    dali.internalSetupProperties(a);
    return a;
  } else {
    return null;
  }
};


//------------------------------------------------------------------------------
//
// Create an actor or something else. Actors have more specific functionality
//
//------------------------------------------------------------------------------
dali.create = function(name) {
  "use strict";

  var handle = dali.createActor(name);

  if (!handle.ok()) {
    handle = dali.createAny(name);
  }

  dali.internalSetupProperties(handle);

  //
  // WORKAROUND FOR SIZE NEGOTIATION SIZE ISSUE; turn it off.... @todo remove when fixed
  //
  handle.relayoutEnabled = false;


  return handle;
};


//------------------------------------------------------------------------------
//
// Shader support
//
//------------------------------------------------------------------------------

// dali shader uniforms prepends for geometry type enum switch
dali.prepends = {image: { vertex:
                          "//precision highp float;\n" +
                          "attribute vec3  aPosition;\n" +
                          "attribute vec2  aTexCoord;\n" +
                          "uniform   mat4  uModelView;\n" +
                          "uniform   mat4  uProjection;\n" +
                          "uniform   mat4  uMvpMatrix;\n" +
                          "uniform   mat3  uNormalMatrix;\n" +
                          "uniform   mat4  uModelMatrix;\n" +
                          "uniform   mat4  uViewMatrix;\n" +
                          "uniform   mediump vec4  uColor;\n" +
                          "varying   mediump vec2  vTexCoord;\n" +
                          "uniform   mediump vec4  sTextureRect;\n" +
                          "uniform   mediump vec4  sEffectRect;\n",
                          fragment:
                          "//precision mediump float;\n" +
                          "uniform   sampler2D sTexture;\n" +
                          "uniform   sampler2D sEffect;\n" +
                          "uniform mediump vec4 sTextureRect;\n" +
                          "uniform mediump vec4 sEffectRect;\n" +
                          "uniform   mediump vec4 uColor;\n" +
                          "varying   mediump vec2 vTexCoord;\n"
                        },
                 "untextured mesh": { vertex:
                                      "uniform   mediump mat4    uProjection;\n" +
                                      "uniform   mediump mat4    uModelView;\n" +
                                      "uniform   mediump mat4    uMvpMatrix;\n" +
                                      "uniform           bool    uTextureMapped;\n" +
                                      "uniform   mediump vec4    uCustomTextureCoords;\n" +
                                      "attribute mediump vec2    aTexCoord;\n" +
                                      "varying   mediump vec2    vTexCoord;\n" +
                                      "uniform   mat3    uModelViewIT;\n" +
                                      "attribute mediump vec3    aNormal;\n" +
                                      "varying   mediump vec3    vNormal;\n" +
                                      "attribute mediump vec3    aPosition;\n" +
                                      "varying   mediump vec4    vVertex;\n" +
                                      "#define USE_NORMALS\n" +
                                      "#define MAX_BONES_PER_MESH  12\n" +
                                      "#ifdef USE_BONES\n" +
                                      "uniform   int             uBoneCount;\n" +
                                      "uniform   mediump mat4    uBoneMatrices[MAX_BONES_PER_MESH];\n" +
                                      "uniform   mediump mat3    uBoneMatricesIT[MAX_BONES_PER_MESH];\n" +
                                      "attribute mediump vec4    aBoneWeights;\n" +
                                      "attribute mediump vec4    aBoneIndices;\n" +
                                      "#endif\n" +
                                      "#ifdef USE_COLOR\n" +
                                      "attribute lowp vec3       aColor;\n" +
                                      "varying   mediump vec3    vColor;\n" +
                                      "#endif\n",
                                      fragment:
                                      "struct Material\n" +
                                      "{\n" +
                                      "  mediump float mOpacity;\n" +
                                      "  mediump float mShininess;\n" +
                                      "  lowp    vec4  mAmbient;\n" +
                                      "  lowp    vec4  mDiffuse;\n" +
                                      "  lowp    vec4  mSpecular;\n" +
                                      "  lowp    vec4  mEmissive;\n" +
                                      "};\n" +
                                      "uniform sampler2D     sTexture;\n" +
                                      "uniform sampler2D     sOpacityTexture;\n" +
                                      "uniform sampler2D     sNormalMapTexture;\n" +
                                      "uniform sampler2D     sEffect;\n" +
                                      "varying mediump vec2  vTexCoord;\n" +
                                      "uniform Material      uMaterial;\n" +
                                      "uniform lowp  vec4    uColor;\n" +
                                      "varying highp vec4    vVertex;\n" +
                                      "varying highp vec3    vNormal;\n" +
                                      "#ifdef USE_LIGHTING\n" +
                                      "struct Light\n" +
                                      "{\n" +
                                      "  int           mType;\n" +
                                      "  highp   vec2  mFallOff;\n" +
                                      "  mediump vec2  mSpotAngle;\n" +
                                      "  mediump vec3  mLightPos;\n" +
                                      "  mediump vec3  mLightDir;\n" +
                                      "  lowp    vec3  mAmbient;\n" +
                                      "  lowp    vec3  mDiffuse;\n" +
                                      "  lowp    vec3  mSpecular;\n" +
                                      "};\n" +
                                      "#endif\n" +
                                      "#ifdef USE_LIGHTING\n" +
                                      "uniform         int   uNumberOfLights;\n" +
                                      "uniform Light         uLight0;\n" +
                                      "uniform Light         uLight1;\n" +
                                      "uniform Light         uLight2;\n" +
                                      "#endif\n" +
                                      "#ifdef USE_COLOR\n" +
                                      "varying mediump vec3  vColor;\n" +
                                      "#endif\n" +
                                      "#ifdef USE_LIGHTING\n" +
                                      "lowp vec3 lightColor;\n" +
                                      "lowp vec3 specularColor;\n" +
                                      "void calculateLight(Light light)\n" +
                                      "{\n" +
                                      "  highp vec3 lightVector = light.mLightPos - vVertex.xyz;\n" +
                                      "  mediump vec3 N = normalize(vNormal);\n" +
                                      "  mediump vec3 L = normalize(lightVector);\n" +
                                      "  mediump float NdotL = dot(N, L);\n" +
                                      "  mediump vec3 color = light.mAmbient * uMaterial.mAmbient.rgb;\n" +
                                      "  color += light.mDiffuse * uMaterial.mDiffuse.rgb * abs(NdotL);\n" +
                                      "  highp float attenuation = 1.0;\n" +
                                      "  if (light.mType >= 2)\n" +
                                      "  {\n" +
                                      "    attenuation -= smoothstep(light.mFallOff.x, light.mFallOff.y, length(lightVector));\n" +
                                      "  }\n" +
                                      "  lightColor += color * attenuation;\n" +
                                      "  if (light.mType > 1 && NdotL > 0.0 && uMaterial.mShininess > 0.0)\n" +
                                      "  {\n" +
                                      "    highp vec3 E = normalize(vVertex.xyz);\n" +
                                      "    highp vec3 R = reflect(L, N);\n" +
                                      "    highp float specular = pow(max(dot(R, E), 0.0), uMaterial.mShininess);\n" +
                                      "    specularColor += uMaterial.mSpecular.rgb * light.mSpecular * specular * attenuation;\n" +
                                      "  }\n" +
                                      "}\n" +
                                      "#endif\n"
                                    },
                 "textured mesh": { vertex:
                                    "uniform   mediump mat4    uProjection;\n" +
                                    "uniform   mediump mat4    uModelView;\n" +
                                    "uniform   mediump mat4    uMvpMatrix;\n" +
                                    "uniform           bool    uTextureMapped;\n" +
                                    "uniform   mediump vec4    uCustomTextureCoords;\n" +
                                    "attribute highp   vec2    aTexCoord;\n" +
                                    "varying   mediump vec2    vTexCoord;\n" +
                                    "uniform   mat3            uModelViewIT;\n" +
                                    "attribute mediump vec3    aNormal;\n" +
                                    "varying   mediump vec3    vNormal;\n" +
                                    "attribute mediump vec3    aPosition;\n" +
                                    "varying   mediump vec4    vVertex;\n" +
                                    "#define MAX_BONES_PER_MESH  12\n" +
                                    "#ifdef USE_BONES\n" +
                                    "uniform   int             uBoneCount;\n" +
                                    "uniform   mediump mat4    uBoneMatrices[MAX_BONES_PER_MESH];\n" +
                                    "uniform   mediump mat3    uBoneMatricesIT[MAX_BONES_PER_MESH];\n" +
                                    "attribute mediump vec4    aBoneWeights;\n" +
                                    "attribute mediump vec4    aBoneIndices;\n" +
                                    "#endif\n",
                                    fragment:
                                    "struct Material\n" +
                                    "{\n" +
                                    "  mediump float mOpacity;\n" +
                                    "  mediump float mShininess;\n" +
                                    "  lowp    vec4  mAmbient;\n" +
                                    "  lowp    vec4  mDiffuse;\n" +
                                    "  lowp    vec4  mSpecular;\n" +
                                    "  lowp    vec4  mEmissive;\n" +
                                    "};\n" +
                                    "uniform sampler2D     sTexture;\n" +
                                    "uniform sampler2D     sOpacityTexture;\n" +
                                    "uniform sampler2D     sNormalMapTexture;\n" +
                                    "uniform sampler2D     sEffect;\n" +
                                    "varying mediump vec2 vTexCoord;\n" +
                                    "uniform Material      uMaterial;\n" +
                                    "uniform lowp  vec4    uColor;\n" +
                                    "varying highp vec4    vVertex;\n" +
                                    "varying highp vec3    vNormal;\n" +
                                    "#ifdef USE_LIGHTING\n" +
                                    "struct Light\n" +
                                    "{\n" +
                                    "  int           mType;\n" +
                                    "  highp   vec2  mFallOff;\n" +
                                    "  mediump vec2  mSpotAngle;\n" +
                                    "  mediump vec3  mLightPos;\n" +
                                    "  mediump vec3  mLightDir;\n" +
                                    "  lowp    vec3  mAmbient;\n" +
                                    "  lowp    vec3  mDiffuse;\n" +
                                    "  lowp    vec3  mSpecular;\n" +
                                    "};\n" +
                                    "uniform         int   uNumberOfLights;\n" +
                                    "uniform Light         uLight0;\n" +
                                    "lowp vec3 lightColor;\n" +
                                    "lowp vec3 specularColor;\n" +
                                    "void calculateLight(Light light)\n" +
                                    "{\n" +
                                    "  highp vec3 lightVector = light.mLightPos - vVertex.xyz;\n" +
                                    "  mediump vec3 N = normalize(vNormal);\n" +
                                    "  mediump vec3 L = normalize(lightVector);\n" +
                                    "  mediump float NdotL = dot(N, L);\n" +
                                    "  mediump vec3 color = light.mAmbient * uMaterial.mAmbient.rgb;\n" +
                                    "  color += light.mDiffuse * uMaterial.mDiffuse.rgb * abs(NdotL);\n" +
                                    "  highp float attenuation = 1.0;\n" +
                                    "  if (light.mType >= 2)\n" +
                                    "  {\n" +
                                    "    attenuation -= smoothstep(light.mFallOff.x, light.mFallOff.y, length(lightVector));\n" +
                                    "  }\n" +
                                    "  lightColor += color * attenuation;\n" +
                                    "  if (light.mType != 0 && NdotL > 0.0 && light.mType != 0)\n" +
                                    "  {\n" +
                                    "    highp vec3 E = normalize(vVertex.xyz);\n" +
                                    "    highp vec3 R = reflect(L, N);\n" +
                                    "    highp float specular = pow(max(dot(R, E), 0.0), uMaterial.mShininess);\n" +
                                    "    specularColor += uMaterial.mSpecular.rgb * light.mSpecular * specular * attenuation;\n" +
                                    "  }\n" +
                                    "}\n" +
                                    "#endif\n"
                                  }
                }; // prepends


// supported uniforms
dali.supportedUniformTypes = ["float",
                              "vec2", "vec3", "vec4",
                              "mat3", "mat4"
                             ];

// need to add a value to uniform registration call
dali.supportedUniformValues = [0.0,
                               [0.0, 0.0],
                               [0.0, 0.0, 0.0],
                               [0.0, 0.0, 0.0, 0.0],
                               [1.0, 0.0, 0.0,
                                0.0, 1.0, 0.0,
                                0.0, 0.0, 1.0
                               ],
                               [1.0, 0.0, 0.0, 0.0,
                                0.0, 1.0, 0.0, 0.0,
                                0.0, 0.0, 1.0, 0.0,
                                0.0, 0.0, 0.0, 1.0
                               ]
                              ];

// get shader information from gl compiled shaders (needs a valid webgl context)
dali.getProgramInfo = function(gl, vertex, fragment) {
  "use strict";
  var i;
  var info = {
    vertex: vertex,
    fragment: fragment,
    attributes: [],
    uniforms: [],
    attributeCount: 0,
    uniformCount: 0,
    hasError: false,    // compiles without error
    vertexError: "",
    fragmentError: "",
    linkError: ""
  };

  var vertexShader = gl.createShader(gl.VERTEX_SHADER);
  var vertexPrepend = "";
  gl.shaderSource(vertexShader, vertexPrepend + vertex);
  gl.compileShader(vertexShader);

  // Check the compile status, return an error if failed
  if (!gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS)) {
    info.hasError = true;
    info.vertexError = gl.getShaderInfoLog(vertexShader);
  }

  var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
  var fragmentPrepend = "";
  gl.shaderSource(fragmentShader, fragmentPrepend + fragment);
  gl.compileShader(fragmentShader);

  // Check the compile status, return an error if failed
  if (!gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS)) {
    info.hasError = true;
    info.fragmentError = gl.getShaderInfoLog(fragmentShader);
  }

  if(info.hasError) {
    gl.deleteShader(vertexShader);
    gl.deleteShader(fragmentShader);
    return info; // ==> out
  } else {
    var program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);

    gl.linkProgram(program);

    if(!gl.getProgramParameter(program, gl.LINK_STATUS)) {
      info.hasError = true;
      info.linkError = gl.getProgramInfoLog(program);
      gl.deleteProgram(program);
      gl.deleteShader(vertexShader);
      gl.deleteShader(fragmentShader);
      return info; // ==> out
    }
  }

  var activeUniforms = gl.getProgramParameter(program, gl.ACTIVE_UNIFORMS);
  var activeAttributes = gl.getProgramParameter(program, gl.ACTIVE_ATTRIBUTES);

  // Taken from the WebGl spec:
  // http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.14
  var enums = {
    0x8B50: "FLOAT_VEC2",
    0x8B51: "FLOAT_VEC3",
    0x8B52: "FLOAT_VEC4",
    0x8B53: "INT_VEC2",
    0x8B54: "INT_VEC3",
    0x8B55: "INT_VEC4",
    0x8B56: "BOOL",
    0x8B57: "BOOL_VEC2",
    0x8B58: "BOOL_VEC3",
    0x8B59: "BOOL_VEC4",
    0x8B5A: "FLOAT_MAT2",
    0x8B5B: "FLOAT_MAT3",
    0x8B5C: "FLOAT_MAT4",
    0x8B5E: "SAMPLER_2D",
    0x8B60: "SAMPLER_CUBE",
    0x1400: "BYTE",
    0x1401: "UNSIGNED_BYTE",
    0x1402: "SHORT",
    0x1403: "UNSIGNED_SHORT",
    0x1404: "INT",
    0x1405: "UNSIGNED_INT",
    0x1406: "FLOAT"
  };

  // Loop through active uniforms
  for (i = 0; i < activeUniforms; i++) {
    var uniform = gl.getActiveUniform(program, i);
    uniform.typeName = enums[uniform.type];
    info.uniforms.push(uniform);
    info.uniformCount += uniform.size;
  }

  // Loop through active attributes
  for (i = 0; i < activeAttributes; i++) {
    var attribute = gl.getActiveAttrib(program, i);
    attribute.typeName = enums[attribute.type];
    info.attributes.push(attribute);
    info.attributeCount += attribute.size;
  }

  return info;
};

dali.removeComments = function(str) {
  "use strict";
  var uid = '_' + +new Date(),
      primatives = [],
      primIndex = 0;

  return (
    str
    /* Remove strings */
      .replace(/(['"])(\\\1|.)+?\1/g, function(match){
        primatives[primIndex] = match;
        return (uid + "") + primIndex++;
      })

    /* Remove Regexes */
      .replace(/([^\/])(\/(?!\*|\/)(\\\/|.)+?\/[gim]{0,3})/g, function(match, $1, $2){
        primatives[primIndex] = $2;
        return $1 + (uid + "") + primIndex++;
      })

    /*
     - Remove single-line comments that contain would-be multi-line delimiters
     E.g. // Comment /* <--
     - Remove multi-line comments that contain would be single-line delimiters
     E.g. /* // <--
     */
      .replace(/\/\/.*?\/?\*.+?(?=\n|\r|$)|\/\*[\s\S]*?\/\/[\s\S]*?\*\//g, "")

    /*
     Remove single and multi-line comments,
     no consideration of inner-contents
     */
      .replace(/\/\/.+?(?=\n|\r|$)|\/\*[\s\S]+?\*\//g, "")

    /*
     Remove multi-line comments that have a replace ending (string/regex)
     Greedy, so no inner strings/regexes will stop it.
     */
      .replace(RegExp('\\/\\*[\\s\\S]+' + uid + '\\d+', 'g'), "")

    /* Bring back strings & regexes */
      .replace(RegExp(uid + '(\\d+)', 'g'), function(match, n){
        return primatives[n];
      })
  );
};

dali.contains = function(array, value) {
  "use strict";
  for(var i = 0; i < array.length; i++) {
    if(array[i] === value) {
      return true;
    }
  }
  // else
  return false;
};

dali.setUniformMetaUiSpec = function(meta) {
  "use strict";
  var type = meta.type;

  function setIfMissing(o, n, v) {
    if(!(n in o)) {
      o[n] = v;
    }
  }

  if(type.startsWith("vec")) { // 2/3/4
    setIfMissing(meta, "control", "slider");
    if(type.substr(-1) === "2") {
      setIfMissing(meta, "min", [0.0, 0.0]);
      setIfMissing(meta, "max", [1.0, 1.0]);
      setIfMissing(meta, "default", [0.0, 0.0]);
    } else if(type.substr(-1) === "3") {
      setIfMissing(meta, "min", [0.0, 0.0, 0.0]);
      setIfMissing(meta, "max", [1.0, 1.0, 1.0]);
      setIfMissing(meta, "default", [0.0, 0.0, 0.0]);
    } else if(type.substr(-1) === "4") {
      setIfMissing(meta, "min", [0.0, 0.0, 0.0, 0.0]);
      setIfMissing(meta, "max", [1.0, 1.0, 1.0, 1.0]);
      setIfMissing(meta, "default", [0.0, 0.0, 0.0, 0.0]);
    }
  } else if(type.startsWith("mat")) { // 3/4
    if(type.substr(-1) === "3") {
      setIfMissing(meta, "control", "mat3");
      setIfMissing(meta, "default", [ 0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0 ]);
    } else if(type.substr(-1) === "4") {
      setIfMissing(meta, "control", "mat4");
      setIfMissing(meta, "default", [ 0.0, 0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0, 0.0 ]);
    }
  } else {
    setIfMissing(meta, "control", "slider");
    setIfMissing(meta, "default", 0.0);
    setIfMissing(meta, "min", 0.0);
    setIfMissing(meta, "max", 1.0);
  }
};

dali.sourceUniformMetaData = function(src) {
  "use strict";
  //  Returns uniform data
  //
  // [ {name:"",
  //    type:"",
  //    index:0,          // index if array
  //    count:10,         // size if array
  //    control:"slider", // ui control [slider|color]
  //    default: ,        // default value of type
  //    min:0,            // optional control parameters
  //    max:1
  //   },
  //   ...                // array of uniforms
  // ]
  //

  // Undoubtedly this approach will be wrong. Hopefully on not too many corner cases...
  // A better way is to compile the source see (getProgramInfo())
  // but that requres a gl context which this module doesnt know.
  var tmp;

  var definesOut = /#define[ \t]+([A-Za-z_0-9]*)[ \t]+(.*)/g;

  var reg = /[ \t]?uniform[ ]*((?:lowp|mediump|highp)?[ \t]*(bool|int|uint|float|[biu]?vec[234]|mat[234]x?[234]?|[^ ]*)[ \t]*([A-Za-z0-9]*))[ \t]*(;|\[.*\][ \t]*;)(.*)/gi;

  // 1. no commented out uniforms
  var noCommentSource = dali.removeComments(src);

  var validUniforms = [];
  while ((tmp = reg.exec(noCommentSource))) {
    validUniforms.push( tmp[3] );
  }

  // 2. replace defines
  var defines = [];
  while ((tmp = definesOut.exec(noCommentSource))) {
    defines.push([tmp[1], tmp[2]]);
  }
  var defineDict = {};
  var defineList = [];
  while(defines.length) {
    var p = defines.pop();
    var n = p[0];
    var v = p[1];
    try {
      defineDict[n] = eval(v);
      defineList.push([n, defineDict[n]]);
    } catch(e) {
      var d = /([A-Za-z]+[A-Za-z0-9]*)/g;
      while ((tmp = d.exec(v))) {
        if(tmp[0] in defineDict) {
          v = v.replace(tmp[0], defineDict[tmp[0]]);
        } else {
          defines.push(p); // stick it back to try again. ...and endless loop if we can't(!)
        }
      }
    }
  }

  for(var i = 0; i < defineList.length; i++) {
    var re = new RegExp(defineList[i][0], "g");
    src = src.replace(re, defineList[i][1]);
  }

  // 3. get uniforms
  var returnvalue = [];
  var addingMeta;
  while ((tmp = reg.exec(src))) {
    if(!dali.contains(validUniforms, tmp[3])) {
      continue;
    }
    var uType = tmp[2];
    var uName = tmp[3];
    var uArray = tmp[4].slice(0, -1);
    var uComments = tmp[5].trim();
    var meta;
    if(uComments.startsWith("//")) { // meta data in comments
      try {
        meta = eval("(" + uComments.substr(2) + ")"); // brackets to be expression not opening statement
        if(typeof meta !== typeof ({})) {
          throw ("ohno");
        }
      } catch (e) {
        meta = {};
      }
    } else {
      meta = {};
    }

    meta.type = tmp[2];
    meta.name = tmp[3];
    meta.count = 0;

    if(uArray.search("[[]") >= 0) { // an array
      meta.count = Number(uArray.slice(1, -1));
    }

    if(dali.contains( dali.supportedUniformTypes, uType) ) {
      if(meta.count !== 0) { // array
        for(var j = 0; j < meta.count; j++) {
          addingMeta = {type: meta.type,
                        name: meta.name + "[" + j + "]",
                        index: j,
                        count: meta.count};
          dali.setUniformMetaUiSpec(addingMeta);
          returnvalue.push( addingMeta );
        }
      } else {
        dali.setUniformMetaUiSpec(meta);
        returnvalue.push( meta );
      }
    } else {
      // not a base type so need to get the compound type
      var structFind = new RegExp( "(struct[ \t\n]*" + uType + "[^{]*{)([^}]*)", "g");
      var structLines = structFind.exec(src)[2].split(";");
      var structUniforms = [];
      var tmpStruct;
      var k;
      for(var lineNo = 0; lineNo < structLines.length; lineNo++) {
        var line = structLines[lineNo].replace(/\n/g, "") + ";";
        if(line !== ";") {
          var structReg = /[ \t\n]*((?:lowp|mediump|highp)?[ \t\n]*(bool|int|uint|float|[biu]?vec[234]|mat[234]x?[234]?|[^ ]*)[ \t\n]*([A-Za-z0-9]*))[ \t\n]*(;|\[.*\][ \t\n]*;)/gi;
          while ((tmpStruct = structReg.exec(line))) {
            addingMeta = { type: tmpStruct[2],
                           name: tmpStruct[3],
                           count: meta.count };
            dali.setUniformMetaUiSpec( addingMeta );
            structUniforms.push( addingMeta );
          }
        }
      }
      if(meta.count === 0) {
        for(k = 0; k < structUniforms.length; k++) {
          addingMeta = {type: structUniforms[k].type,
                        name: uName + "." + structUniforms[k].name,
                        count: meta.count,
                        index: 0,
                        structType: meta.type,
                        structName: meta.name};
          dali.setUniformMetaUiSpec(addingMeta);
          returnvalue.push(addingMeta);
        }
      } else { // array
        for(var l = 0; l < meta.count; l++) {
          for(k = 0; k < structUniforms.length; k++) {
            addingMeta = {type: structUniforms[k].type,
                          name: uName + "[" + l + "]" + "." + structUniforms[k].name,
                          count: meta.count,
                          index: l,
                          structType: meta.type,
                          structName: meta.name};
            dali.setUniformMetaUiSpec(addingMeta);
            returnvalue.push(addingMeta);
          }
        }
      }
    }
  }
  return returnvalue;
};

dali.uniformMetaData = function(vertexShader, fragmentShader) {
  "use strict";
  var ret = dali.sourceUniformMetaData(vertexShader);

  // todo check for duplicates that arent the same type
  if(fragmentShader) {
    ret = ret.concat( dali.sourceUniformMetaData(fragmentShader) );
  }

  return ret;
};

dali.defaultUniforms = [ // vertex
  "uMvpMatrix",
  "uModelMatrix",
  "uViewMatrix",
  "uModelView",
  "uNormalMatrix",
  "uProjection",
  "uColor",
  // fragment
  "sTexture",
  "sEffect",
  "uColor",
  "uTextColor"
];

dali.createShaderEffect = function(options) {
  "use strict";

  var vertex = "";
  var fragment = "";

  if ("vertex" in options) {
    vertex = options.vertex;
  }
  if ("fragment" in options) {
    fragment = options.fragment;
  }

  //
  // using value instead of the type/hint directly as theres no way for embind
  // to represent additive enums (for hints)
  //
  var geometryHints = 0;

  if ("geometryHints" in options) {
    var hints = options.geometryHints.toLowerCase().split(" ");
    for(var i = 0; i < hints.length; i++) {
      var hint = hints[i];
      if (hint === "gridx") {
        geometryHints += dali.GeometryHints.HINT_GRID_X.value;
      } else if (hint === "gridy") {
        geometryHints += dali.GeometryHints.HINT_GRID_Y.value;
      } else if (hint === "grid") {
        geometryHints += dali.GeometryHints.HINT_GRID.value;
      } else if (hint === "depthbuffer") {
        geometryHints += dali.GeometryHints.HINT_DEPTH_BUFFER.value;
      } else if (hint === "blending") {
        geometryHints += dali.GeometryHints.HINT_BLENDING.value;
      } else if (hint === "doesntmodifygeometry") {
        geometryHints += dali.GeometryHints.HINT_DOESNT_MODIFY_GEOMETRY.value;
      }
    }
  }

  var handle = new dali.ShaderEffectWrapper(vertex, fragment,  geometryHints);

  // keep the sources around so we can register the uniforms to the actor automatically
  // (we could just keep the uniforms if thats all we need)
  dali.internalSetupProperties(handle);

  handle.setUniform = function(name, value) {
    var setProperty = new dali.DaliPropertyValue(null, name, value);
    handle.__setUniform(name, setProperty);
    if(setProperty) {
      setProperty.delete();
    }
  };

  handle.__dali_animationData = {}; // debug

  handle.registerUniform = function(name, value) {
    "use strict";
    var ret = -1;
    handle.setUniform( name, value ); // use SetUniform which also registers (but doesnt name convert camelCase to hyphenated)
    Object.defineProperty(handle, name, {
      enumerable: true,
      configurable: false,
      get: function() {
        return dali.marshallGetProperty(this, name);
      },
      set: function(newValue) {
        dali.marshallSetProperty(this, name, newValue);
      }
    });
  };

  var uniforms = [];
  uniforms = uniforms.concat(dali.sourceUniformMetaData(vertex));
  uniforms = uniforms.concat(dali.sourceUniformMetaData(fragment));

  var uniformNames = [];
  for (var i = 0; i < uniforms.length; i++) {
    var uniformType = uniforms[i].type;
    var uniformName = uniforms[i].name;
    //var count = uniforms[i][2];
    var typeIndex = dali.supportedUniformTypes.indexOf(uniformType);
    if (dali.defaultUniforms.indexOf(uniformName) < 0) {
      if (typeIndex >= 0 && uniformNames.indexOf(uniformName) < 0) {
        console.log("Uniform registered:", uniformName);
        uniformNames.push(uniformName);
        var uniformValue = dali.supportedUniformValues[typeIndex];

        if( "default" in uniforms[i]) {
          uniformValue = uniforms[i].default;
        }

        // handle.registerAnimatedProperty(name, value);
        handle.registerUniform(uniformName, uniformValue);
        // debug
        handle.__dali_animationData[uniformName] = {
          value: uniformValue,
          function: "linear",
          delay: 0,
          duration: 3
        };
      }
    }
  }

  // debug
  handle.__dali_runAnimation = function(duration, looping) {
    if (handle.__dali_animation) {
      handle.__dali_animation.stop();
      handle.__dali_animation.delete();
    }
    var a = new dali.Animation();
    a.setDuration(duration);
    a.setLooping(looping);
    for (name in handle.__dali_animationData) {
      a.animateTo(handle, name,
                  handle.__dali_animationData[name].value,
                  handle.__dali_animationData[name].function,
                  handle.__dali_animationData[name].delay,
                  handle.__dali_animationData[name].duration);
    }
    a.play();
    handle.__dali_animation = a;
  };

  return handle;
};


//------------------------------------------------------------------------------
//
// debug
//
//------------------------------------------------------------------------------

dali.debugPrintProperties = function(typeName) {
  "use strict";

  var t = dali.getTypeRegistry();
  var info = t.getTypeInfo(typeName);
  var props = info.getProperties();
  for (var i = 0; i < props.size(); i++) {
    console.log(i + ":" + props.get(i));
  }
  info.delete(); // wrapper
  t.delete(); // wrapper
};

dali.debugPrintTypes = function() {
  "use strict";

  var t = dali.getTypeRegistry();
  for (var i = 0; i < t.getTypeNameCount(); i++) {
    console.log(t.getTypeName(i));
  }
  t.delete(); // wrapper
};


dali.__debugPrintParents = function(actor, list) {
  "use strict";
  var p = null;

  if (!actor.ok()) {
    return;
  }

  try {
    p = actor.getParent();
    if (!p.ok()){
      p = null;
    }
  } catch (e) {
    // console.log("Cannot get parent", e);
  }

  if (p) {
    list.push(p);
    dali.__debugPrintParents(p, list);
  }
};

dali.debugPrintTree = function(actor) {
  "use strict";
  var l = [];
  dali.__debugPrintParents(actor, l);
  var ti;
  console.log("---");
  for (var i = l.length - 1; i >= 0; i--) {
    var a = l[i];
    ti = a.getTypeInfo();
    console.log("|", Array(l.length - i).join("-"), ti.getName(), "P", a.position, "R", a.orientation, a.name);
    ti.delete();
  }
  ti = actor.getTypeInfo();
  console.log("*", Array(l.length + 1).join("*"), ti.getName(), "P", actor.position, "R", actor.orientation, actor.name);
  ti.delete();

  var children = actor.getChildren();
  for (var j = 0; j < children.length; j++) {
    var a = children[j];
    ti = a.getTypeInfo();
    console.log("|", Array(l.length + 1 + 1 + j).join("-"), ti.getName(), "P", a.position, "R", a.orientation, a.name);
    ti.delete();
  }
};

dali.debugPrintRenderTask = function(rendertask) {
  "use strict";
  console.log("[X,Y]", rendertask.getCurrentViewportPosition());
  console.log("[W,H]", rendertask.getCurrentViewportSize());

  var c = rendertask.getCameraActor();
  if (!c.ok()) {
    console.log("No Camera");
  } else {
    console.log("Camera Pos:", c.position);
    console.log("Camera Rot:", c.orientation);
    console.log("Camera Inherit:", c.inheritRotation);
    console.log("Camera ParentOrigin:", c.parentOrigin);
    console.log("Camera AnchorPoint:", c.anchorPoint);
    var p = null;
    try {
      p = c.getParent();
      if(!p.ok()) {
        p = null;
      }
    } catch (e) {
      console.log("Cannot get parent", e);
    }

    if (!p) {
      console.log("Camera has no parent?");
    } else {
      var ti = p.getTypeInfo();
      console.log("Parent Name", ti.getName());
      ti.delete();
      p.delete();
    }
  }
};

dali.debugPrintRenderTasks = function() {
  "use strict";
  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();
  for (var i = 0; i < taskList.getTaskCount(); i++) {
    var t = taskList.getTask(i);
    console.log("RenderTask:", i);
    dali.debugPrintRenderTask(t);
    t.delete(); // wrapper
  }
  taskList.delete(); // wrapper
};

dali.debugFindFirstActor = function(actor, predicateFunction) {
  "use strict";
  for (var i = 0, len = actor.getChildCount(); i < len; i++) {
    var a = actor.getChildAt(i);
    var found = predicateFunction(a);
    if (found) {
      return a;
    }
    var child = dali.debugFindFirstActor(a, predicateFunction);
    if (child) {
      return child;
    }
    a.delete();
  }
  return null;
};

dali.debugDepthVisit = function(actor, operation, dontDelete) {
  "use strict";
  for (var i = 0, len = actor.getChildCount(); i < len; i++) {
    var a = actor.getChildAt(i);
    var done = operation(a);
    if (!done) {
      return false;
    }
    if (!dali.debugDepthVisit(a, operation, dontDelete)) {
      return false;
    }
    var doit = true;
    if (dontDelete !== undefined) {
      if (dontDelete) {
        doit = false;
      }
    }
    if (doit) {
      a.delete();
    }
  }
  return true;
};

dali.operationPrintProperty = function(property, all) {
  "use strict";
  return (function(actor) {
    if (property in actor) {
      dali.log(actor.getId() + "property:" + actor[property]);
    } else {
      dali.log(actor.getId() + "property:n/a");
    }
    return all;
  });
};

dali.predicatePropertyEquals = function(property, value) {
  "use strict";
  return (function(actor) {
    if (property in actor) {
      if (actor[property] === value) {
        return true;
      }
    }
    return false;
  });
};



dali.init = function() {
  "use strict";

  dali.stage = dali.__getStage();

  dali.getStage = function() { // duplication of dali.stage to stop regressions
    return dali.stage;
  };

  //
  // Add constructor functions to dali from the type registry
  //
  var t = dali.getTypeRegistry();

  for (var i = 0; i < t.getTypeNameCount(); i++) {
    // anon function because of closure with defineProperty
    // (if just variable in loop then the variable 'address' is captured, not the value
    //  so it becomes last value set)
    (function(name) {
      // @todo Dali error?? name lengths should never be zero
      if (name.length && name != "Animation" && name != "ShaderEffect") {
        Object.defineProperty(dali, name, {
          enumerable: true,
          configurable: false,
          get: function() {
            return function() {
              // console.log(name);
              return dali.create(name);
            };
          }
        });
      }
    })(t.getTypeName(i));
  }

  //
  // Special wrappings overriding the dali.Create() which just returns a HandleWrapper
  //
  dali.Animation = dali.AnimationWrapper;

  dali.ShaderEffect = dali.createShaderEffect;

  dali.__updateOnce();
  dali.__renderOnce();

}(); // call init


//------------------------------------------------------------------------------
//
// Helper functions for creating front/top,left views
//
//------------------------------------------------------------------------------
dali.setClearColor = function(renderTaskIndex, color) {
  "use strict";
  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();
  if (renderTaskIndex >= taskList.getTaskCount()) {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);
  rendertask.setClearEnabled(true);
  rendertask.setClearColor(color);
};

dali.getClearColor = function(renderTaskIndex) {
  "use strict";
  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();
  if (renderTaskIndex >= taskList.getTaskCount()) {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return null;
  }
  var rendertask = taskList.getTask(renderTaskIndex);
  return rendertask.getClearColor();
};

dali.setFrontView = function(renderTaskIndex, x, y, w, h) {
  "use strict";
  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();
  if (renderTaskIndex >= taskList.getTaskCount()) {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);

  var c = rendertask.getCameraActor();
  assert(c.ok(), "Rendertask has no valid camera actor");

  rendertask.setViewportPosition([x, y]);
  rendertask.setViewportSize([w, h]);
  c.position = [0, 0, 800];
  c.orientation = [0, 1, 0, 180];
  c.aspectRatio = w / h;

  c.delete(); // wrapper
  rendertask.delete(); // wrapper
  taskList.delete(); // wrapper
};

dali.setTopView = function(renderTaskIndex, x, y, w, h) {
  "use strict";
  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();
  if (renderTaskIndex >= taskList.getTaskCount()) {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);

  var c = rendertask.getCameraActor();
  assert(c.ok(), "Rendertask has no valid camera actor");

  rendertask.setViewportPosition([x, y]);
  rendertask.setViewportSize([w, h]);

  var q1 = dali.axisAngleToQuaternion([0, 1, 0, dali.radian(180)]); // yaw around to look at scene down -ve z
  var q2 = dali.axisAngleToQuaternion([1, 0, 0, dali.radian(-90)]); // pitch to look at scene
  var q = dali.quaternionToAxisAngle(dali.quatByQuat(q1, q2));

  c.position = [0, -800, 0]; // @todo; get 800 from dali not hard coded here
  c.orientation = [q[0], q[1], q[2], dali.degree(q[3])]; // @todo; should really all be in radians
  c.aspectRatio = w / h;

  c.delete(); // wrapper
  rendertask.delete(); // wrapper
  taskList.delete(); // wrapper
};

dali.setRightView = function(renderTaskIndex, x, y, w, h) {
  "use strict";
  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();
  if (renderTaskIndex >= taskList.getTaskCount()) {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);

  var c = rendertask.getCameraActor();
  assert(c.ok(), "Rendertask has no valid camera actor");

  rendertask.setViewportPosition([x, y]);
  rendertask.setViewportSize([w, h]);

  var q1 = dali.axisAngleToQuaternion([0, 1, 0, dali.radian(180)]); // yaw around to look at scene down -ve z
  var q2 = dali.axisAngleToQuaternion([0, 1, 0, dali.radian(90)]); // yaw again to look from right
  var q = dali.quaternionToAxisAngle(dali.quatByQuat(q1, q2));

  c.position = [800, 0, 0];
  c.orientation = [q[0], q[1], q[2], dali.degree(q[3])]; // @todo; should really all be in radians
  c.aspectRatio = w / h;

  c.delete(); // wrapper
  rendertask.delete(); // wrapper
  taskList.delete(); // wrapper
};

dali.onePane = function() {
  "use strict";
  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();
  var tasks = [];
  var i, len;

  for (i = 1, len = taskList.getTaskCount(); i < len; i++) {
    tasks.push(taskList.getTask(i));
  }

  for (i = 0, len = tasks.length; i < len; i++) {
    var task = tasks[i];
    // delete the camera actors we created in twoPane and threePane
    var c = task.getCameraActor();
    if (c.ok()) {
      var p = c.getParent();
      if (p.ok()) {
        p.remove(c);
      }
      p.delete(); // wrapper
    }
    c.delete(); // wrapper

    taskList.removeTask(task);
    task.delete(); // wrapper
  }

  taskList.delete();
};

dali.twoPane = function() {
  "use strict";
  dali.onePane();

  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();

  var defaultTask = taskList.getTask(0);
  var defaultCamera = defaultTask.getCameraActor();
  var defaultCameraParent = defaultCamera.getParent();

  var t;
  t = taskList.createTask();

  var c = new dali.CameraActor(); // add camera for different viewpoint
  c.position = [0, 0, 800];
  c.orientation = [0, 1, 0, 180];
  c.parentOrigin = [0.5, 0.5, 0.5];
  c.anchorPoint = [0.5, 0.5, 0.5];
  t.setCameraActor(c);
  defaultCameraParent.add(c);
  c.delete(); // wrapper

  t.delete(); // wrapper

  defaultCameraParent.delete(); // wrapper
  defaultCamera.delete(); // wrapper
  defaultTask.delete(); // wrapper

  taskList.delete(); // wrapper
};

//
// Creates render tasks and cameras for a three pane view
// Use setFrontView/Top/Right with 0-2 index to setup the actual views.
// (in a separate function to allow window gutters)
//
dali.threePane = function() {
  "use strict";
  dali.onePane();

  var stage = dali.stage;
  var taskList = stage.getRenderTaskList();

  var defaultTask = taskList.getTask(0);
  var defaultCamera = defaultTask.getCameraActor();
  var defaultCameraParent = defaultCamera.getParent();

  var t;
  t = taskList.createTask();

  var c = new dali.CameraActor(); // add camera for different viewpoint
  c.position = [0, 0, 800];
  c.orientation = [0, 1, 0, 180];
  c.parentOrigin = [0.5, 0.5, 0.5];
  c.anchorPoint = [0.5, 0.5, 0.5];
  t.setCameraActor(c);
  defaultCameraParent.add(c);
  c.delete(); // wrapper

  t.delete(); // wrapper

  t = taskList.createTask();

  c = new dali.CameraActor(); // add camera for different viewpoint
  c.position = [0, 0, 800];
  c.orientation = [0, 1, 0, 180];
  c.parentOrigin = [0.5, 0.5, 0.5];
  c.anchorPoint = [0.5, 0.5, 0.5];
  t.setCameraActor(c);
  defaultCameraParent.add(c);
  c.delete(); // wrapper

  t.delete(); // wrapper

  defaultCameraParent.delete(); // wrapper
  defaultCamera.delete(); // wrapper
  defaultTask.delete(); // wrapper

  taskList.delete(); // wrapper
};



//
// chain load main.js
//
var chainLoadScript = document.createElement("script");
chainLoadScript.src = "dali-toy.js";
document.body.appendChild(chainLoadScript);
