/**
 * slider
 *
 * multirole slider plug-in
 *
 * @category	jQuery plugin
 * @license		http://www.opensource.org/licenses/mit-license.html	 MIT License
 * @copyright	2010 RaNa design associates, inc.
 * @author		keisuke YAMAMOTO <keisukey@ranadesign.com>
 * @link		http://www.ranadesign.com/
 * @version		4.0
 * @since		Aug 30, 2010
 * @update		Feb 13, 2014
 */

;(function($) {

	// for mobile =============================
	var mobile = "createTouch" in document;

	// slider ====================================
	$.fn.slider = function(param) {
		// 初始設定 ------------------------- //

		// 初始值
		var def = {
			loop: true,			// 是否支援loop
			time: 10,			// 移動1px的花費時間
			speed: 1,			// 往前往後快轉的比率
			direction: "left",	// 輪播方向
			reverse: true,		// 設定使用按鈕控制時是否可以反方向輪播
			auto: true,			// rue為自動播放，false則是關閉自動播放
			easing: "linear",	// Easing的初始值
			guideSelector: ".slideGuide",	// guide用的類別名稱
			cellSelector: ".slideCell", // cell用的類別名稱
			ctrlSelector: ".slideCtrl", // controller用的類別名稱
			ctrlClick: false,	// controller操作用的EventHandler(click)
			ctrlHover: true,	// controller操作用的EventHandler(hover)
			draggable: false,	// 是否支援拖拉操作
			dragCursorOpen: "open.cur", // 支援拖拉時的滑鼠指標圖示
			dragCursorClose: "close.cur", // 拖拉時的滑鼠指標圖示
			shuttle: false,		// 是否支援往返播放
			once: false,		// 設定是否播放1單位就停止
			restart: true,		// 是否支援restart(click事件用)
			restartTime: 3000,	// restart後的等待時間
			pause: true,		// 設定滑鼠移入時是否暫停
			build: true,		// 預設的選項(寬度的初始設定
			sp: 1				// 基本速度
		};

		return this.each(function() {

			// 私有屬性禁止覆蓋
			if (param) {
				delete param.guide;
				delete param.sp;
			}
			
			// 傳入參數
			$.extend(def, param);
			def.guide = $(this).find(def.guideSelector);

			// 當draggable或shuttle設定為true時，將相關選像設定為false
			if (def.draggable || def.shuttle) def.loop = def.auto = def.pause = false;

			// 內部處理用的屬性設定
			def.d = def.direction; // 紀錄方向的初始設定
			def.cell = def.cellSelector;
			def.ctrl = def.ctrlSelector;
			def.curOpen = "url(" + def.dragCursorOpen + "), default";
			def.curClose = "url(" + def.dragCursorClose + "), default";
			def.mousedownX = 0;
			
			if (!def.guide || def.loop && !def.guide.children(def.cell).length || !def.loop && def.guide.hasClass(def.guideSelector)) return true;
			
			// 取得frame border
			def.frameBorder = def.guide.offset().left - def.guide.parent().offset().left;
			// 取得cell margincellマージンの取得
			def.horizontalMargin = def.guide.find(def.cell).eq(0).outerWidth(true) - def.guide.find(def.cell).eq(0).outerWidth();
			def.verticalMargin = def.guide.find(def.cell).eq(0).outerHeight(true) - def.guide.find(def.cell).eq(0).outerHeight();

			// for mobile
			def.handlerMousedown = mobile ? "touchstart" : "mousedown";
			def.handlerMousemove = mobile ? "touchmove" : "mousemove";
			def.handlerMouseup = mobile ? "touchend" : "mouseup";

			// init 定義 =============================
			// slide border的寬度設定
			var init = function(def) {
				var cell = def.guide.find(def.cell);
				// slicdeCell的個數調整，不足時複製增加
				var max = 0;
				var size = 0;
				switch (def.direction) {
					case "up":
					case "down":
						// 取得最大寬度
						cell.each(function() {
							max = max > $(this).outerHeight() ? max : $(this).outerHeight();
							size += $(this).outerHeight(true);
						});
						// 若無支援loop時，不增加cell
						if (!def.loop) {
							def.guide.height(size);
							return false;
						}
						// 超過frame外框時增加cell
						while (size < def.guide.parent().height() + max) {
							cell.clone(true).appendTo(def.guide);
							size += cell.outerHeight();
						}
						// slideGuide的大小為size+max
						def.guide.height(size + max);
						break;
					case "left":
					case "right":
					default:
						// 取得cell的寬度
						cell.each(function() {
							max = max > $(this).outerWidth() ? max : $(this).outerWidth();
							size += $(this).outerWidth(true);
						});
						// 若無支援loop則不增加cell。
						if (!def.loop) {
							def.guide.width(size);
							return false;
						}
						// 超過frame外框時增加cell
						while (size < def.guide.parent().width() + max) {
							cell.clone(true).appendTo(def.guide);
							size += cell.outerWidth();
						}
					    // slideGuide的大小為size+max
						def.guide.width(size + max);
				}
			}
			var initTimerId = 0;

			// slider 定義 ==========================
			var slider = function(par) {
				var cell = par.guide.find(par.cell);
				
				if (par.loop) {
					cell.first = cell.eq(0);
					cell.last = cell.eq(cell.length - 1);

					switch (par.direction.toLowerCase()) {
						case "up":
							if (cell.first.height() === 0) {
								setTimeout(function() {
									slider(par);
								}, 200);
								return false;
							}
							cell.first.animate({
								marginTop: -1 * cell.first.innerHeight() - def.verticalMargin
							}, {
								duration: ~~Math.abs(par.time / par.sp * cell.first.height() * 
									(cell.first.offset().top - par.guide.parent().offset().top < 0 ? 
										(cell.first.height() + cell.first.offset().top - par.guide.parent().offset().top) / cell.first.height() : 1
									)
								),
								easing: par.easing,
								complete: function() {
									if (par.loop) {
										cell.first.appendTo(par.guide).css("marginTop", 0);
										if (!par.once) slider(par);
									}
								}
							});
							break;
						case "down":
							if (cell.first.height() === 0) {
								setTimeout(function() {
									slider(par);
								}, 200);
								return false;
							}
							if (cell.first.offset().top - par.guide.parent().offset().top < 0) {
								cell.first.animate({
									marginTop: 0
								},{
									duration: ~~Math.abs(par.time / par.sp * cell.first.height() * 
										(cell.first.offset().top - par.guide.parent().offset().top < 0 ? 
											(par.guide.parent().offset().top - cell.first.offset().top) / cell.first.height() : 1
										)
									),
									easing: par.easing,
									complete: function() { if (!par.once) slider(par); }
								});
							} else {
								cell.last.prependTo(par.guide).css("marginTop", -1 * cell.last.innerHeight() - def.verticalMargin);
								slider(par);
							}
							break;
						case "left":
							cell.first.animate({
								marginLeft: -1 * cell.first.innerWidth() - par.horizontalMargin
							}, {
								duration: ~~Math.abs(par.time / par.sp * cell.first.width() * 
									(cell.first.offset().left - par.guide.offset().left < 0 ? 
										(cell.first.width() + cell.first.offset().left - par.guide.offset().left) / cell.first.width() : 1
									)
								),
								easing: par.easing,
								complete: function() {
									if (par.loop) {
										cell.first.appendTo(par.guide).css("marginLeft", 0);
										if (!par.once) slider(par);
									}
								}
							});
							break;
						case "right":
							if (cell.first.offset().left - par.guide.offset().left < 0) {
								cell.first.animate({
									marginLeft: 0
								},{
									duration: ~~Math.abs(par.time / par.sp * cell.first.width() * 
										(cell.first.offset().left - par.guide.offset().left < 0 ? 
											(par.guide.offset().left - cell.first.offset().left) / cell.first.width() : 1
										)
									),
									easing: par.easing,
									complete: function() { if (!par.once) slider(par); }
								});
							} else {
								cell.last.prependTo(par.guide).css("marginLeft", -1 * cell.last.innerWidth() - par.horizontalMargin);
								slider(par);
							}
							break;
						default:
							return false;
					}
				
				} else { // 無loop時
					// controller的顯示
					var ctrl = par.guide.siblings(par.ctrl);
					ctrl.show();
					// 方向的取得
					var d = 0;
					switch (par.direction.toLowerCase()) {
						case "up":
						case "down":
							d = par.direction.toLowerCase() === "up" ?  -1 : 1;
							// slide設定
							par.guide.animate({
								marginTop: par.guide.height() * d
							}, {
								duration: par.time * par.guide.height() / par.sp,
								easing: par.easing
							});
							// 停止設定
							var gl = par.guide.offset().top;
							var fl = par.guide.parent().offset().top;
							var timerId = setInterval(function() {
								if (d > 0 && par.guide.offset().top > par.guide.parent().offset().top + par.frameBorder) {
									clearInterval(timerId);
									par.guide.stop(true);
									ctrl.filter(".up").hide();
								}
								if (d < 0 && par.guide.parent().height() + par.guide.parent().offset().top + par.frameBorder + par.verticalMargin > par.guide.height() + par.guide.offset().top) {
									clearInterval(timerId);
									par.guide.stop(true);
									ctrl.filter(".down").hide();
								}
							}, 20);
							break;
						case "left":
						case "right":
						default:
							d = par.direction.toLowerCase() === "left" ?  -1 : 1;
						    // slide設定
							par.guide.animate({
								marginLeft: par.guide.width() * d
							}, {
								duration: par.time * par.guide.width() / par.sp,
								easing: par.easing
							});
							// 停止設定
							var gl = par.guide.offset().left;
							var fl = par.guide.parent().offset().left;
							var timerId = setInterval(function() {
								if (d > 0 && par.guide.offset().left > par.guide.parent().offset().left + par.frameBorder) {
									clearInterval(timerId);
									par.guide.stop(true);
									ctrl.filter(".left").hide();
								}
								if (d < 0 && par.guide.parent().width() + par.guide.parent().offset().left + par.frameBorder + par.horizontalMargin > par.guide.width() + par.guide.offset().left) {
									clearInterval(timerId);
									par.guide.stop(true);
									ctrl.filter(".right").hide();
								}
							}, 20);
							break;
					}
				}
			}

			// 執行設定 ------------------------------- //

			// 幅度初始設定
			if (def.build) {
				$(window).resize(function() {
					clearTimeout(initTimerId);
					initTimerId = setTimeout(function() {
						init(def);
					}, 100);
				}).triggerHandler("resize");
			}

			// 自動起動
			if (def.auto) slider(def);

			// 選項設定 ------------------------- //

			// 滑鼠移入時暫停
			if (def.pause) {
				def.guide.hover(
					function() { $(this).find(def.cell).stop(true); },
					function() { slider(def); }
				);
			}
			
			// controllerコントローラ設定 ------------------------- //

			// ホバーイベント
			if (def.ctrlHover) {
				// for mobile
				if (mobile) {
					def.guide.siblings(def.ctrl)
						.bind(def.handlerMousedown, function(event) {
							event.preventDefault();
							def.guide.find(def.cell).stop(true);
							def.sp = def.speed;
							switch (true) {
								case $(this).hasClass("right"):
									def.direction = "right";
									break;
								case $(this).hasClass("up"):
									def.direction = "up";
									break;
								case $(this).hasClass("down"):
									def.direction = "down";
									break;
								case $(this).hasClass("left"):
								default:
								   def.direction = "left";
							}
							slider(def);
						}).bind(def.handlerMouseup, function() {
							def.guide.find(def.cell).stop(true);
							def.sp = 1;
							def.direction = def.d;
							if (def.auto) slider(def);
						});
				} else {
					// 往前播放、往後播放
					def.guide.siblings(def.ctrl).hover(
						function() {
							def.guide.stop(true).find(def.cell).stop(true);
							def.sp = def.speed;
							def.direction = $(this).hasClass("right") ? def.reverse ? "left" : "right" : def.reverse ? "right" : "left";
							switch (true) {
								case $(this).hasClass("right"):
									def.direction = def.reverse ? "left" : "right";
									break;
								case $(this).hasClass("up"):
									def.direction = def.reverse ? "down" : "up";
									break;
								case $(this).hasClass("down"):
									def.direction = def.reverse ? "up" : "down";
									break;
								case $(this).hasClass("left"):
								default:
									def.direction = def.reverse ? "right" : "left";
							}
							slider(def);
						},
						function() {
							def.guide.stop(true).find(def.cell).stop(true);
							def.sp = 1;
							def.direction = def.d;
							if (def.auto) slider(def);
						}
					);
				}
			}
			
			// 點擊事件
			if (def.ctrlClick) {
				def.guide.siblings(def.ctrl).bind(def.handlerMousedown, function(event) {
					// 省略動畫
					def.guide.find(def.cell).stop(true, true);
					// 初始動作停止
					event.preventDefault();
					// 方向設定
						def.direction = $(this).hasClass("right") ? def.reverse ? "left" : "right" : def.reverse ? "right" : "left";
					// 每點擊一次移動一單位
					def.once = true;
					slider(def);
					if (def.restart) {
						setTimeout(function() {
							def.once = false;
							slider(def);
						}, def.restartTime);
					}
				});
			}

			// 拖拉事件
			// dragmove設定
			var dragmove = function(event) {
				var frameEnd = def.guide.parent().width() + def.guide.parent().position().left;
				var guideEnd = def.guide.width() + def.guide.position().left;
				var stuff = 100;

				def.guide.css("position", "absolute").css("left", (mobile ? event.originalEvent.touches[0].pageX : event.pageX) - def.mousedownX);

				if (frameEnd - guideEnd > stuff || def.guide.offset().left - def.guide.parent().offset().left > stuff) {
					$(document).unbind(def.handlerMousemove, dragmove).one(def.handlerMouseup, function() {
						def.guide.animate({
							left: frameEnd - guideEnd > stuff ? "+=" + (frameEnd - guideEnd) : 0
						}, {
							duration: 500,
							easing: "easeOutQuart"
						});
					});
				}
			}
			if (def.draggable) {
				def.guide.bind(def.handlerMousedown, function(event) {
					event.preventDefault();
					def.mousedownX = (mobile ? event.originalEvent.touches[0].pageX : event.pageX) - def.guide.position().left;
					$(this).stop(true).css("cursor", def.curClose);
					$(document).bind(def.handlerMousemove, dragmove);
				});
				$(document).bind(def.handlerMouseup, function() {
					def.guide.css("cursor", def.curOpen);
					$(document).unbind(def.handlerMousemove, dragmove);
				});
			}
			
		});
	}; // slider end

}(jQuery));
