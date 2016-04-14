;(function (d, $) {

  var jQdm_adjustContent;

  jQdm_adjustContent = function(){

      // 初始設定
    var prop = {
      $window: $(window),
      snapTarget: '.column',
      snapSpeed: 500,
      snapInterval: 500,
      transitionSpeed: 1200,
      transitionEasing: 'easeInOutBack',
      transitionEasingException: 'easeInOutQuint',
      mainNavigation: '#mainNavigation',
      deferredImage: 'img.deferred',
      dataPossessor: [],
      currentSelector: '.current'
    }

    function init(){

        // 快取sanp元素(位置修正)
      var _$t = $(prop.snapTarget),
      _$w = prop.$window;

        // 頁面載入及縮放時，將snap元素調整視窗大小
      _$w.on('load', function(){
        setTargetPostion(_$t);
        fitWindowScale(_$t);
      });
      _$w.on('resize', function(){
        setTimeout( function() {
          setTargetPostion(_$t);
          fitWindowScale(_$t);
        }, 200);
      });
        // 頁面捲動時確認當前頁面的處理
      _$w.on('scroll', function(){
        changeCurrent();
      });
        // 頁面載入一段時間後確認當前位置
      setTimeout( function(){
        setTargetPostion(_$t);
      }, 100);


        // 將snap元素附加snap功能
      _$t.windows({
        snapping: true,
        snapSpeed: prop.snapSpeed,
        snapInterval: prop.snapInterval
      });

        // 瀏覽列的點擊事件
      $(prop.mainNavigation).find('a').on('click', function(e){
        if(typeof e.preventDefault() !== undefined){
          e.preventDefault();
        }

          // 快取連結的目標及設定Easing
        var _href = $(this).attr('href'),
        _ease = $(this).parent().index() == 0 ? prop.transitionEasingException : prop.transitionEasing;

          // 動畫移動至目標
        if(_href && $(_href).length > 0 ) {
          $(_href).animatescroll({
            scrollSpeed: prop.transitionSpeed,
            easing: _ease
          });
        }
      });

      // 圖片延遲載入設定
      $(prop.deferredImage).lazyload({
        threshold : 10,
        effect: 'fadeIn',
        effectspeed: 1000
      });

    }

    function setTargetPostion(_$t){
      prop.dataPossessor = [];
      _$t.each(function(){
        prop.dataPossessor.push($(this).offset().top);
      });
      changeCurrent();
    }

    function changeCurrent(){
      var _$w = prop.$window,
      _y = _$w.scrollTop() - _$w.height(),
      _dp = prop.dataPossessor,
      _nav = $(prop.mainNavigation).find('li'),
      _cur = replaceString(prop.currentSelector),
      i;

      for( i = 0; i < _dp.length; i++) {
        if( _y < _dp[i]){
          _nav.removeClass(_cur).eq(i).addClass(_cur);
          return;
        }
      }
    }

      // 讓元素調整為視窗大小
    function fitWindowScale(_$t){
      var _w = window.innerWidth,
      _h = window.innerHeight,
      _$p = _$t.find('.photo');

      _$t.css({
        height: _h
      });

      _$p.css({
        top: _h / 2 - _$p.height() / 2 - 40
      });
    }

      // 字串取代處理
    function replaceString(_str, _bf, _af, _flg){
      var _reg = new RegExp(_bf || '[\\.#]', _flg || '');
      return _str ? _str.replace(_reg, _af || '') : false;
    }

    init();

  };

  jQdm_adjustContent();

})(document, jQuery);