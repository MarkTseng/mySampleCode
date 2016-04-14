;(function (d, $) {

  var jQdm_horizontal_scroll;

  jQdm_horizontal_scroll = function(){

    // 初期設定
    var prop = {
      $window: $(window),
      contentContainer: '#contentContainer',
      columnContainer: '#columnContainer',
      columnContainerItem: '.column',
      columnMainContent: '.column_mainContent',
      vurtualContainer: 'body',
      transitionSpeed: 1000,
      transitionEasingDefault: 'easeOutQuint',
      transitionEasingJump: 'easeInOutQuint',
      mainNavigation: '#mainNavigation',
      dataPossessor: [],
      currentSelector: '.current',
      mode: {
        scroll: 1, // 0 瀏覽器縱向捲軸功能。 1 用滑鼠滾輪或手控滑動左右移動。無捲軸功能或手機環境的強制mode
        wheel: 1 // 滑鼠滾輪移動 0 無效 1 window用 2 mac用  0以外的狀況會自動判定1或2
      },
      fitWindowWidthScale: true, // 讓欄位寬度與視窗大小相同
      ua_query: {
        mobile: 'iphone ipad ipod android',
        mac: 'mac',
        windows: 'windows'
      }
    };

    function init(){

      var _$w = prop.$window,
      _$cn = $(prop.columnContainer),
      _$t = $(prop.columnContainerItem);

      // 頁面載入時或縮放時、設定snap元素調整為視窗大小
      _$w.on('load', function(){

        prop.mode.scroll = checkUserAgent('mobile') ? 1 : prop.mode.scroll;
        prop.mode.wheel = prop.mode.wheel == 0 ? 0 : checkUserAgent('mac') ? 2 : checkUserAgent('windows') ? 1 : prop.mode.wheel;
        if(prop.mode.scroll == 1) { // 捲動模式為1時，隱藏卷軸
          $('body').css('overflow','hidden');
        }

        if(checkUserAgent('mobile') || checkUserAgent('ipod')){
          $('*').removeClass('noSwipe');
        }
        setTargetPostion(_$t); // 執行取得個欄位目前位置的處理
        fitWindowScale(_$t);   // 執行將各欄位調整成同視窗大小的處理
      });

      _$w.on('resize', function(){
        setTimeout( function() {
          setTargetPostion(_$t);
          fitWindowScale(_$t);
        }, 200);
      });

      _$w.on('resize scroll', function(e){

        preventDefault(e);
        setTargetPostion(_$t);
        if($('body').is(':animated')) {
          return;
        }
        adjustScrollPosition();
      });

      // swipe事件
      _$w.on('touchmove',function(e){
          preventDefault(e);
      });
      _$w.swipe({
        swipeLeft:function(ev, dir, dist, dur, fin) {
          adjustScrollPosition(dist, dist);
        },
        swipeRight:function(ev, dir, dist, dur, fin) {
          adjustScrollPosition(-dist, dist);
        }
      });

      // 滑鼠滾輪事件
      _$w.on('mousewheel', function(e) {
          preventDefault(e);
          var _dur = e.deltaY;
          _dur = _dur > 50 ? 50 : _dur < - 50 ? -50 : _dur;
          switch(prop.mode.wheel){
            case 0:
              _dur = 0;
              break;
            case 1:
              _dur *= -150;
              break;
            case 2:
              _dur *= 2;
              break;
            default:
          }
          console.log(prop.mode.wheel);
          adjustScrollPosition(_dur, Math.abs(_dur) * 2);
      });

      // 瀏覽列點擊事件
      $(prop.mainNavigation).find('a').on('click', function(e){
        preventDefault(e);
        // 快取連結目標
        var _href = $(this).attr('href'),
        _diff = 0;
        // 移動至連結目標的動畫
        if(_href && $(_href).length > 0) {
          _diff = getColumnContainerInfo().left + parseInt($(_href).css('left'), 10);
          // 讓各內容於虛擬容器內的縱向位置移動。依照縱向捲動移動的事件，轉成橫向捲動
          adjustScrollPosition(_diff, prop.transitionSpeed, prop.transitionEasingJump);
        }
      });

      // 設定欄位配置的相關資訊
      setTimeout( function() {
        setTargetPostion(_$t);
      }, 200)

    }

    // 取得視窗捲動位置、scale等資訊
    function getWindowInfo(){

      var _$w = prop.$window;
      return {
        obj: _$w,
        w: window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth,
        h: window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight
      };
    }

    function getColumnContainerInfo(_val){
      var _$t = $(prop.columnContainer);
      return {
        obj: _$t,
        left: parseInt(_$t.css('left')),
        top: parseInt(_$t.css('top')),
        w: _$t.width(),
        maxpos: _$t.width() - getWindowInfo().w
      }
    }

    // 取得virtual容器的資訊
    function getVurtualContainerInfo(_val){
      var _$vc = $(prop.vurtualContainer),
      _win = getWindowInfo(),
      _res = _val ? _$vc.height(_val) : _$vc.height();
      return {
        obj: _$vc,
        h: _res,
        top: _$vc.scrollTop(),
        maxpos: _res - _win.h + _win.w * _$vc.height() / $(prop.columnContainer).width(),
        ratio: _$vc.height() / $(prop.columnContainer).width()
      };
    }

    // 讓元素同視窗大小
    function fitWindowScale(_$t){
      var _win = getWindowInfo();
      _$cn = $(prop.columnContainer),
      _pos = { x: 0, y: 0};

      _$t.each(function(){
          
          // 讓各欄位的高度、寬度同視窗大小(瀏覽器顯示區域)，並調整CSS讓其於欄位容器中橫向排列
        $(this).css({
          left: _pos.x,
          width: prop.fitWindowWidthScale ? _win.w : $(this).width(),
          height: _win.h
        });

        _pos.x += $(this).width();  // 將下個橫向排列欄位位置往右邊移動
        _pos.y += $(this).height(); // 為決定virtual容器的總高度，將各欄位的高度加總

      });

      _$cn.width(_pos.x); // 設定欄位內容的寬度
      getVurtualContainerInfo(_pos.y); // 設定Virtual容器的高度

    }

    function adjustScrollPosition(_diff, _dur, _ease){
        var _cn = getColumnContainerInfo(), // 取得欄位內容的資訊
        _win = getWindowInfo(), // 取得視窗資訊
        _vc = getVurtualContainerInfo(), // 取得Virtual內容的資訊
        _destX = _cn.left, // 儲存欄位容器橫向位置的計算結果
        _destY = 0;　// 儲存欄位容器縱向位置的計算結果

        if(prop.mode.scroll == 0) { // 捲動模式為0的時候，要將垂直位置轉換成橫向位置

          if(_diff == undefined) {
            // 若沒有設定移動距離，縱向捲動位置不可超過virtual內容的最大垂直位置
            _destY = _vc.top;// > _vc.h ? _vc.h : _vc.top;
          } else {
            _diff *= _vc.ratio; // 將縱向的virtual容器的移動距離轉換成橫向距離
            
            _destY = _vc.top  + _diff;
          }

          _destY = _destY <= 0 ? 0 : _destY >= _vc.maxpos ? _vc.maxpos : _destY;
          _destX = -_destY / _vc.ratio; 

        } else {
          // 手持式設備不用轉換縱向位置
          _destX -= _diff;
        }

        _destX = _destX >= 0 ? 0 : _destX <= -_cn.maxpos ? -_cn.maxpos : _destX;

        var _option = {  //動畫選項
          duration: _dur ? _dur : prop.mode.scroll != 0 ? 500 : 0,
          easing: _ease || prop.transitionEasingDefault,
          queue: false,
          complete: function(){
            setTargetPostion($(prop.columnContainerItem));
          }
        };
        _vc.obj.stop(true, true).animate({
          scrollLeft: _destX,
          scrollTop: prop.mode.scroll == 0 ? _destY : 0,
        }, _option);
        _cn.obj.stop(true, true).animate({
          left: _destX,
          top: prop.mode.scroll == 0 ? _destY : 0
        }, _option);
    }

    function setTargetPostion(_$t){
      prop.dataPossessor = [];
      _$t.each(function(){
        prop.dataPossessor.push(parseInt($(this).css('left'),10));
      });
      changeCurrent();
    }

    function changeCurrent(){
      var _win = getWindowInfo(),
      _cn = getColumnContainerInfo(),
      _pos = -_cn.left - _win.w / 2, 
      _dp = prop.dataPossessor,
      _$nav = $(prop.mainNavigation).find('li'),
      _cur = replaceString(prop.currentSelector);

      for(var i = 0; i < _dp.length; i++) {
        if( _pos <= _dp[i]){
          var _id = '#' + $(prop.columnContainerItem).eq(i).attr('id'),
          _$cur = $(prop.mainNavigation).find('a[href="'+_id+'"]');
          if(_$cur.length > 0){
            _$nav.removeClass(_cur);
            _$cur.parent('li').addClass(_cur);
          }
          return;
        }
      }
    }

    // 字串取代處理
    function replaceString(_str, _bf, _af, _flg){
      var _reg = new RegExp(_bf || '[\\.#]', _flg || '');
      return _str ? _str.replace(_reg, _af || '') : false;
    }

    // 取消預設動作
    function preventDefault(e){
      if(typeof e.preventDefault !== undefined){
          e.preventDefault();
      }
    }

    // 檢驗user agent
    function checkUserAgent(_t){
      var _ua = navigator.userAgent.toLowerCase(),
      _query = prop.ua_query,
      _res;
      for(var _k in _query){
        if(_ua.search(new RegExp(_k)) != -1) {
          _res = _k;
          break;
        }
        _res = _ua;
      }
      return _t ? _t == _res : _res;
    }

    init();

  };

  jQdm_horizontal_scroll();

})(document, jQuery);