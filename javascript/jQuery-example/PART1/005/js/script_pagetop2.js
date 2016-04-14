;(function (d, $) {
  
  // pagetop pattern2

  var $pagetop = isMobile()?$('.pagetop'):
  (function(){
   return $('.pagetop').removeClass('pagetop').addClass('pagetop2');
  })(), // タッチデバイスでなければ、pagetopをpagetop2に差し替える
  pagetop_offset = 400;

  // ページトップをクリックするとページの先頭へ戻る  
  $pagetop.on('click', function(e){
      e.preventDefault();
      var _dest = $($(this).find('a').attr('href')).offset().top;      
      $('html, body').animate({
        scrollTop: _dest
      }, 1200, 'easeInOutQuart');
  });
  
  // 一定量スクロールしたらページトップボタンをフェードイン
  if( !isMobile() ) {
   $pagetop.hide();
   $(window).on('scroll', function () {
    if ($(this).scrollTop() > pagetop_offset) {
     $pagetop.fadeIn(400);
    } else {
     $pagetop.fadeOut(400);
    }
   });
  }
  
  // タッチデバイス（スマートフォンなど）かどうかを判定する関数
  function isMobile(){
   return typeof window.orientation != "undefined";
  }
 
})(document, jQuery);