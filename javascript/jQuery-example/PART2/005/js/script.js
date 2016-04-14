(function(){

	// options
	var options = {
		duration: 400,
		easing: 'easeOutCubic',
		auto: true,
		interval: 3000
	};

	// doms
	var $window = $(window),
		$container = $('#slider'),
		$element = $container.find('ul'),
		$list = $element.find('li'),
		$next = $container.find('#next'),
		$prev = $container.find('#prev'),
		shift = 2,
		lw = $list.find('img').width(),
		lh = $list.find('img').height(),
		len = $list.length,
		timer = '',
    isNarrowScreen = (function(){
      return $(window).width()<767;
    })();

	function initialize(){
		setup();
		rollover();
		$window.on('resize', resize);
		$next.on('click', function(){ slide(true); });
		$prev.on('click', function(){ slide(false); });
		load();
		if(options.auto) timer = setInterval(function(){ slide(true) }, options.interval);
	};

	function setup(){

    for(var i = shift; i > 0; i--) $element.find('li').eq(len - i).remove().prependTo($element)

    // 若螢幕大小太窄，則不配置左右元素
    if(!isNarrowScreen){
      // 新增兩個div.layer
      for(var i = 0; i < 2; i++) $('<div class="layer"></div>').insertAfter($element);

      // 將新增的div存入變數
      $leftlayer = $container.find('.layer').eq(0);
      $rightlayer = $container.find('.layer').eq(1);
    } else {
      // 螢幕大小太窄時調整slider的高度
      $container.height(lh);
    }

    // 呼叫resize函數
    resize();

	};

	function resize(){
		var _val = ($window.width() - lw) / 2 - lw * shift;

  $element.css({
			'width': lw * len,
			'left': _val
		});

    // 螢幕大小太窄時改變調整大小的方法
    if(!isNarrowScreen){
      $leftlayer.css('left', _val + lw);
      $rightlayer.css('left', _val + lw * 3);
    } else {
      $container.height(lh);
      $list.css({
      'width': lw,
      'height': lh
      });
    }

	};

	// アニメーションを管理する関数
	function slide(direction){

		// $elementがアニメーション中なら、処理しない
		if($element.filter(':animated').length) return;

		// ループを停止
		if(options.auto) clearInterval(timer);

		// 移動を変数に入れる。引数がtrueなら-lw,falseならlw
		val = (direction)? -lw: lw;

		// アニメーションスタート。最後にコールバック関数を呼び出す。
		$element.animate({
			'marginLeft': val
		}, options.duration, options.easing, callback);

	};

	// slide関数実行後に呼び出す関数
	function callback(){

		// valが0より小さければ、最初のliを最後に移動させる。　
		// valが0より大きければ、最後のliを最初に移動させる。
		(0 > val)? $element.find('li').eq(0).remove().appendTo($element): $element.find('li').eq(len - 1).remove().prependTo($element);

		// valが0より大きければ、最後のliを最初に移動させる。
		$element.css('marginLeft', 0);

		// options.autoがtrueなら、ループを開始
		if(options.auto) timer = setInterval(function(){ slide(true) }, options.interval);

	};

	function load(){
		var array = [$element, $next, $prev];
		for(var i = 0; i < array.length; i++) array[i].css('visibility', 'visible');
		$container.css('background', 'none');
	};

	function rollover(){
		$(".rollover").hover(function(){
			$(this).attr("src",$(this).attr("src").replace(/^(.+)(\.[a-z]+)$/, "$1_on$2"));
		},function(){
			$(this).attr("src",$(this).attr("src").replace(/^(.+)_on(\.[a-z]+)$/, "$1$2"));
		}).each(function(){
			$("<img>").attr("src",$(this).attr("src").replace(/^(.+)(\.[a-z]+)$/, "$1_on$2"));
		});
	};

	$window.on('load', initialize);

}());