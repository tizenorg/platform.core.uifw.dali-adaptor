/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

var dali;
var document;
var console;

var step = 0;

var testfile = { // output from clara.io "Threejs scene output"
  "metadata": {
    "version": 4.3,     // This isnt as its documented on threejs website
    "type": "Object",   // and the general format looks more like format V3.
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

  var ret = [1,0,0,0,
             0,1,0,0,
             0,0,1,0,
             0,0,0,1];

  ret[0 ] = A[ 0] * B[ 0] + A[ 1] * B[ 4] + A[ 2] * B[ 8] + A[ 3] * B[12];
  ret[4 ] = A[ 4] * B[ 0] + A[ 5] * B[ 4] + A[ 6] * B[ 8] + A[ 7] * B[12];
  ret[8 ] = A[ 8] * B[ 0] + A[ 9] * B[ 4] + A[10] * B[ 8] + A[11] * B[12];
  ret[12] = A[12] * B[ 0] + A[13] * B[ 4] + A[14] * B[ 8] + A[15] * B[12];

  ret[1 ] = A[ 0] * B[ 1] + A[ 1] * B[ 5] + A[ 2] * B[ 9] + A[ 3] * B[13];
  ret[5 ] = A[ 4] * B[ 1] + A[ 5] * B[ 5] + A[ 6] * B[ 9] + A[ 7] * B[13];
  ret[9 ] = A[ 8] * B[ 1] + A[ 9] * B[ 5] + A[10] * B[ 9] + A[11] * B[13];
  ret[13] = A[12] * B[ 1] + A[13] * B[ 5] + A[14] * B[ 9] + A[15] * B[13];

  ret[2 ] = A[ 0] * B[ 2] + A[ 1] * B[ 6] + A[ 2] * B[10] + A[ 3] * B[14];
  ret[6 ] = A[ 4] * B[ 2] + A[ 5] * B[ 6] + A[ 6] * B[10] + A[ 7] * B[14];
  ret[10] = A[ 8] * B[ 2] + A[ 9] * B[ 6] + A[10] * B[10] + A[11] * B[14];
  ret[14] = A[12] * B[ 2] + A[13] * B[ 6] + A[14] * B[10] + A[15] * B[14];

  ret[3 ] = A[ 0] * B[ 3] + A[ 1] * B[ 7] + A[ 2] * B[11] + A[ 3] * B[15];
  ret[7 ] = A[ 4] * B[ 3] + A[ 5] * B[ 7] + A[ 6] * B[11] + A[ 7] * B[15];
  ret[11] = A[ 8] * B[ 3] + A[ 9] * B[ 7] + A[10] * B[11] + A[11] * B[15];
  ret[15] = A[12] * B[ 3] + A[13] * B[ 7] + A[14] * B[11] + A[15] * B[15];

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
    A[ 0] * x + A[ 4] * y + A[ 8] * z + A[12] * w,
    A[ 1] * x + A[ 5] * y + A[ 9] * z + A[13] * w,
    A[ 2] * x + A[ 6] * y + A[10] * z + A[14] * w,
    A[ 3] * x + A[ 7] * y + A[11] * z + A[15] * w
  ];
};

dali.matrixByVector2 = function(A,v) {
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
    A[ 0] * x + A[ 1] * y + A[ 2] * z + A[ 3] * w,
    A[ 4] * x + A[ 5] * y + A[ 6] * z + A[ 7] * w,
    A[ 8] * x + A[ 9] * y + A[10] * z + A[11] * w,
    A[12] * x + A[13] * y + A[14] * z + A[15] * w
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
  return [ A[0 ] * s, A[1 ] * s, A[2 ] * s, A[3 ] * s,
           A[4 ] * s, A[5 ] * s, A[6 ] * s, A[7 ] * s,
           A[8 ] * s, A[9 ] * s, A[10] * s, A[11] * s,
           A[12] * s, A[13] * s, A[14] * s, A[15] * s
         ];
};

dali.matrixInverse = function(A) {
  "use strict";
  var ret = [1,0,0,0,
             0,1,0,0,
             0,0,1,0,
             0,0,0,1];

  ret[0] =   A[5]*A[10]*A[15] - A[5]*A[11]*A[14] - A[9]*A[6]*A[15] + A[9]*A[7]*A[14] + A[13]*A[6]*A[11] - A[13]*A[7]*A[10];
  ret[4] =  -A[4]*A[10]*A[15] + A[4]*A[11]*A[14] + A[8]*A[6]*A[15] - A[8]*A[7]*A[14] - A[12]*A[6]*A[11] + A[12]*A[7]*A[10];
  ret[8] =   A[4]*A[9]*A[15] - A[4]*A[11]*A[13] - A[8]*A[5]*A[15] + A[8]*A[7]*A[13] + A[12]*A[5]*A[11] - A[12]*A[7]*A[9];
  ret[12] = -A[4]*A[9]*A[14] + A[4]*A[10]*A[13] + A[8]*A[5]*A[14] - A[8]*A[6]*A[13] - A[12]*A[5]*A[10] + A[12]*A[6]*A[9];

  ret[1] =  -A[1]*A[10]*A[15] + A[1]*A[11]*A[14] + A[9]*A[2]*A[15] - A[9]*A[3]*A[14] - A[13]*A[2]*A[11] + A[13]*A[3]*A[10];
  ret[5] =   A[0]*A[10]*A[15] - A[0]*A[11]*A[14] - A[8]*A[2]*A[15] + A[8]*A[3]*A[14] + A[12]*A[2]*A[11] - A[12]*A[3]*A[10];
  ret[9] =  -A[0]*A[9]*A[15] + A[0]*A[11]*A[13] + A[8]*A[1]*A[15] - A[8]*A[3]*A[13] - A[12]*A[1]*A[11] + A[12]*A[3]*A[9];
  ret[13] =  A[0]*A[9]*A[14] - A[0]*A[10]*A[13] - A[8]*A[1]*A[14] + A[8]*A[2]*A[13] + A[12]*A[1]*A[10] - A[12]*A[2]*A[9];

  ret[2] =   A[1]*A[6]*A[15] - A[1]*A[7]*A[14] - A[5]*A[2]*A[15] + A[5]*A[3]*A[14] + A[13]*A[2]*A[7] - A[13]*A[3]*A[6];
  ret[6] =  -A[0]*A[6]*A[15] + A[0]*A[7]*A[14] + A[4]*A[2]*A[15] - A[4]*A[3]*A[14] - A[12]*A[2]*A[7] + A[12]*A[3]*A[6];
  ret[10] =  A[0]*A[5]*A[15] - A[0]*A[7]*A[13] - A[4]*A[1]*A[15] + A[4]*A[3]*A[13] + A[12]*A[1]*A[7] - A[12]*A[3]*A[5];
  ret[14] = -A[0]*A[5]*A[14] + A[0]*A[6]*A[13] + A[4]*A[1]*A[14] - A[4]*A[2]*A[13] - A[12]*A[1]*A[6] + A[12]*A[2]*A[5];

  ret[3] =  -A[1]*A[6]*A[11] + A[1]*A[7]*A[10] + A[5]*A[2]*A[11] - A[5]*A[3]*A[10] - A[9]*A[2]*A[7] + A[9]*A[3]*A[6];
  ret[7] =   A[0]*A[6]*A[11] - A[0]*A[7]*A[10] - A[4]*A[2]*A[11] + A[4]*A[3]*A[10] + A[8]*A[2]*A[7] - A[8]*A[3]*A[6];
  ret[11] = -A[0]*A[5]*A[11] + A[0]*A[7]*A[9] + A[4]*A[1]*A[11] - A[4]*A[3]*A[9] - A[8]*A[1]*A[7] + A[8]*A[3]*A[5];
  ret[15] =  A[0]*A[5]*A[10] - A[0]*A[6]*A[9] - A[4]*A[1]*A[10] + A[4]*A[2]*A[9] + A[8]*A[1]*A[6] - A[8]*A[2]*A[5];

  var det = A[0]*ret[0] + A[1]*ret[4] + A[2]*ret[8] + A[3]*ret[12];

  if ( det === 0 ) {
    throw "no inverse";
  }

  return dali.matrixByScalar(ret, 1 / det );
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

  var mat = dali.matrixByMatrix( camera.viewMatrix, camera.projectionMatrix );
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


dali.matrixIdentity = function() {
  return [1,0,0,0,
          0,1,0,0,
          0,0,1,0,
          0,0,0,1];
};

dali.matrixTransform = function(x,y,z) {
  return [1,0,0,0,
          0,1,0,0,
          0,0,1,0,
          x,y,z,1];
};


dali.screenToLocal = function(screenX, screenY, actor, renderTask) {

  var camera  = renderTask.getCameraActor();

  var pos     = renderTask.getCurrentViewportPosition();
  var size    = renderTask.getCurrentViewportSize();
  var viewport = [ pos[0], pos[1], size[0], size[1] ];
  var viewportX= pos[0];
  var viewportY= pos[1];
  var viewportW= size[0];
  var viewportH= size[1];
  var modelView = dali.matrixByMatrix( actor.worldMatrix, camera.viewMatrix );

  var inverseMvp = dali.matrixInverse(
    dali.matrixByMatrix( modelView, camera.projectionMatrix) );

  var screenPos = [ screenX - viewportX,
                    viewportH - (screenY - viewportY),
                    0.0, 1.0 ];

  screenPos[2] = 0.0;

  var oglScreenPos = [ (screenPos[0] / viewportW) * 2 - 1,
                           (screenPos[1] / viewportH) * 2 - 1,
                           (screenPos[2]) * 2 - 1,
                           1];


  var nearPoint = dali.matrixByVector(inverseMvp, oglScreenPos);

  if( 0.0 === nearPoint[3] )
    throw "Unproject near fails";

  nearPoint[3] = 1 / nearPoint[3];
  nearPoint[0] = nearPoint[0] * nearPoint[3];
  nearPoint[1] = nearPoint[1] * nearPoint[3];
  nearPoint[2] = nearPoint[2] * nearPoint[3];


  oglScreenPos[2] = 1.0 * 2 - 1;

  var farPoint = dali.matrixByVector(inverseMvp, oglScreenPos);

  if( 0.0 === farPoint[3] )
    throw "Unproject far fails";

  farPoint[3] = 1 / farPoint[3];
  farPoint[0] = farPoint[0] * farPoint[3];
  farPoint[1] = farPoint[1] * farPoint[3];
  farPoint[2] = farPoint[2] * farPoint[3];


  if(!(farPoint[2] < 0) && (nearPoint[2] > 0)) {
    throw "ray not crossing xy plane";
  }

  var dist = nearPoint[2] / (nearPoint[2] - farPoint[2]);

  var intersect = [ nearPoint[0] + (farPoint[0] - nearPoint[0]) * dist,
                    nearPoint[1] + (farPoint[1] - nearPoint[1]) * dist,
                    0.0 ];

  intersect[0] = intersect[0] + actor.size[0] * 0.5;
  intersect[1] = intersect[1] + actor.size[1] * 0.5;

  return intersect;
};


dali.screenCoordinates = function(actor, renderTask)
{
  var size = actor.size;
  var w2 = size[0]/2;
  var h2 = size[1]/2;
  var actorWorldMatrix = actor.worldMatrix;
  var actorWorldPosition=actor.worldPosition;

  return { topLeft : dali.worldToScreen( dali.matrixByVector(actorWorldMatrix, [-w2, -h2, 0]), renderTask ),
           topRight : dali.worldToScreen( dali.matrixByVector(actorWorldMatrix, [+w2, -h2, 0]), renderTask ),
           bottomRight : dali.worldToScreen( dali.matrixByVector(actorWorldMatrix, [+w2, +h2, 0]), renderTask ),
           bottomLeft : dali.worldToScreen( dali.matrixByVector(actorWorldMatrix, [-w2, +h2, 0]), renderTask ),
           centre: dali.worldToScreen( actorWorldPosition )
         };
};

dali.screenToActor = function(actor, screenPos, renderTask)
{
  // , function will return coordinates in relation to actor`s anchorPoint (client coordinates).
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
  var vpp     = renderTask.getCurrentViewportPosition();
  var vps    = renderTask.getCurrentViewportSize();

  var mat = dali.matrixByMatrix( camera.projectionMatrix, camera.viewMatrix );

  var inverseMvp = dali.matrixInverse(mat);

  var x = screenPos[0];
  var y = screenPos[1];
  var z = screenPos[2];

  var objectPos = dali.matrixByVector( inverseMvp,
                                       // normalized +-1
                                       [( (x-vpp[0])/ vps[0] ) *2.0 - 1.0,
                                        ( (y-vpp[1])/ vps[1] ) *2.0 - 1.0,
                                        ( z * 2.0) - 1.0,
                                        1.0 ] );

  if( objectPos[3] === 0.0 )
  {
    throw "Cannot find screen Position";
  }

  objectPos[3] = 1/objectPos[3];

  return [ objectPos[0] * objectPos[3],
           objectPos[1] * objectPos[3],
           objectPos[2] * objectPos[3],
           objectPos[3] ];
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
  "use strict";
  if(handle.ok())
  {
    dali.internalSetupProperties(handle);
    dali.internalAddActorMethods(handle);
  }
  return handle;
};


dali.internalSetupRenderTask = function(renderTask) {
  "use strict";
  renderTask.getCameraActor = function()
  {
    var a = this.__getCameraActor();
    if(a.ok())
    {
      dali.internalSetupActorWrapper(a);
    }
    return a;
  };

  Object.defineProperty(renderTask, "x", {
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
  Object.defineProperty(renderTask, "y", {
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
  Object.defineProperty(renderTask, "width", {
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
  Object.defineProperty(renderTask, "height", {
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

  return renderTask;
};


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
            f_faces;

        if( !("metadata" in f_geom) )
        {
          f_geom["metadata"] = {"type":""}; // Warning: modified input!?
        }

        if("formatVersion" in f_geom["metadata"]) // then version 3.1
        {
          f_indices = f_geom["indices"];
          f_posns   = f_geom["vertices"];
          f_norms   = f_geom["normals"];
          f_uvs     = f_geom["uvs"];
          f_faces   = f_geom["faces"];
        }
        else if(f_geom["type"] === "Geometry") // V4 clara io output? not standard???
        {
          f_indices = f_geom["data"]["indices"];
          f_posns   = f_geom["data"]["vertices"];
          f_norms   = f_geom["data"]["normals"];
          f_uvs     = f_geom["data"]["uvs"];
          f_faces   = f_geom["data"]["faces"];
        }
        else if(f_geom["metadata"]["type"] === "Geometry") // V4
        {
          f_indices = f_geom["indices"];
          f_posns   = f_geom["vertices"];
          f_norms   = f_geom["normals"];
          f_uvs     = f_geom["uvs"];
          f_faces   = f_geom["faces"];
        }
        else if(f_geom["metadata"]["type"] === "BufferGeometry") // V4
        {
          f_posns = f_geom["data"]["attributes"]["position"]["array"];
          f_norms = f_geom["data"]["attributes"]["norms"]["array"];
          f_uvs = f_geom["data"]["attributes"]["uv"]["array"];
        }

        var nUvLayers = 0;

        // disregard empty arrays
        for (var i = 0; i < this.fileObject.uvs.length; i++ ) {
          if ( this.fileObject.uvs[ i ].length )
            nUvLayers++;
        }

        var verts= new dali.VectorVertex();
        var vert = [];//new dali.Vertex();
        for(var i =0, len = f_posns.length/3; i < len; i++) {
          vert.push(f_posns[(i*3)+0]);
          vert.push(f_posns[(i*3)+1]);
          vert.push(f_posns[(i*3)+2]);

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
        for(var i=0, len = this.fileObject.materials.length; i < len; ++i)
        {
          // get materials and force them to be loaded up
          var mat = this.__getMaterial( this.fileObject.materials[i]["uuid"] );
        }

        var idx = 0;
        var idx_len = f_faces.length;
        var materialUUID = undefined;
        while(idx < idx_len)
        {
          var f_type = f_faces[idx++];
          var isQuad              = __isBitSet( f_type, 0 );
          var hasMaterial         = __isBitSet( f_type, 1 );
          var hasFaceUv           = __isBitSet( f_type, 2 );
          var hasFaceVertexUv     = __isBitSet( f_type, 3 );
          var hasFaceNormal       = __isBitSet( f_type, 4 );
          var hasFaceVertexNormal = __isBitSet( f_type, 5 );
          var hasFaceColor        = __isBitSet( f_type, 6 );
          var hasFaceVertexColor  = __isBitSet( f_type, 7 );

          var nVertices = 3;
          var faceVertexIndices;
          if( isQuad )
          {
            faces.push_back( f_faces[idx  ] );
            faces.push_back( f_faces[idx+1] );
            faces.push_back( f_faces[idx+2] );

            faces.push_back( f_faces[idx  ] );
            faces.push_back( f_faces[idx+2] );
            faces.push_back( f_faces[idx+3] );

            faceVertexIndices = [ f_faces[idx  ],
                                  f_faces[idx+1],
                                  f_faces[idx+2] ];

            idx += 4;
            nVertices = 4;
          }
          else
          {
            faces.push_back( f_faces[idx  ] );
            faces.push_back( f_faces[idx+1] );
            faces.push_back( f_faces[idx+2] );

            faceVertexIndices = [ f_faces[idx  ],
                                  f_faces[idx+1],
                                  f_faces[idx+2] ];

            idx += 3;
          }

          if( hasMaterial )
          {
            if( materialUUID === undefined )
            {
              materialUUID = this.fileObject.materials[ f_faces[idx] ]["uuid"];
            }
            else
            {
              // different material per face is bonkers - I'm not going to support it.
              if( this.fileObject.materials[ f_faces[idx] ]["uuid"] !== materialUUID )
              {
                throw "Faces with different materials is not supported";
              }
            }
            idx++;
          }


          if( hasFaceUv )
          {
            for(var i =0; i < nUvLayers; i++)
            {
              var uvLayer = self.fileObject.uvs[i];
              var uvIndex = f_faces[ idx++ ];
              var u = uvLayer[ uvIndex * 2 ];
              var v = uvLayer[ uvIndex * 2 + 1 ];
              // discarded - tbd ?
            }
          }

          if( hasFaceVertexUv )
          {
            for( var i = 0; i < nUvLayers; i++ )
            {
              var uvLayer = f_geom.uvs[ i ];
              var uvs = [];
              for ( var j = 0; j < nVertices; j++ )
              {
                var uvIndex = f_faces[ idx++ ];
                var u = uvLayer[ uvIndex * 2 ];
                var v = uvLayer[ uvIndex * 2 + 1 ];
                // discarded- tbd ?
              }
            }
          }

          if ( hasFaceNormal )
          {
            var  normalIndex = f_faces[ idx++ ] * 3;

            var x = f_geom.normals[ normalIndex ++ ];
            var y = f_geom.normals[ normalIndex ++ ];
            var z = f_geom.normals[ normalIndex ];

            for(var i=0; i < faceVertexIndices.length; i++)
            {
              var v = vert.get( faceVertexIndices[i] );

              v[4] += x;
              v[5] += y;
              v[6] += z;
            }
          }

          if( hasFaceVertexNormal )
          {
            for ( var i = 0; i < nVertices; i++ )
            {
              var normalIndex = faces[ idx ] * 3;
              var x = f_geom.normals[ normalIndex++ ];
              var y = f_geom.normals[ normalIndex++ ];
              var z = f_geom.normals[ normalIndex   ];

              var v = vert.get( faces[ idx ] );

              v[4] += x;
              v[5] += y;
              v[6] += z;

              idx += 1;
              // face.vertexNormals.push( normal );
            }
          }

          if ( hasFaceColor )
          {
            var color = f_faces[ idx++ ];
          }

          if ( hasFaceVertexColor )
          {
            for ( var i = 0; i < nVertices; i++ )
            {
              var colorIndex = faces[ idx++ ];
              var color = f_geom.colors[ colorIndex ];  // ??? f_geom.colors?
              // face.vertexColors.push( color );
            }
          }


// void MeshData::SetData(
//     const VertexContainer& vertices,
//     const FaceIndices&     faceIndices,
//     const BoneContainer&   bones,
//     Material               material )


          // mesh.SetData( vertices, faces, bones?, material?);

          // scope.faces.push( face );

// ***

        var faces = null;
        if(f_faces) {
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

        console.log(verts.size() + ":" + faces.size() + ":" + uuid_material);

        var material = this.__getMaterial(uuid_material);
        mesh.setLineData(verts, faces, material);
// void MeshData::SetData(
//     const VertexContainer& vertices,
//     const FaceIndices&     faceIndices,
//     const BoneContainer&   bones,
//     Material               material )
        }

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
  info.delete(); // wrapper
  t.delete(); // wrapper
};

dali.debugPrintTypes = function() {
  "use strict";

  var t = dali.getTypeRegistry();
  var types = t.getTypeNames();
  for (var i = 0; i < types.size(); i++) {
    console.log(types.get(i));
  }
  t.delete(); // wrapper
};


dali.__debugPrintParents = function(actor, list)
{
  var p=null;

  if(!actor.ok())
  {
    return;
  }

  try
  {
    p = actor.getParent();
    if(!p.ok())
      p = null;
  }
  catch(e)
  {
    // console.log("Cannot get parent", e);
  }

  if(p)
  {
    list.push(p);
    dali.__debugPrintParents(p, list);
  }
};

dali.debugPrintTree = function(actor)
{
  l = [];
  dali.__debugPrintParents(actor, l);
  var ti;
  console.log("---");
  for(var i = l.length-1; i>=0; i--)
  {
    var a = l[i];
    ti = a.getTypeInfo();
    console.log("|", Array(l.length-i).join("-"), ti.getName(), "P", a.position, "R", a.rotation, a.name);
    ti.delete();
  }
  ti = actor.getTypeInfo();
  console.log("*", Array(l.length+1).join("*"), ti.getName(), "P", actor.position, "R", actor.rotation, actor.name);
  ti.delete();

  var children = actor.getChildren();
  for(var j=0; j < children.length; j++)
  {
    var a  = children[j];
    ti = a.getTypeInfo();
    console.log("|", Array(l.length+1+1+j).join("-"), ti.getName(), "P", a.position, "R", a.rotation, a.name);
    ti.delete();
  }
};

dali.debugPrintRenderTask = function(rendertask)
{
  console.log("[X,Y]", rendertask.getCurrentViewportPosition());
  console.log("[W,H]", rendertask.getCurrentViewportSize());

  var c = rendertask.getCameraActor();
  if(!c.ok())
  {
    console.log("No Camera");
  }
  else
  {
    console.log("Camera Pos:", c.position);
    console.log("Camera Rot:", c.rotation);
    console.log("Camera Inherit:", c.inheritRotation);
    console.log("Camera ParentOrigin:", c.parentOrigin);
    console.log("Camera AnchorPoint:", c.anchorPoint);
    var p=null;
    try
    {
      p = c.getParent();
      if(!p.ok())
        p = null;
    }
    catch(e)
    {
      console.log("Cannot get parent", e);
    }

    if(!p)
    {
      console.log("Camera has no parent?");
    }
    else
    {
      var ti = p.getTypeInfo();
      console.log("Parent Name", ti.getName());
      ti.delete();
      p.delete();
    }
  }
};

dali.debugPrintRenderTasks = function()
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  for(var i =0; i < taskList.getTaskCount(); i++)
  {
    var t = taskList.getTask(i);
    console.log("RenderTask:", i);
    dali.debugPrintRenderTask(t);
    t.delete(); // wrapper
  }
  taskList.delete(); // wrapper
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

  this.self = this;

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

  this.getLayer = function(index) {
    var wrapper = this.stageWrapper.__getLayer(index);
    dali.internalSetupActorWrapper(wrapper);
    return wrapper;
  };

  this.getLayerCount = function() {
    return this.stageWrapper.getLayerCount();
  };

  this.getRenderTaskList = function() {
    var wrapper = this.stageWrapper.__getRenderTaskList();
    wrapper.createTask = function()
    {
      var t = this.__createTask();
      dali.internalSetupRenderTask(t);
      return t;
    };
    wrapper.getTask = function(id)
    {
      var t = this.__getTask(id);
      dali.internalSetupRenderTask(t);
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
};

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

dali.setClearColor = function(renderTaskIndex, color)
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  if(renderTaskIndex >= taskList.getTaskCount())
  {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);
  rendertask.setClearEnabled(true);
  rendertask.setClearColor(color);
};

dali.setFrontView = function(renderTaskIndex, x, y, w, h)
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  if(renderTaskIndex >= taskList.getTaskCount())
  {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);

  var c = rendertask.getCameraActor();
  assert(c.ok(), "Rendertask has no valid camera actor");

  rendertask.setViewportPosition([x,y]);
  rendertask.setViewportSize([w,h]);
  c.position = [0,0,800];
  c.rotation = [0,1,0,180];
  c.aspectRatio=w/h;

  c.delete(); // wrapper
  rendertask.delete(); // wrapper
  taskList.delete(); // wrapper
};

dali.setTopView = function(renderTaskIndex, x, y, w, h)
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  if(renderTaskIndex >= taskList.getTaskCount())
  {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);

  var c = rendertask.getCameraActor();
  assert(c.ok(), "Rendertask has no valid camera actor");

  rendertask.setViewportPosition([x,y]);
  rendertask.setViewportSize([w,h]);

  var q1 = dali.axisAngleToQuaternion([0,1,0, dali.radian(180)]) ; // yaw around to look at scene down -ve z
  var q2 = dali.axisAngleToQuaternion([1,0,0, dali.radian(-90)]);  // pitch to look at scene
  var q = dali.quaternionToAxisAngle( dali.quatByQuat(q1, q2) );

  c.position = [0, -800, 0]; // @todo; get 800 from dali not hard coded here
  c.rotation = [q[0], q[1], q[2], dali.degree(q[3])]; // @todo; should really all be in radians
  c.aspectRatio=w/h;

  c.delete(); // wrapper
  rendertask.delete(); // wrapper
  taskList.delete(); // wrapper
};

dali.setRightView = function(renderTaskIndex, x, y, w, h)
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  if(renderTaskIndex >= taskList.getTaskCount())
  {
    console.log("RenderTaskIndex out of bounds:", renderTaskIndex);
    taskList.delete(); // wrapper
    return;
  }
  var rendertask = taskList.getTask(renderTaskIndex);

  var c = rendertask.getCameraActor();
  assert(c.ok(), "Rendertask has no valid camera actor");

  rendertask.setViewportPosition([x,y]);
  rendertask.setViewportSize([w,h]);

  var q1 = dali.axisAngleToQuaternion([0,1,0, dali.radian(180)]) ; // yaw around to look at scene down -ve z
  var q2 = dali.axisAngleToQuaternion([0,1,0, dali.radian(90)]);  // yaw again to look from right
  var q = dali.quaternionToAxisAngle( dali.quatByQuat(q1, q2) );

  c.position = [800, 0, 0];
  c.rotation = [q[0], q[1], q[2], dali.degree(q[3])]; // @todo; should really all be in radians
  c.aspectRatio=w/h;

  c.delete(); // wrapper
  rendertask.delete(); // wrapper
  taskList.delete(); // wrapper
};

dali.onePane = function()
{
  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  var tasks = [];
  var i,len;

  for(i=1, len = taskList.getTaskCount(); i < len; i++)
  {
    tasks.push( taskList.getTask(i) );
  }

  for(i=0, len = tasks.length; i < len; i++)
  {
    var task = tasks[i];
    // delete the camera actors we created in twoPane and threePane
    var c = task.getCameraActor();
    if(c.ok())
    {
      var p = c.getParent();
      if(p.ok())
      {
        p.remove(c);
      }
      p.delete(); // wrapper
    }
    c.delete(); // wrapper

    taskList.removeTask( task );
    task.delete(); // wrapper
  }

  taskList.delete();
};

dali.twoPane = function()
{
  dali.onePane();

  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();

  var defaultTask = taskList.getTask(0);
  var defaultCamera=defaultTask.getCameraActor();
  var defaultCameraParent =defaultCamera.getParent();

  var t;
  t = taskList.createTask();

  var c = new dali.CameraActor(); // add camera for different viewpoint
  c.position = [0,0,800];
  c.rotation = [0,1,0,180];
  // c.aspectRatio=w/h;
  c.parentOrigin = [0.5,0.5,0.5];
  c.anchorPoint  = [0.5,0.5,0.5];
  t.setCameraActor(c);
  defaultCameraParent.add(c);
  c.delete(); // wrapper

  t.delete(); // wrapper

  defaultCameraParent.delete(); // wrapper
  defaultCamera.delete(); // wrapper
  defaultTask.delete(); // wrapper

  taskList.delete(); // wrapper
};

dali.threePane = function()
{
  dali.onePane();

  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();

  var defaultTask = taskList.getTask(0);
  var defaultCamera=defaultTask.getCameraActor();
  var defaultCameraParent =defaultCamera.getParent();

  var t;
  t = taskList.createTask();

  var c = new dali.CameraActor(); // add camera for different viewpoint
  c.position = [0,0,800];
  c.rotation = [0,1,0,180];
  // c.aspectRatio=w/h;
  c.parentOrigin = [0.5,0.5,0.5];
  c.anchorPoint  = [0.5,0.5,0.5];
  t.setCameraActor(c);
  defaultCameraParent.add(c);
  c.delete(); // wrapper

  t.delete(); // wrapper

  t = taskList.createTask();

  var c = new dali.CameraActor(); // add camera for different viewpoint
  c.position = [0,0,800];
  c.rotation = [0,1,0,180];
  // c.aspectRatio=w/h;
  c.parentOrigin = [0.5,0.5,0.5];
  c.anchorPoint  = [0.5,0.5,0.5];
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
var s = document.createElement("script");
s.src = "test-devel.js";
document.body.appendChild(s);

