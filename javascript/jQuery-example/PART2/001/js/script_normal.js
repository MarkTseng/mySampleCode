;(function(d,$){

  //變數的預設值
  var jQdmtab_defaults = {
    tabContentsContainer: '.tabContentsContainer',
    tabEventAction: 'click',
   current: 0,
    currentSelector: 'current',
  };

  $.fn.jQdmtab = function(options){

    // 変数を設定
    var defaults = jQdmtab_defaults;
    var setting = $.extend( defaults, options);

    var _$obj = $(this.get(0)),
    _s = $.data( $(this), setting ),
    _p = {
      tabs: _$obj.find('li'),
      tabCn: _$obj.find(_s.tabContentsContainer),
      tabCnHeight: function(){
        var _$cns = _p.tabCn.children(),
        _len = _$cns.length,
        _hi = 0;
        while(_len > 0){
          _hi = Math.max( _hi, _$cns.eq(--_len).height());
        }
        return _hi + 40;
      },
      current: _s.current
    };

    //設定頁面顯示時初始頁簽
    tabChangeCurrent(_p.current);
    _p.tabCn.children().not(':eq('+ _p.current +')').css({
      display: 'none',
      opacity: 0
    });
    _p.tabCn.css({
      position: 'relative',
      overflow: 'hidden',
      background: '#f0f0f0',
      height: _p.tabCnHeight()
    });

    // 新增頁簽點擊事件
    _p.tabs.on( _s.tabEventAction, function(e){
      if(typeof e.preventDefault() === 'function') {
        e.preventDefaut();
      }

      var _$t = $(this),
      _index = _$t.index();
      _current = _p.current;

      if(_index != _current && !_p.isAnimate) {
        hideTabContent(_current);
        _p.current = _index;
        showTabContent(_index);
      }
    });

    // 頁簽內容隱藏處理
    function hideTabContent(_current){

      var _$target = _p.tabCn.children().eq(_current);
      tabChangeCurrent(_current);

     _$target.css({
        left: 0,
        opacity: 0,
        display: 'none',
        position: 'relative'
      });
    }

    // 頁簽內容顯示處理
    function showTabContent(_t){

      var _$target = _p.tabCn.children().eq(_t);
      tabChangeCurrent(_t);
      _$target.css({
        display: 'block',
        position: 'relative',
        opacity: 1
      });
    }

    // 切換至目前點擊之頁籤
    function tabChangeCurrent(_t){
      _p.tabs.eq(_t).toggleClass(_s.currentSelector);
    }

  }

  // 原生外掛的有效化
  // 一般型態
  $('#tabContainer1').jQdmtab();

  // 原生外掛的有效化：一般型態
  // 一般型態，頁籤寬度固定之型態
  $('#tabContainer1a').jQdmtab();

}(document, jQuery));