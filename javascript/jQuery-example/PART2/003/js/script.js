;(function (d, $) {

  var jQdm_slideMenu;

  jQdm_slideMenu = function(){

    // 初始設定
    var prop = {
    	deferredImage: 'img.deferred',
    	subContainer: '#subContainer',
    	mainNavigation_trigger: '.mainNavigation_trigger',
    	mainNavigation_appear: '.appeared'
    }

    function init(){

      // 圖片延遲載入設定
    	$(prop.deferredImage).lazyload({
    		threshold : 10,
    		effect: 'fadeIn',
    		effectspeed: 1000
    	});

      // 滑動選單的開閉按鈕設定
    	$(prop.mainNavigation_trigger).find('a').on('click', function(){
    		$(prop.subContainer).toggleClass(replaceString(prop.mainNavigation_appear));
    	});

	  }

		// 字串取代處理
    function replaceString(_str, _bf, _af, _flg){
      var _reg = new RegExp(_bf || '[\\.#]', _flg || '');
      return _str ? _str.replace(_reg, _af || '') : false;
    }

    init();

  };

  jQdm_slideMenu();

})(document, jQuery);