;(function (d, $) {
  
  // pagetop pattern3

  var $pagetop = isMobile()?$('.pagetop'):
  (function(){
   return $('.pagetop').removeClass('pagetop').addClass('pagetop3');
  })(), // タッチデバイスでなければ、pagetopをpagetop3に差し替える
  pagetop_offset = 50;

  // ページトップをクリックするとページの先頭へ戻る  
  $pagetop.on('click', function(e){
      e.preventDefault();
      var _dest = $($(this).find('a').attr('href')).offset().top;
      $('html, body').animate({
        scrollTop: _dest
      }, 1200, 'easeInOutQuart');
  });
  
  // 一定量スクロールしたらページトップボタンをスライドイン
  if( !isMobile() ) {
   $(window).on('scroll', function () {
    if ($(this).scrollTop() > pagetop_offset) {
     $pagetop.css('visibility','visible').addClass('visible');
    } else {
        $pagetop.removeClass('visible');
    }
   });
  }
   
  // タッチデバイス（スマートフォンなど）かどうかを判定する関数
  function isMobile(){
   return typeof window.orientation != "undefined";
  }
 
})(document, jQuery);