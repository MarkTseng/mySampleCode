(function( d, $){
  
  // 變數設定
  var param = {
    overlayContainer: 'bgOverlayContainer',
    overlaySelector: 'overlayPattern',
    mobileMoviePrependTo: '#wrapper',
    movie: 'movie/tilt_shift',
    extension : ['.mp4','.ogg','.webm','.png']
  };
  
  // 瀏覽環境判別函數
  var _ua = (function(){
    var ua = navigator.userAgent;
    return {
      Mobile: typeof window.orientation != "undefined",
	  	Firefox: window.sidebar,
      Android: ua.search(/Android/) != -1 && ua.search(/Mobile/ != -1)
    };
  })();
  
  // 初始設定
  var init = function() {
    
    // 設定中必要變數的初始設定
    var $BV, $vcm,
    _bv = {
      movie : param.movie,
      suffix : '',
      extension : param.extension,
      source : function(_t){
        _t = _t || 0;
        return _bv.movie + _bv.suffix + _bv.extension[_t]; },
      isPlaying : false
    };
    
    // 全螢幕背景動畫無法正常運行時的替代處理
    if (_ua.Mobile) {
      $('body').addClass('mobile');
      
      // Android版及Firefox以外的替代處理
      if(!_ua.Firefox) {
      setTimeout(function(){ scrollTo(0,1); }, 100);

      $vcm = $('<video id="videoContainer_mobile" src="'+_bv.source(0)+'" poster="'+_bv.source(3)+'" controls>').prependTo(param.mobileMoviePrependTo);
      
      if(_ua.Android) {
        $vcm.on('click', function(){
          $(this).width('100%').height('auto');
          this.play();
          _bv.isPlaying = true;
        });
      }

      $(window).on('load resize orientationchange', function()
        {
          changeMovieSize($vcm);
        });
    }
  } else {
    //
    var _ovc = $('<div id="' + param.overlayContainer + '">').appendTo($('body'));
    _ovc.addClass(param.overlaySelector);

    $BV = new $.BigVideo({
      useFlashForFirefox: false,
      forceAutoplay: false,
      controls: false,
      doLoop: true,
      container: $('body')
    });
    $BV.init();
    $BV.show(_bv.source(), {
      altSource: _bv.source(1),
      ambient: true
      });
    }
    
    // 視窗縮放時動畫縮放的處理
    function changeMovieSize($vcm){
      var _w = window;
      var _ww = _w.innerWidth, _wh = _w.innerHeight, _wo = Math.abs(_w.orientation);
      var _adj = _ua.Android_mobile ? -0.1 : 0;
      if(_wo === 90) {
        $vcm.width(_ww);
        $vcm.height(_wh * (0.9 + _adj));
      } else {
        $vcm.width(_ww);
        $vcm.height(_wh * 0.45);
      }
      if(_ua.Android && _bv.isPlaying){
         setTimeout(function(){
          $vcm.width('100%').height('auto');
          $vcm.play();
        }, 1000);
      }
    }
  };

  init();

})( document, jQuery);