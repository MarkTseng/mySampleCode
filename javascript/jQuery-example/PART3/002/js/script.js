(function( d, $){

  var _ua = (function(){
    var ua = navigator.userAgent;
    return {
      Mobile: typeof window.orientation != "undefined"
    };
  })();

  var init = function() {
    
    var $TB = $('<div id="videoContainer">').prependTo('body'),
    _option = {
      videoId : '-bCKCl3Gj1M',
      mute: false
    };

    var _ovc = $('<div id="bgOverlayContainer">').appendTo($('body'));
    _ovc.addClass('overlayPattern');
       
    $(window).on('resize', function(){
        _ovc.css({
          height : window.innerHeight
        });
    });
    
    if(_ua.Mobile) {
      $('body').addClass('mobile');
      var _tpl = '<iframe width="640" height="360" src="//www.youtube.com/embed/'+_option.videoId+'" frameborder="0" allowfullscreen></iframe>';
      $TB.append(_tpl);
    } else {
      $TB.tubular(_option);
    }
  };

  init();

})( document, jQuery);