;(function (d, $) {

  var jQdm_adjustContent;

  jQdm_adjustContent = function(){

    // 初始設定
    var prop = {
      $window: $(window),
      snapTarget: '.column',
      snapSpeed: 500,
      snapInterval: 500
    }

    function init(){

        // 快取sanp元素(位置修正)
      var _$t = $(prop.snapTarget),
      _$w = prop.$window;

        // 頁面載入及縮放時，將snap元素調整視窗大小
      _$w.on('load', function(){
        fitWindowScale(_$t);
      });
      _$w.on('resize', function(){
        setTimeout( function() {
          fitWindowScale(_$t);
        }, 200);
      });

        // 將snap元素附加snap功能
      _$t.windows({
        snapping: true,
        snapSpeed: prop.snapSpeed,
        snapInterval: prop.snapInterval
      });

    }

    // 讓元素調整為視窗大小
    function fitWindowScale(_$t){
      var _w = window.innerWidth,
      _h = window.innerHeight,
      _$num = _$t.find('.number');

      _$t.css({
        height: _h
      });

      _$num.css({
        top: _h / 2 - _$num.height() / 2 - 20,
        left: _w / 2 - _$num.width() / 2
      });
    }

    init();

  };

  jQdm_adjustContent();

})(document, jQuery);