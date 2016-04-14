(function(){

  var options = {
    minWidth: 800,
    minHeight: 600,
    images: {
     dir: 'img/',
     path: [
     '01.jpg',
     '02.jpg',
     '03.jpg'
     ],
     current: 0,
     duration: 8000
    }
  };
  
  //　變數初始化與元素的快取
  var
    $window = $(window),
    $contents = $('#contentContainer'),
    $background = $('<div id="background">').prependTo($contents),
    imgs = [];

  //　初始化
  function init() {
    //　圖片預載
    var _imgspath = options.images.path,
    _imgsdir = options.images.dir,
    _rnd = Math.floor(Math.random()*_imgspath.length);
    
    for(var i = 0; len = _imgspath.length, i < len; i++){
     imgs.push(jQuery("<img>").attr("src", _imgsdir + _imgspath[i]));
    }
    imgs.current = _rnd;
     
    setTimeout( function(){
      changeBgImage(_rnd);
      $contents.css('background','none').fadeIn();
      }, 1200);

    resize();
  };
  
  //　背景圖切換處理
  function changeBgImage(_cur) {
    var _images = options.images,  _$img;
       
    _cur = _cur > _images.path.length-1 ? 0 : _cur;
    _$img = imgs[_cur].appendTo($background).hide();
    _images.current = ++_cur;
    
    setTimeout( function(){ adjustImage(_$img) }, 100);
    setTimeout( function(){ changeBgImage(_cur) }, options.images.duration || 2000);
  }
  
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
      left : moveX,
      top : moveY
   });
   _obj.fadeIn(1000);
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
   init();
  });

}());