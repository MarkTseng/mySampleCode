;(function (d, $) {

  var jQdm_slideMenu;

  jQdm_slideMenu = function(){

    // 初始設定
    var prop = {
    	deferredImage: 'img.deferred',
    	subContainer: '#subContainer',
    	mainNavigation_trigger: '.mainNavigation_trigger',
    	mainNavigation_appear: '.appeared',

      pagerContainer: '.navigation',
      pagerTargetContainer: '#galleryContainer',
      pagerLoadingContents: '#galleryContainer .column'
    }

    function init(){

      // 執行圖片延遲載入的函數
    	deferredImage_setting()

      // 滑動選單的開閉按鈕設定
    	$(prop.mainNavigation_trigger).find('a').on('click', function(){
    		$(prop.subContainer).toggleClass(replaceString(prop.mainNavigation_appear));
    	});

      // 若捲動至特定元素，就開始載入外部頁面
      $(prop.pagerTargetContainer).infinitescroll({
        itemSelector: prop.pagerLoadingContents,
         animate: false, // 頁面載入後是否有動畫移動至特定位置
        extraScrollPx: 300,　// 頁面載入後移動的距離(px)
        pixelsFromNavToBottom: 50, // 捲動的位置距離頁面底部多少距離時開始載入頁面（px）
        loading: {
          img: "img/loader.gif",  // 載入中的圖片
          msgText: "圖片載入中…",  // 頁面載入時所顯示的資訊
          finishedMsg: "全部皆已載入"  //　頁面載入時所顯示的資訊
        },
      }, function(_t, _opt){  // 頁面載入後的處理
        var _len = _t.length;
        while(_len--){
          deferredImage_setting($(_t[_len]).find(prop.deferredImage));
        }
      });

	  }

    // 設定圖片延遲載入
    function deferredImage_setting(_$t){

      $( _$t || prop.deferredImage).lazyload({
        threshold : 10,
        effect: 'fadeIn',
        effectspeed: 1000
      });
    }

		// 文字列置換処理
    function replaceString(_str, _bf, _af, _flg){
      var _reg = new RegExp(_bf || '[\\.#]', _flg || '');
      return _str ? _str.replace(_reg, _af || '') : false;
    }

    init();

  };

  jQdm_slideMenu();

})(document, jQuery);