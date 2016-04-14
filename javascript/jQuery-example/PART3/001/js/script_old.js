(function(){

  var options = {
    minWidth: 800,
    minHeight: 600
  }

  var $window = $(window),
      $contents = $('#contentContainer'),
      $background = $('#background'),
      $img = $background.find('img');

  var iw = $img.width(),
      ih = $img.height();

  $contents.css({
    'width': options.minWidth,
    'height': options.minHeight
  });

  function resize() {

    var ww = $window.width(),
        wh = $window.height();

    $background.css({
      'width': '100%',
      'height': '100%'
    });

    if(options.minWidth > ww) {
      ww = options.minWidth;
      $background.css('width', options.minWidth);
    }
    if(options.minHeight > wh) {
      wh = options.minHeight;
      $background.css('height', options.minHeight);
    }

    var scale = Math.max( ww / iw, wh / ih ),
        sw = Math.floor( iw * scale ),
        sh = Math.floor( ih * scale ),
        moveX = Math.floor( (ww - sw) / 2 ),
        moveY = Math.floor( (wh - sh) / 2 );

    $contents.css({
      'width': ww,
      'height': wh
    });

    $img.css({
      'width': sw,
      'height': sh,
      'left' : moveX,
      'top' : moveY
    });

  }

  $window.on('resize', resize);
  $window.on('load', function(){
    $contents.fadeIn();
    resize();
  });

}());