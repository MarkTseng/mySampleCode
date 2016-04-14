;(function(d,$){

  var JQdmtips;

  JQdmtips = function(){

    // 定義tips所用的共同設定
    var param = {
      tipsContentContainer: '.tipsContentContainer',
      tipsContentGenerateContainer: 'body',
      tipsSelector: '*[title], *[data-tips-image], *[data-tips-movie]',
      tipsAttrTitle: 'title',
      tipsAttrBasic: 'data-tips',
      tipsAttrImage: 'data-tips-image',
      tipsPositionAttr: 'data-tips-pos',
      dataPossessor: d.body,

      imageMagnificationRate: 2.5, // 圖片放大率
       // tips氣球的樣式。CSS中輸入文字顏色、背景顏色的選擇器，預設為不輸入。
      tipsBallonColorSelector: 'color3'
    };

    // 移入tips目標元素時的顯示處理
    $(param.tipsSelector).on('mouseover', function(e){
      showTipsContent($(this));
    });

    // 移出tips目標元素時的隱藏處理
    $(param.tipsSelector).on('mouseout', function(e){
      hideTipsContent($(param.tipsContentContainer));
    });

    /* tips氣球顯示 */
    function showTipsContent(_$t){

      // 定義處理中所使用的變數
      var _$tcn = $(param.tipsContentContainer),
      _$tcgn = $(param.tipsContentGenerateContainer),
      _p = {
        tipsWidth: 0,
        tipsHeight: 0,
        triggerWidth: 0,
        triggerHeight: 0,
        contentAttr: (function(){
          return _$t.attr(param.tipsAttrImage) ? param.tipsAttrImage :
          _$t.attr(param.tipsAttrBasic) ? param.tipsAttrBasic :
          _$t.attr(param.tipsAttrTitle) ? param.tipsAttrTitle : null;
        })()
      },
      _attr = _p.contentAttr,
      _pos = _$t.attr(param.tipsPositionAttr),
      _$cnt;

      // 建立tips內容的容器
      if(isElem(_$tcn)) {
        hideTipsContent(_$tcn);
      }
      _$tcn = $('<div>').prependTo(_$tcgn).addClass(replaceString(param.tipsContentContainer, '\\.')).addClass(replaceString(param.tipsBallonColorSelector, '\\.'));

      // 從屬性決定tips內容
      switch(_attr){

        // 圖片簡易放大功能的處理
        case 'data-tips-image':
          var _base = {
            w: _$t.parent().width(),
            h: _$t.parent().height()
          },
          _rate = _base.h / _base.w,
          _magW = _base.w * param.imageMagnificationRate,
          _magH = _magW * _rate;

          _$cnt = $('<img>').appendTo(_$tcn).attr({
            src: _$t.attr(_attr)
          });
          _$tcn.css({
            width: _magW,
            height: _magH,
            overflow: 'hidden'
          });
          _p.triggerWidth = _base.w;
          _p.triggerHeight = _base.h;
          _pos = _pos || 'tc';
          break;

        // 一般的tips
        default:
          _$cnt = $('<p>').appendTo(_$tcn).html(_$t.attr(_attr));
          // 複製title屬性的內容給data-tips屬性
          _$t.attr(param.tipsAttrBasic, _$t.attr(_attr));
          _$t.removeAttr('title');
      }


      // 依data-tips-dir屬性決定tips容器的位置
      _pos = _pos || 'bc';
      $.data(param.dataPossessor, 'tipsWidth', _p.tipsWidth || _$tcn.width());
      $.data(param.dataPossessor, 'tipsHeight', _p.tipsHeight || _$tcn.height());
      $.data(param.dataPossessor, 'triggerWidth', _p.triggerWidth || _$t.width());
      $.data(param.dataPossessor, 'triggerHeight', _p.triggerHeight || _$t.height());
      _$tcn.css({
        top: _$t.offset().top + cordinatePosition(_pos.charAt(0)),
        left: _$t.offset().left + cordinatePosition(_pos.charAt(1)),
        opacity: 1
      });
    }

    /* 隱藏tips氣球 */
    function hideTipsContent(_$t){
      if(isElem(_$t)){
        _$t.animate({
          opacity: 0
        }, 500, function(){
          _$t.remove();
        });
      }
    }

    // 調整tips氣球位置
    function cordinatePosition(_pos){
      return _pos == 't' ? -detectScale('tipsHeight') - 48:
      _pos == 'm' ? -detectScale('tipsHeight') / 2 :
      _pos == 'b' ? +detectScale('triggerHeight') + 24 :
      _pos == 'l' ? - detectScale('tipsWidth') :
      _pos == 'c' ? - detectScale('tipsWidth') / 2 + detectScale('triggerWidth') / 2 :
      _pos == 'r' ? + detectScale('triggerWidth') + 32 : 0;
    }

    // 偵測元素個數
    function isElem(_$t){
      return _$t.length;
    }

    // 取得暫存值
    function detectScale(_param){
      return $.data(param.dataPossessor, _param) || 0;
    }

    // 字串置換處理
    function replaceString(_str, _bf, _af, _flg){
      var _reg = new RegExp(_bf, _flg || '');
      return _str.replace(_reg, _af || '');
    }

  };


  JQdmtips();

})(document, jQuery);