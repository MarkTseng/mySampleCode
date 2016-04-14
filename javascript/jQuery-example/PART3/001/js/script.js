(function(){

  var options = {
    minWidth: 800,
    minHeight: 600,
  };

  //　元素快取
  var
    $window = $(window),
    $contents = $('#contentContainer'),
    $background = $('#background'),
    $img = $background.find('img');

  //　將背景圖調整為畫面置中
  function adjustImage(_obj) {
    var
    ww = $window.width(),
    wh = $window.height(),
    iw = _obj.width(),
    ih = _obj.height(),
    scale = Math.max( ww / iw, wh / ih ),
    sw = Math.floor( iw * scale ),
    sh = Math.floor( ih * scale ),
    moveX = Math.floor( (ww - sw) / 2 ),
    moveY = Math.floor( (wh - sh) / 2 );
    
    _obj.css({
      width: sw,
      height: sh,
      left: moveX,
      top: moveY
      });
  }

  //　配合瀏覽器視窗大小重新調整圖片外框大小
  function resize() {
    var
    ww = $window.width(),
    wh = $window.height(),
    _opw = options.minWidth,
    _oph = options.minHeight,
    _res = {
     w: _opw > ww ? _opw : ww,
     h: _oph > wh ? _oph : wh
    };

    $background.css({
     width: _res.w,
     height: _res.h
    });

    $contents.css({
     width: _res.w,
     height: _res.h
    });
  
    $background.find('img').each(function(){
      adjustImage($(this));
    });
  }

  $window.on('resize', resize);
  $window.on('load', function(){
   setTimeout( function(){
    $contents.css('background','none').fadeIn();
   }, 400);
   resize();
  });

}());