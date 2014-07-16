/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

var dali;
var document;
var console;

var step = 0;

var testfile = {
  "metadata": {
    "version": 4.3,
    "type": "Object",
    "generator": "ObjectExporter"
  },
  "geometries": [
    {
      "uuid": "2f167add-e571-47c2-9da2-6f0e45cc1119",
       "type": "Geometry",
      "data": {
        "vertices": [
          0.5,
          0.5,
          0.5,
          0.5,
          0.5,
            -0.5,
          0.5,
            -0.5,
          0.5,
          0.5,
            -0.5,
            -0.5,
            -0.5,
          0.5,
            -0.5,
            -0.5,
          0.5,
          0.5,
            -0.5,
            -0.5,
            -0.5,
            -0.5,
            -0.5,
          0.5
        ],
        "normals": [
          1,
          0,
          0,
            -1,
          0,
          0,
          0,
          1,
          0,
          0,
            -1,
          0,
          0,
          0,
          1,
          0,
          0,
            -1
        ],
        "uvs": [
          [
            0,
            1,
            0,
            0,
            1,
            0,
            1,
            1
          ]
        ],
        "faces": [
          56,
          0,
          2,
          3,
          0,
          1,
          2,
          0,
          0,
          0,
          0,
          56,
          0,
          3,
          1,
          0,
          2,
          3,
          0,
          0,
          0,
          0,
          56,
          4,
          6,
          7,
          0,
          1,
          2,
          1,
          1,
          1,
          1,
          56,
          4,
          7,
          5,
          0,
          2,
          3,
          1,
          1,
          1,
          1,
          56,
          4,
          5,
          0,
          0,
          1,
          2,
          2,
          2,
          2,
          2,
          56,
          4,
          0,
          1,
          0,
          2,
          3,
          2,
          2,
          2,
          2,
          56,
          7,
          6,
          3,
          0,
          1,
          2,
          3,
          3,
          3,
          3,
          56,
          7,
          3,
          2,
          0,
          2,
          3,
          3,
          3,
          3,
          3,
          56,
          5,
          7,
          2,
          0,
          1,
          2,
          4,
          4,
          4,
          4,
          56,
          5,
          2,
          0,
          0,
          2,
          3,
          4,
          4,
          4,
          4,
          56,
          1,
          3,
          6,
          0,
          1,
          2,
          5,
          5,
          5,
          5,
          56,
          1,
          6,
          4,
          0,
          2,
          3,
          5,
          5,
          5,
          5
        ]
      }
    }
  ],
  "materials": [
    {
      "uuid": "14D499F1-27EF-45BF-A457-FD24DAB11205",
      "type": "MeshPhongMaterial",
      "color": 11579568,
      "ambient": 11579568,
      "emissive": 0,
      "specular": 0,
      "shininess": 50,
      "opacity": 1,
      "transparent": false,
      "wireframe": false
    }
  ],
  "object": {
    "uuid": "BFEFB48D-0E6E-46A6-8568-5E258BA17078",
    "type": "Scene",
    "matrix": [
      1,
      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      1
    ],
    "children": [
      {
        "uuid": "aa901bec-9e47-4b3b-bf3c-4efb0fe5d298",
        "name": "Box",
        "type": "Mesh",
        "geometry": "2f167add-e571-47c2-9da2-6f0e45cc1119",
        "material": "14D499F1-27EF-45BF-A457-FD24DAB11205",
        "castShadow": true,
        "receiveShadow": true,
        "matrix": [
          1,
          0,
          0,
          0,
          0,
          1,
          0,
          0,
          0,
          0,
          1,
          0,
          0,
          0,
          0,
          1
        ]
      }
    ]
  }
};

dali.matrixByMatrix = function(A,B) {
  "use strict";

  var a11 = A[ 0 ], a12 = A[ 4 ], a13 = A[ 8 ], a14 = A[ 12];
  var a21 = A[ 1 ], a22 = A[ 5 ], a23 = A[ 9 ], a24 = A[ 13];
  var a31 = A[ 2 ], a32 = A[ 6 ], a33 = A[ 10], a34 = A[ 14];
  var a41 = A[ 3 ], a42 = A[ 7 ], a43 = A[ 11], a44 = A[ 15];

  var b11 = B[ 0 ], b12 = B[ 4 ], b13 = B[ 8 ], b14 = B[ 12];
  var b21 = B[ 1 ], b22 = B[ 5 ], b23 = B[ 9 ], b24 = B[ 13];
  var b31 = B[ 2 ], b32 = B[ 6 ], b33 = B[ 10], b34 = B[ 14];
  var b41 = B[ 3 ], b42 = B[ 7 ], b43 = B[ 11], b44 = B[ 15];

  var ret = [1,0,0,0,
             0,1,0,0,
             0,0,1,0,
             0,0,0,1];

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
};

dali.matrixByVector = function(A,v) {
  "use strict";

  var x = v[0];
  var y = v[1];
  var z = v[2];
  var w = 1;

  if(v.length == 4)
  {
    w = v[3];
  }

  return [
    A[0 ] * x + A[4 ] * y + A[8 ] * z + A[12] * w,
    A[1 ] * x + A[5 ] * y + A[9 ] * z + A[13] * w,
    A[2 ] * x + A[6 ] * y + A[10] * z + A[14] * w,
    A[3 ] * x + A[7 ] * y + A[11] * z + A[15] * w
  ];
};


dali.matrixDeterminant = function(A) {
  "use strict";

  var n11 = A[0], n12 = A[4], n13 = A[8 ], n14 = A[12];
  var n21 = A[1], n22 = A[5], n23 = A[9 ], n24 = A[13];
  var n31 = A[2], n32 = A[6], n33 = A[10], n34 = A[14];
  var n41 = A[3], n42 = A[7], n43 = A[11], n44 = A[15];

  var m0 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
  var m4 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
  var m8 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
  var m12= n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

  return n11 * m0 + n21 * m4 + n31 * m8 + n41 * m12;
};

dali.matrixByScalar = function(A, s) {
  "use strict";

  var ret = [1,0,0,0,
             0,1,0,0,
             0,0,1,0,
             0,0,0,1];

  ret[ 0 ] *= s; ret[ 4 ] *= s; ret[ 8 ] *= s; ret[ 12] *= s;
  ret[ 1 ] *= s; ret[ 5 ] *= s; ret[ 9 ] *= s; ret[ 13] *= s;
  ret[ 2 ] *= s; ret[ 6 ] *= s; ret[ 10] *= s; ret[ 14] *= s;
  ret[ 3 ] *= s; ret[ 7 ] *= s; ret[ 11] *= s; ret[ 15] *= s;

  return ret;
};

dali.matrixInverse = function(A) {
  "use strict";

  var n11 = A[0], n12 = A[4], n13 = A[8 ], n14 = A[12];
  var n21 = A[1], n22 = A[5], n23 = A[9 ], n24 = A[13];
  var n31 = A[2], n32 = A[6], n33 = A[10], n34 = A[14];
  var n41 = A[3], n42 = A[7], n43 = A[11], n44 = A[15];

  var ret = [1,0,0,0,
             0,1,0,0,
             0,0,1,0,
             0,0,0,1];

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
    return null;
  }

  dali.matrixByScalar(ret, 1 / det );

  return ret;
};

dali.degree = function(radians)
{
  return (radians*180.0)/Math.PI;
}

dali.radian = function(degrees)
{
  return (degrees/180.0)*Math.PI;
}

dali.vectorLength = function(array)
{
  var N = 3;// array.length;

  var length = 0;
  for(var i=0; i < N; ++i)
  {
    length += array[i] * array[i];
  }
  return Math.sqrt(length);
};

dali.vectorLengthSquared = function(array)
{
  var N = 3;// array.length;

  var length = 0;
  for(var i=0; i < N; ++i)
  {
    length += array[i] * array[i];
  }
  return length;
};

dali.normalize = function(array)
{
  var N = 3; // array.length;

  var length = 0;
  for(var i=0; i < 3; ++i)
  {
    length += array[i] * array[i];
  }
  length = Math.sqrt(length);

  if(length != 0)
  {
    var ret = [];
    for(var i = 0; i < N; ++i)
    {
      ret.push( array[i]/length );
    }
    for(var i = N; i < array.length; ++i)
    {
      ret.push( array[i] );
    }
    return ret;
  }
  else
  {
    return array;
  }

};

dali.axisAngleToQuaternion = function(axisAngle)
{
  var an = dali.normalize(axisAngle);
  var angle = axisAngle[ axisAngle.length-1 ];
  var halfTheta = angle * 0.5;
  var sinThetaByTwo = Math.sin(halfTheta);
  var cosThetaByTwo = Math.cos(halfTheta);
  return [ an[0] * sinThetaByTwo,
           an[1] * sinThetaByTwo,
           an[2] * sinThetaByTwo,
           cosThetaByTwo ];
};

dali.vectorDot = function(v1, v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
};

dali.vectorDot4 = function(v1, v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
};

dali.vectorCross = function(v1,v2)
{
  var v1_x = v1[0];
  var v1_y = v1[1];
  var v1_z = v1[2];

  var v2_x = v2[0];
  var v2_y = v2[1];
  var v2_z = v2[2];

  return [ v1_y * v2_z - v1_z * v2_y,
           v1_z * v2_x - v1_x * v2_z,
           v1_x * v2_y - v1_y * v2_x ];
};

dali.vectorByScalar = function(v1, s)
{
  var ret = [];
  for(var i =0, len = v1.length; i < len; i++)
  {
    ret.push(v1[i] * s);
  }
  return ret;
};

dali.vectorAdd = function()
{
  var ret = arguments[0];
  var l = ret.length;
  for(var i = 1, len = arguments.length; i < len; i++)
  {
    var v = arguments[i];
    for(var j=0; j < l; j++)
    {
      ret[j] += v[j];
    }
  }
  return ret;
};

dali.quatByQuat = function(q1, q2)
{
  var q1_x = q1[0];
  var q1_y = q1[1];
  var q1_z = q1[2];
  var q1_w = q1[3];

  var q2_x = q2[0];
  var q2_y = q2[1];
  var q2_z = q2[2];
  var q2_w = q2[3];

  return [q1_y * q2_z - q1_z * q2_y + q1_w * q2_x + q1_x * q2_w,
          q1_z * q2_x - q1_x * q2_z + q1_w * q2_y + q1_y * q2_w,
          q1_x * q2_y - q1_y * q2_x + q1_w * q2_z + q1_z * q2_w,
          q1_w * q2_w - dali.vectorDot(q1, q2)];
};

dali.quaternionToAxisAngle = function(q)
{
  var angle = Math.acos(q[3]);
  var sine = Math.sin(angle);

  if( sine === 0.0 )
  {
    throw "Cannot convert quaternion";
  }

  var sinf_theta_inv = 1.0 / sine;

  return [ q[0] * sinf_theta_inv,
           q[1] * sinf_theta_inv,
           q[2] * sinf_theta_inv,
           angle * 2.0
         ];
};

dali.worldToScreen = function(position, renderTask) {
  "use strict";
  var useFirstRenderTask = false;

  if(typeof renderTask === 'undefined') {
    useFirstRenderTask = true;
  } else if( renderTask === null ) { // null is an object
    useFirstRenderTask = true;
  }

  if(useFirstRenderTask)
  {
    var tasks = dali.getStage().getRenderTaskList();
    renderTask = tasks.getTask(0);
    tasks.delete() ; // wrapper
  }

  var camera  = renderTask.getCameraActor();
  var pos     = renderTask.getCurrentViewportPosition();
  var size    = renderTask.getCurrentViewportSize();

  var mat = dali.matrixByMatrix( camera.projectionMatrix, camera.viewMatrix );
  var p = dali.matrixByVector( mat, position );
  var depthRange = [0,1];
  var viewport = [ pos[0], pos[1], size[0], size[1] ];

  var div = 1 / p[3]; // homogenous divide

  camera.delete(); // wrapper

  return [
    (1 + p[0] * div) * viewport[2] / 2 + viewport[0],
    (1 - p[1] * div) * viewport[3] / 2 + viewport[1],
    (p[2] * div) * (depthRange[1] - depthRange[0]) + depthRange[0],
    div];
};

dali.screenCoordinates = function(actor, renderTask)
{
  var size = actor.size;
  var w2 = size[0]/2;
  var h2 = size[1]/2;
  var actorWorldMatrix = actor.worldMatrix;
  var actorWorldPosition=actor.worldPosition;

  return { topLeft : dali.worldToScreen( dali.matrixByVector(actorWorldMatrix, [-w2, -h2, 0]), renderTask ),
           bottomRight : dali.worldToScreen( dali.matrixByVector(actorWorldMatrix, [+w2, +h2, 0]), renderTask ),
           centre: dali.worldToScreen( actorWorldPosition )
         };
};


/** @private */
dali.internalGetGlyphImage = function(sFontFamily, sFontStyle, fFontSize, iChar) {
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

  if (0) // write data onto a test canvas to check its correct
  {
    // console.log(sFontStyle + " " + fFontSize + "px " + sFontFamily + " char:" + iChar);
    // console.log("GetGlyphImage" + imageData + " size " + imageData.width + "," + imageData.height);

    var testCanvas = document.getElementById("testCanvas");
    var testCtx = testCanvas.getContext("2d");

    step = step + fFontSize;
    testCtx.putImageData(imageData, fFontSize, step);
  }

  return imageData.data; // return uint8array; //
};

/** @private */
dali.internalSetupProperties = function(handle) {
  "use strict";

  var props = handle.getProperties();

  // console.log("props", props.size());

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
  return handle;
};

/** @private */
dali.internalSetupActorWrapper = function(handle) {
  dali.internalSetupProperties(handle);
  dali.internalAddActorMethods(handle);
  return handle;
}
  "use strict";

/** @private */
dali.internalGetImage = function() {
  "use strict";

  console.log("GetImage called:");

  // return
  // v["data"]
  // v["w"]
  // v["h"]
};

/** @private */
dali.internalGetImageMetadata = function() {
  "use strict";

  console.log("GetImageMetadata called");

  // return
  // v["w"] = 100;
  // v["h"] = 100;
};

dali.setCallbackGetGlyphImage(dali.internalGetGlyphImage);
dali.setCallbackGetImage(dali.internalGetImage);
dali.setCallbackGetImageMetadata(dali.internalGetImageMetadata);

//
//
// Takes a JS value and sets the property. Generally does the right thing without needing dali.PropertyXXX.
//
dali.DaliPropertyValue = function(object, name, value) {
  "use strict";

  var setProperty = false;
  if (typeof(value) === "boolean") {
    setProperty = new dali.PropertyBoolean(value);
  } else if (typeof(value) === "number") {
    setProperty = new dali.PropertyFloat(value);
  } else if (typeof(value) === "string") {
    setProperty = new dali.PropertyString(value);
  } else if (typeof(value) === "object") {
    if (value.constructor === dali.PropertyValue) {
      setProperty = value;
    } else {
      var p = object.getProperty(name);
      var type = p.getType();
      p.delete();
      // This shouldnt need .value on the end? Maybe because I'm currently in emscritpen devel branch and it will change
      if (type === dali.PropertyType.ROTATION.value) {
        if (value.length === 3) {
          setProperty = new dali.PropertyEuler(value);
        } else {
          setProperty = new dali.PropertyAxisAngle(value);
        }
      } else if (value.length) {
        if (value.length === 2) {
          setProperty = new dali.PropertyVector2(value);
        } else if (value.length === 3) {
          setProperty = new dali.PropertyVector3(value);
        } else if (value.length === 4) {
          setProperty = new dali.PropertyVector4(value);
        } else {
          // wo betide those who want length 2,3,4 arrays
          setProperty = new dali.PropertyArray(value);
        }
      } else {
        setProperty = new dali.PropertyMap(value);
      }
    }
  } else {
    throw object.toString() + " " + name.toString() + " " + value.toString();
  }

  return setProperty;
};


/** @private */
dali.marshallSetProperty = function(object, name, value) {
  "use strict";

  var setProperty = new dali.DaliPropertyValue(object, name, value);

  if (setProperty) {
    object.setProperty(name, setProperty);
    setProperty.delete();
  }
};


dali.getPropertyType = function(object, name) {
  "use strict";

  var p = object.getProperty(name);
  var type = p.getType();
  p.delete();
  return type;
};

//
// GetProperty.
// Needs to get the property value which needs to be manually deleted which
// this function does for you.
//

//1  BOOLEAN,         ///< A boolean type
//2  FLOAT,           ///< A float type
//3  INTEGER,         ///< An integer type
//4  UNSIGNED_INTEGER,///< An unsigned integer type
//5  VECTOR2,         ///< a vector array of size=2 with float precision
//6  VECTOR3,         ///< a vector array of size=3 with float precision
//7  VECTOR4,         ///< a vector array of size=4 with float precision
//8  MATRIX3,         ///< a 3x3 matrix
//9  MATRIX,          ///< a 4x4 matrix
//10 RECTANGLE,       ///< an integer array of size=4
//11 ROTATION,        ///< either a quaternion or an axis angle rotation
//12 STRING,          ///< A string type
//13 ARRAY,           ///< an array of Property::Value
//14 MAP,             ///< a string key to Property:value mapping
//   TYPE_COUNT       ///< The number of supported property types
/** @private */
dali.marshallGetProperty = function(object, name) {
  "use strict";

  var ret;
  var p = object.getProperty(name);
  if (!p) {
    throw new Error("Property doesnt exist?");
  }
  var type = p.getType();

  if (type === 0) {
    // raise?
    throw new Error("Property has no type?");
  } else if (type === 1) {
    ret = p.getBoolean();
  } else if (type === 2) {
    ret = p.getFloat();
  } else if (type === 3) {
    ret = p.getInteger();
  } else if (type === 4) {
    ret = p.getInteger();
  } else if (type === 5) {
    ret = p.getVector2();
  } else if (type === 6) {
    ret = p.getVector3();
  } else if (type === 7) {
    // VECTOR4
    ret = p.getVector4();
  } else if (type === 8) {
    // MATRIX3
    ret = p.getMatrix3();
  } else if (type === 9) {
    // MATRIX
    ret = p.getMatrix();
  }
  // else if( type === 10 ) // RECT<INT>
  // {
  //   ret = p.getMatrix();
  // }
  else if (type === 11) {
    // ROTATION
    ret = p.getRotation();
  } else if (type === 12) {
    ret = p.getString();
  } else if (type === 13) {
    // array
    ret = p.getArray();
  } else if (type === 14) {
    // map
    ret = p.getMap();
  }
  p.delete();
  return ret;
};

/** @private */
dali.internalDepthFirstCollection = function(actor, collection) {
  for (var i = 0; i < actor.getChildCount(); i++) {
    var a = actor.getChildAt(i); // adds properties in dotted
    collection.push(a);
    dali.internalDepthFirstCollection(a, collection);
  }
}

/** @private */
dali.internalAddActorMethods = function(actor) {
  "use strict";
  // some functions need to be adorned with properties that we add in the javascript layer
  actor.getParent = function() {
    var bareActorWrapper = this.__getParent();
    // add properties to the bare ActorWrapper
    dali.internalSetupActorWrapper(bareActorWrapper);
    return bareActorWrapper;
  };
  actor.findChildById = function(index) {
    var bareActorWrapper = this.__findChildById(index);
    dali.internalSetupActorWrapper(bareActorWrapper);
    return bareActorWrapper;
  };
  actor.findChildByName = function(name) {
    var bareActorWrapper = this.__findChildByName(name);
    if (bareActorWrapper.ok()) {
      dali.internalSetupActorWrapper(bareActorWrapper);
      return bareActorWrapper;
    } else {
      return null;
    }
  };
  actor.getChildAt = function(index) {
    var bareActorWrapper = this.__getChildAt(index);
    dali.internalSetupActorWrapper(bareActorWrapper);
    return bareActorWrapper;
  };
  actor.findAllChildren = function() {
    var col = [];
    dali.internalDepthFirstCollection(this, col);
    return col;
  };
  actor.getChildren = function() {
    var col = [];
    for (var i = 0, len = this.getChildCount(); i < len; i++) {
      var c = this.getChildAt(i);
      col.push(c);
    }
    return col;
  };
  actor.directChildren = actor.getChildren;

  actor.connectSignal = function(signalName, callback) {
    // wrap in closure so we can setup properties in . notation
    // and add actor methods to c++ raw ActorWrapper
    this.__connectSignal(
      signalName, (function(callback) {
        return function(actorWrapper) {
          dali.internalSetupActorWrapper(actorWrapper);
          callback(actorWrapper);
        };
      })(callback)
    );
  };

  actor.setImageData = function(imageData) {
    // imageData as returned from canvas.getImageData
    var uint8array = new Uint8Array(imageData.data);
    actor.__setImageRawRGBA(imageData.width, imageData.height, uint8array);
  };

  actor.setImageDataRGB = function(imageData) {
    // imageData as returned from canvas.getImageData
    var uint8array = new Uint8Array(imageData.data);
    actor.__setImageRawRGB(imageData.width, imageData.height, uint8array);
  };

  actor.registerProperty = function(name, value) {
    var ret = -1;
    if(typeof(value) != "object") // just simple types, bool, string, float
    {
      var propertyValue = new dali.DaliPropertyValue({}, name, value);
      ret = actor.__registerProperty(name, propertyValue);
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
    }
    return ret;
  };

  actor.registerAnimatedProperty = function(name, propertyValue) {
    var ret = -1;
    if(typeof(propertyValue) != "object") // just simple types, bool, string, float
    {
      var value = new dali.DaliPropertyValue({}, name, propertyValue);
      ret = actor.__registerAnimatedProperty(name, propertyValue);
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
    }
    return ret;
  };

};

dali.createSolidColorActor = function(color, border, borderColor, borderSize) {
  var a = dali.__createSolidColorActor(color, border, borderColor, borderSize);
  dali.internalSetupActorWrapper(a);
  return a;
};


function ObjectLoader(fileObject) {
  // cached
  this.self = this;
  this.meshByUUID = {};
  this.geomByUUID = {};
  this.matByUUID = {};

  this.fileObject = fileObject;
}

function __longToArray(v) {
  return [((v>>24)&0xFF)/255.0,
          ((v>>16)&0xFF)/255.0,
          ((v>>8)&0xFF)/255.0,
          (v&0xFF)/255.0];
}

function __isBitSet(value, bit)
{
  return (value&(1<<bit));
}

ObjectLoader.prototype.__getMaterial = function(uuid) {
  if( !(uuid in this.matByUUID) ) {
    for(var i=0, len = this.fileObject["materials"].length; i<len;i++ ) {
      var f_mat = this.fileObject["materials"][i];
      skewer.log(i + ":" + f_mat["uuid"] + " " + ( f_mat["uuid"] === uuid ) );
      if( f_mat["uuid"] === uuid ) {
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
  if( !(uuid in this.meshByUUID) ) {
    for(var i=0,len= this.fileObject["geometries"].length; i<len;i++) {
      var f_geom = this.fileObject["geometries"][i];
      if( f_geom["uuid"] === uuid ) {
        var f_indices, // file data
            f_posns,
            f_norms,
            f_uvs,
            f_faces,
            f_type;

        if("formatVersion" in f_geom["metadata"]) // then version 3.1
        {
          f_indices = f_geom["indices"];
          f_posns   = f_geom["vertices"];
          f_norms   = f_geom["normals"];
          f_uvs     = f_geom["uvs"];
          f_faces   = f_geom["faces"];
          if(f_faces) {
            f_type = f_geom["faces"][0]; // ["type"];
          }
        }
        else if(f_geom["type"] === "Geometry") // V4 clara io output? not standard???
        {
          f_indices = f_geom["data"]["indices"];
          f_posns   = f_geom["data"]["vertices"];
          f_norms   = f_geom["data"]["normals"];
          f_uvs     = f_geom["data"]["uvs"];
          f_faces   = f_geom["data"]["faces"];
          if(f_faces) {
            f_type = f_geom["data"]["faces"][0]; // ["type"];
          }
        }
        else if(f_geom["metadata"]["type"] === "Geometry") // V4
        {
          f_indices = f_geom["indices"];
          f_posns   = f_geom["vertices"];
          f_norms   = f_geom["normals"];
          f_uvs     = f_geom["uvs"];
          f_faces   = f_geom["faces"];
          if(f_faces) {
            f_type = f_geom["faces"][0]; // ["type"];
          }
        }
        else if(f_geom["metadata"]["type"] === "BufferGeometry") // V4
        {
          f_posns = f_geom["data"]["attributes"]["position"]["array"];
          f_norms = f_geom["data"]["attributes"]["norms"]["array"];
          f_uvs = f_geom["data"]["attributes"]["uv"]["array"];
        }

        var isQuad              = __isBitSet( f_type, 0 );
        assert(!isQuad, "Not supported");
        var hasMaterial         = __isBitSet( f_type, 1 );
        assert(!hasMaterial, "Not Supported");
        var hasFaceUv           = __isBitSet( f_type, 2 );
        var hasFaceVertexUv     = __isBitSet( f_type, 3 );
        var hasFaceNormal       = __isBitSet( f_type, 4 );
        var hasFaceVertexNormal = __isBitSet( f_type, 5 );
        var hasFaceColor        = __isBitSet( f_type, 6 );
        var hasFaceVertexColor  = __isBitSet( f_type, 7 );

        var mesh = new dali.MeshDataWrapper();
        var verts= new dali.VectorVertex();
        var vert = [];//new dali.Vertex();
        for(var i =0, len = f_posns.length/3; i < len; i++) {
          vert.push(f_posns[(i*3)+0]);
          vert.push(f_posns[(i*3)+1]);
          vert.push(f_posns[(i*3)+2]);
          if(f_norms) {
            vert.push(f_norms[(i*3)+0]);
            vert.push(f_norms[(i*3)+1]);
            vert.push(f_norms[(i*3)+2]);
          } else {
            vert.push(0);
            vert.push(0);
            vert.push(0);
          }

          if(f_uvs) {
            vert.push(f_uvs[(i*3)+0]);
            vert.push(f_uvs[(i*3)+1]);
          } else {
            vert.push(0);
            vert.push(0);
          }
          verts.push_back(vert);
        }

        var faces = null;
        if(f_faces) {
          faces = new dali.VectorFaceIndex();
          for(var i =1, len = f_faces.length; i < len; i++) {
            faces.push_back( f_faces[i] );
          }
        }

        if(f_indices) {
          faces = new dali.VectorFaceIndex();
          for(var i =1, len = f_indices.length; i < len; i++) {
            faces.push_back( f_indices[i] );
          }
        }

        if(!faces) {
          faces = [];
          for(var i=0, len = f_posns.length; i < len; i++) {
            faces.push(i);
          }
        }

        var material = this.__getMaterial(uuid_material);
        mesh.setLineData(verts, faces, material);
        this.meshByUUID[uuid] = mesh;
        verts.delete();
        faces.delete();
        break;
      } // if uuid found
    } // for geom in geometries
  }// if uid ! in meshByUUID

  return this.meshByUUID[uuid];
};

ObjectLoader.prototype.delete = function() {
  for(var a in this.meshByUUID) {
    a.delete();
  }
  this.meshByUUID ={};
  for(var b in this.matByUUID) {
    b.delete();
  }
  this.matByUUID = {};
};

ObjectLoader.prototype.createMeshActors = function() {
  var ret = [];
  if( "object" in this.fileObject ) {
    for( var i=0, len=this.fileObject["object"]["children"].length; i < len; i++) {
      var child = this.fileObject["children"];
      if( child["type"] === "Mesh" ) {
        var meshData = this.__getMeshData( child["geometry"],
                                           child["material"] );
        ret.push( dali.__createMeshActor(meshData) );
        meshData.delete();
      }
    }
  }

  var parent;

  if(ret) {
    parent = new dali.Actor();
    for(var a in ret) {
      parent.add(a);
      a.delete();
    }
  }

  return parent;
};

dali.createMeshActor = function(threeDjs_formatV4) {

  var loader = new ObjectLoader(threeDjs_formatV4);

  return loader.createMeshActor();
};

dali.hitTest = function(x,y) {
  var a = dali.__hitTest(x,y);
  if( a.ok() ) {
    dali.internalSetupActorWrapper(a);
    return a;
  } else {
    return null;
  }
};

dali.create = function(name) {
  "use strict";

  var handle = dali.createActor(name);

  if (handle.ok()) {
    dali.internalAddActorMethods(handle);
  } else {
    handle = dali.createAny(name);
  }

  dali.internalSetupProperties(handle);

  return handle;
};

//
// Adds and animateTo( ...property... ) to an animation object set.
//
dali.animateTo = function(animation, object, propertyName, value, alphaFunction, delay, duration) {
  "use strict";

  var propertyValue = new dali.DaliPropertyValue(object, propertyName, value);
  if (propertyValue) {
    animation.animateTo(object, propertyName, propertyValue, alphaFunction, delay, duration);
    propertyValue.delete();
  } else {
    throw new Error("Unknown property?");
  }

};

dali.animateBy = function(animation, object, propertyName, value, alphaFunction, delay, duration) {
  "use strict";

  var propertyValue = new dali.DaliPropertyValue(object, propertyName, value);
  if (propertyValue) {
    animation.animateBy(object, propertyName, propertyValue, alphaFunction, delay, duration);
    propertyValue.delete();
  } else {
    throw new Error("Unknown property?");
  }
};

dali.debugPrintProperties = function(typeName) {
  "use strict";

  var t = dali.getTypeRegistry();
  // var types = t.getTypeNames()
  var info = t.getTypeInfo(typeName);
  var props = info.getProperties();
  for (var i = 0; i < props.size(); i++) {
    console.log(i + ":" + props.get(i));
  }
};

dali.debugPrintTypes = function() {
  "use strict";

  var t = dali.getTypeRegistry();
  var types = t.getTypeNames();
  for (var i = 0; i < types.size(); i++) {
    console.log(types.get(i));
  }

};

dali.debugFindFirstActor = function(actor, predicateFunction) {
  "use strict";
  for(var i = 0, len = actor.getChildCount(); i < len; i++) {
    var a = actor.getChildAt(i);
    var found = predicateFunction(a);
    if(found) {
      return a;
    }
    var child = dali.debugFindFirstActor(a, predicateFunction);
    if(child)
    {
      return child;
    }
    a.delete();
  }
  return null;
};

dali.debugDepthVisit = function(actor, operation, dontDelete) {
  "use strict";
  for(var i = 0, len = actor.getChildCount(); i < len; i++) {
    var a = actor.getChildAt(i);
    var done = operation(a);
    if(!done) {
      return false;
    }
    if(!dali.debugDepthVisit(a, operation, dontDelete))
    {
      return false;
    }
    var doit = true;
    if ( dontDelete !== undefined )
    {
      if( dontDelete )
      {
        doit = false;
      }
    }
    if(doit)
    {
      a.delete();
    }
  }
  return true;
};

dali.operationPrintProperty = function(property, all) {
  return (function(actor) {
    if(property in actor) {
      dali.log(actor.getId() + "property:" + actor[property]);
    } else {
     dali.log( actor.getId() + "property:n/a");
    }
    return all;
  });
};

dali.predicatePropertyEquals = function(property, value) {
  return (function(actor) {
    if(property in actor) {
      if(actor[property] === value) {
        return true;
      }
    }
    return false;
  });
};

dali.__theStage = (function() {
  this.stageWrapper = dali.__getStage();

  this.add = function(actor) {
    this.stageWrapper.__add(actor);
  };

  this.remove = function(actor) {
    this.stageWrapper.__remove(actor);
  };

  this.getRootLayer = function() {
    var wrapper = this.stageWrapper.__getRootLayer();
    dali.internalSetupActorWrapper(wrapper);
    return wrapper;
  };

  this.getRenderTaskList = function() {
    var wrapper = this.stageWrapper.__getRenderTaskList();
    wrapper.createTask = function()
    {
      var t = this.__createTask();
      t.getCameraActor = function()
      {
        var a = this.__getCameraActor();
        if(a.ok())
        {
          dali.internalSetupActorWrapper(a);
        }
        return a;
      };
      return t;
    };
    wrapper.getTask = function(id)
    {
      var t = this.__getTask(id);
      t.getCameraActor = function()
      {
        var a = this.__getCameraActor();
        if(a.ok())
        {
          dali.internalSetupActorWrapper(a);
        }
        return a;
      };
      return t;
    };
    return wrapper;
  };

  this.setBackgroundColor = function(vector4) {
    return this.stageWrapper.__setBackgroundColor(vector4);
  };

  return this;
})();

dali.getStage = function() {
  return dali.__theStage;
}

dali.init = function() {
  "use strict";

  var t = dali.getTypeRegistry();
  var types = t.getTypeNames();

  //
  // Add creation functions so "var image = new dali.ImageActor()" works
  //
  for (var i = 0; i < types.size(); i++) {
    // anon function because of closure with defineProperty
    // (if just variable in loop then the variable 'address' is captured, not the value
    //  so it becomes last value set)
    (function(name) {
      // @todo Dali error?? name lengths should never be zero
      if (name.length && name != "Animation") {
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
    })(types.get(i));
  }

  //
  // Special wrappings overriding the dali.Create() which just returns a HandleWrapper
  //
  dali.Animation = dali.AnimationWrapper;

}(); // call init


//
// Multi plane views
//
dali.singlePane = function()
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();

  for(var i =1, len =taskList.getTaskCount(); i < len;i++)
  {
    var task = taskList.getTask(1);
    taskList.removeTask(task);
    task.delete();
  }

  var task = taskList.getTask(0);
  var w = dali.canvas.width;
  var h = dali.canvas.height;
  task.setViewportSize([w,h]);
  task.setViewportPosition([0,0]);
  task.delete();

  taskList.delete();
};

dali.threePane = function()
{
  dali.singlePane();

  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  var w = dali.canvas.width;
  var h = dali.canvas.height;
  var first = taskList.getTask(0);
  first.setViewportSize([w/2,h]);
  var camera = first.getCameraActor();
  first.delete(); //wrapper

  var second = taskList.createTask();
  second.setViewportSize([w/2,h/2]);
  second.setViewportPosition([w/2,0]);
  var second_ca = new dali.CameraActor();
  stage.add(second_ca);

  //second_ca.parentOrigin = [0.5, 0.5, 0.5];
  second_ca.inheritRotation = true;
  var q1 = dali.axisAngleToQuaternion([0,1,0, dali.radian(180)]) ; // yaw around to look at scene down -ve z
  var q2 = dali.axisAngleToQuaternion([1,0,0, dali.radian(-90)]);  // pitch to look at scene
  var q = dali.quaternionToAxisAngle( dali.quatByQuat(q1, q2) );
  second_ca.position = [0,-camera.position[2],0]; // camera.position; // [ camera.position[2], 0, 0 ];
  second_ca.rotation = [q[0], q[1], q[2], dali.degree(q[3])]; // tbd; should really all be in radians
  second.setCameraActor(second_ca);

  second_ca.delete(); // wrapper
  second.delete(); // wrapper

  var third = taskList.createTask();
  third.setViewportSize([w/2,h/2]);
  third.setViewportPosition([w/2,h/2]);
  var third_ca = new dali.CameraActor();
  stage.add(third_ca);
  third.setCameraActor(third_ca);

  third_ca.delete(); // wrapper
  third.delete(); // wrapper

  taskList.delete(); // wrapper

};

//
// chain load main.js
//
var s = document.createElement("script");
s.src = "test-devel.js";
document.body.appendChild(s);
