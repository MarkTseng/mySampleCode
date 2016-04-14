/**
 * scrollbg.js
 * 
 * @copyright   RaNa design associates, inc.
 * @author      keisuke YAMAMOTO <keisukey@ranadesign.com>
 * @link        http://ranadesign.com/
 * @version     1.0
 * @date        2014/03/10
 *
 */
;(function($) {

	var defaultOptions = {
		smallImgPath: "img/bg/small/",
		largeImgPath: "img/bg/large/",
		imgExt: ".png",
		imgCount: 150,
		countStart: 100
	};
	
	// 低解析度
	$.fn.scrollBgSmall = function(options) {
		var $this = this;
		var smallCount = options.countStart;
		var smallImgs = [];
		var setSmallBg = function(id) {  // 設定對應序號的圖片
			if (smallImgs[id] && smallImgs[id].complete) {
				$this.css({
					backgroundImage: "url(" + smallImgs[id].src + ")"
				});
			}
		};
		var scrollBg = function(event) {  // Scroll事件的Handler
			var scrollHeight = (document.body.scrollHeight - window.innerHeight) || 1;
			var y = window.scrollY || document.documentElement.scrollTop;
			var id = options.imgCount * y / scrollHeight | 0;
			id = id < 0 ? 0 : id > options.imgCount - 1 ? options.imgCount - 1 : id;
			setSmallBg(id);
		};
		// Preload
		while (smallCount++ < options.countStart + options.imgCount) {
			var img = new Image();
			img.src = options.smallImgPath + smallCount + options.imgExt;
			smallImgs.push(img);
		}
		// 設置初始圖片
		setSmallBg(0);
		// Bind Scroll事件
		$(window).scroll(scrollBg);
	};
	
	// 高解析度
	$.fn.scrollBgLarge = function(options) {
		var $this = this;
		var $large = $("<div />", { "class": "mod-scrollBg" });
		var timerId = 0;
		var largeImgs = [];
		var setLargeBg = function(id) {
			var src = options.largeImgPath + (id + options.countStart + 1) + options.imgExt;
			if (largeImgs[id] && largeImgs[id].complete) {
				src = largeImgs[id].src;
			} else {
				var img = new Image();
				img.src = src;
				largeImgs[id] = img;
			}
			$large.css({
				backgroundImage: "url(" + src + ")",
				opacity: 1
			});
		};
		var scrollBg = function(event) {
			var scrollHeight = (document.body.scrollHeight - window.innerHeight) || 1;
			var y = window.scrollY || document.documentElement.scrollTop;
			var id = options.imgCount * y / scrollHeight | 0;
			id = id < 0 ? 0 : id > options.imgCount - 1 ? options.imgCount - 1 : id;
			$large.css("opacity", 0);
			clearTimeout(timerId);
			timerId = setTimeout(function() {  // 預防閃爍
				setLargeBg(id);
			}, 50);
		};
	    //建立高解析度圖片用的div
		$this.append($large);
		// 設置初始圖片
		setLargeBg(0);
	    // Bind Scroll事件
		$(window).scroll(scrollBg);
	};

	$.fn.scrollBg = function(params) {
		var options = $.extend(defaultOptions, params);
		this.scrollBgSmall(options);
		this.scrollBgLarge(options);
	};

}(jQuery));