;(function (d, $) {
  
  // pagetop pattern1
  
  var $pagetop = $('.pagetop a');
  
  $pagetop.on('click', function(e){
    e.preventDefault();
    $('html, body').animate({
      scrollTop: $($(this).attr("href")).offset().top
      }, 1200, 'easeInOutQuart');
  })
 
})(document, jQuery);