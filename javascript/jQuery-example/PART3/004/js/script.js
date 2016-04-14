;(function (d, $) {

	var param = {
		prog : 0,			// slide step current progress
		slideCount : $('.parallax').size() - 1, // total slide amount - 1
		stepMode : 0,		// 0 enable control, 1 disable control
		suspendTime : 1000,		// suspending time until user can control
		transDur : 800,
    calcHeight : function(){
			$('html, body').height( $(window).height() * (param.slideCount));
		},
    nav: {
      siteID: '#siteID a',
      pageNavi: '#navigation a',
      pagerNavi: '#pagerNavi a'
    },
    footer: {
      obj: $('#footerContainer'),
      posY: function(){
        return $(window).height() - param.footer.obj.height();
      }
    }
	};

	var _ua = (function(){
		return {
  	ltIE8:typeof window.addEventListener == "undefined" && typeof document.getElementsByClassName == "undefined",
  	Mobile:typeof window.orientation != "undefined"
		}
	})();

	var init = function() {

		if(!_ua.ltIE8) {

		var jarallax = new Jarallax();
		var animation = jarallax.addAnimation('#slide2',[{progress:'0', display:'block', opacity:'1', top:'120%'},
			{progress:'10', top:'0%', opacity:'1'},
			{progress:'20', top:'-50%', opacity:'0'}
			]);

		jarallax.addAnimation('#slide1',[
		{progress:'0', display:'block', opacity:'1', top:'0%'},
		{progress:'20', top:'-50%', opacity:'0'}]);

		jarallax.cloneAnimation('#slide3',{progress:'+10'}, animation);
            jarallax.cloneAnimation('#slide4',{progress:'+20'}, animation);
		jarallax.cloneAnimation('#slide5',{progress:'+30'}, animation);
		jarallax.cloneAnimation('#slide6',{progress:'+40'}, animation);
		jarallax.cloneAnimation('#slide7',{progress:'+50'}, animation);

		$('html').on('mousewheel',function( e, del, delX, delY) {
			e.preventDefault();
			if(param.stepMode == 1) { return; }
			var dest = param.prog - ((del>0)?1:-1);
			if(param.stepMode == 0 && dest < 0 || dest > param.slideCount){
				return;
			} else {
				param.stepMode = 1;
				param.prog = dest;
				jarallax.jumpToProgress( dest * 10, param.transDur, 60);
				setTimeout(changeMode, param.suspendTime);
			}
		});

		}

    $(window).on('resize', function(){
      adjustFooter();
    });

		// DOM attach event

    var _nav = param.nav;

		$(_nav.siteID + ',' + _nav.pageNavi + ',' + _nav.pagerNavi).on('click',function(e){
			e.preventDefault();
     });

		$(_nav.siteID + ',' + _nav.pageNavi).on('click',function(){
			var dest = $(this).attr('data-prog'), diff;
      if( dest != 0 ) {
        diff = Math.abs(param.prog - dest) + 1;
        param.prog = --dest;
      } else {
        diff = param.prog * 0.75;
        param.prog = 0;
      }
      jumpSlide(dest, diff);
		});

		$(_nav.pagerNavi).on('click',function(){
			var dir = $(this).index() == 0 ? -1: 1;
			var dest = parseInt(param.prog) + dir;
			if( dest >= 0 && dest <= param.slideCount) {
				jumpSlide(dest, 1);
				param.prog = dest;
			};
		});

		// handlers

		function changeMode(){
			param.stepMode = 0;
		};

		function jumpSlide(dest, diff) {
			if( !_ua.ltIE8 ) {
                    jarallax.jumpToProgress( dest * 10, param.transDur * diff, 60);
			} else {
				dest *= 940;
				$('html').stop(true).animate({ scrollTop: destH },1000);
			}
		};

	};

  function adjustFooter(){
    param.footer.obj.css({
      top: param.footer.posY
    });
  };

	$(function(){
		jQuery.fx.interval = 1;
    adjustFooter();
  	if(!_ua.Mobile) { param.calcHeight(); };
		$('#contentContainer').delay(1000).animate({
			opacity : 1
		},1000,function(){
			if(!_ua.Mobile) { init(); }
		});
	});

})(document, jQuery);