
QUnit.module("Views", {
    setup : function() {
        QUnit.stop();
        var that = this;
        this.cb = function(e) {
          QUnit.ok(true, "Scene loaded");
          var iframe = document.getElementById('daliframe');
          that.doc =  iframe.contentDocument || iframe.contentWindow.document;
          that.doc.Module.postDaliWrapperRun = function() {
            dali = that.doc.Module;
            QUnit.start();
          };
        };
        loadDocument("dali-page.html"+window.location.search, this.cb);
    },
    teardown : function() {
        var v = document.getElementById("daliframe");
        v.removeEventListener("load", this.cb, true);
    }
});

QUnit.test( "Exercise view helpers", function( assert ) {

  var done1 = assert.async();
  var done2 = assert.async();
  var done3 = assert.async();

  var w = dali.canvas.width;
  var h = dali.canvas.height;
  var col = dali.getClearColor(0);
  console.log(col);

  function one() {
    dali.onePane();
    dali.setFrontView(0, 0, 0, w, h);
    dali.setClearColor(0, col);
    done3();
  }

  function three() {
    dali.threePane();
    dali.setClearColor(0, [0.4, 0, 0, 1]);
    dali.setClearColor(1, [0, 0.4, 0, 1]);
    dali.setClearColor(2, [0, 0, 0.4, 1]);
    dali.setFrontView(0, 0, 0, w / 2 - 5, h);
    dali.setTopView(1, w / 2, 0, w / 2, h / 2 - 5);
    dali.setRightView(2, w / 2, h / 2 + 5, w / 2, h / 2 - 5);

    window.setTimeout(one, 1000);
    done2();
  }

  function two() {
    dali.twoPane();
    dali.setFrontView(0, 0, 0, w / 2 - 10, h);
    dali.setTopView(1, 210, 0, w / 2 - 10, h);
    dali.setClearColor(0, [0.4, 0, 0, 1]);
    dali.setClearColor(1, [0, 0.4, 0, 1]);

    window.setTimeout(three, 1000);
    done1();
  }

  //one();

  window.setTimeout(two, 1000);

});
