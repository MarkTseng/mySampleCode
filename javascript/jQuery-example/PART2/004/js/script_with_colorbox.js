;(function (d, $) {

	var jQdm_flexGrid;

	jQdm_flexGrid = function(){

		// 初期設定
		var param = {
			masonryTargetContainer: $('#photoGalleryContainer'),
			itemSelector: '.column',
			loaderSymbolSelector: '.loaderSymbol' // 載入中的CSS選擇器
		};

	//為方便之後操作，將變數中的容器資訊於初始時先設定至$container，
//容器主要是在Masonry中用以放置排版用的格子元素
		var　$container = param.masonryTargetContainer;
		// 將masonry的基本設定當作參數傳入函數
		$container.masonry({
				columnWidth: '.grid_sizer',	// 設定基本寬度的CSS選擇器整列用基本
			  itemSelector: param.itemSelector	// 格子的CSS選則器
		});

		// 顯示載入中動畫圖片的處理
		var LoaderSymbol = {
			init: function(){
				var _sel = param.loaderSymbolSelector;
				if( $(_sel).length == 0 ) {
					$('<div>').prependTo('body').addClass(replaceString(_sel, '\\.')).hide().fadeIn(1000);
				}
			},
			destroy: function(){
				$(param.loaderSymbolSelector).fadeOut(500,
					function(){
						$(this).remove();
						revealItems();
					});
			}
		};

		// 顯示所有載入圖片並呼叫執行Masonry外掛
		LoaderSymbol.init();
		$container.imagesLoaded().done(function(){
			// 圖片載入結束
			LoaderSymbol.destroy();
		});

		// 顯示載入的圖片
		function revealItems(){
			var _elmes, _items;
			_elems = $container.masonry('getItemElements'),
			_items = $container.masonry( 'getItems', _elems );

			$container.children().css({
				visibility: 'visible'
				});
			$container.fadeIn().masonry('reveal', _items).masonry();

			// 呼叫對元素設定colorBox的函數
			colorBoxSetting(param.itemSelector);
		}

		// 執行colorBox
		function colorBoxSetting(_t){
			$(_t).find('a').colorbox();
		}

		// 字串取代處理
    function replaceString(_str, _bf, _af, _flg){
      var _reg = new RegExp(_bf, _flg || '');
      return _str.replace(_reg, _af || '');
    }

	}


	jQdm_flexGrid();



})(document, jQuery);