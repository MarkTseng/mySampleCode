(function(){

  var $frame01 = $('#frame01'),
      $frame02 = $('#frame02'),
      $frame03 = $('#frame03'),
      $frame04 = $('#frame04'),
      $frame05 = $('#frame05'),
      $frame06 = $('#frame06'),
      $frame07 = $('#frame07'),
      $frame08 = $('#frame08'),
      $frame09 = $('#frame09'),
      $frameGroup01 = $('#frame06, #frame07, #frame08'),
      $skip = $('#skip'),
      $animation = $('#animation');

  function animation01() {
    return $frame01.fadeIn(500);
  }

  function animation02(){
    return $frame02.fadeIn(500);
  }

  function animation03(){
    return $frame03.fadeIn(500);
  }

  function animation04(){
    return $frame04.fadeIn(500);
  }

  function animation05(){
    return $frame05.animate({
      bottom: 105
    }, 500, 'easeOutBack');
  }

  function animation06(){
    return $frame06.animate({
      right: 260
    }, 500, 'easeOutExpo');
  }

  function animation07(){
    return $frame07.animate({
      right: 140
    }, 500, 'easeOutExpo');
  }

  function animation08(){
    return $frame08.animate({
      right: 20
    }, 500, 'easeOutExpo');
  }

  function animation09(){
    return $frameGroup01.delay(500).fadeOut(1000);
  }

  function animation10(){
    return $frame09.fadeIn(2000);
  }

  function animation11(){
    return $skip.fadeOut(500);
  }

  function onSkip(){
    $frame01.stop(true, true).show();
    $frame02.stop(true, true).show();
    $frame03.stop(true, true).show();
    $frame04.stop(true, true).show();
    $frame05.stop(true, true).css('bottom', 105);
    $frame06.stop(true, true).css('right', 260);
    $frame07.stop(true, true).css('right', 140);
    $frame08.stop(true, true).css('right', 20);
    $frameGroup01.stop(true, true).hide();
    $frame09.stop(true, true).show();
    $skip.stop(true, true).hide();
  }

  var $deferred = $.Deferred(function(deferred){
    deferred.then(animation01)
            .then(animation02)
            .then(animation03)
            .then(animation04)
            .then(animation05)
            .then(animation06)
            .then(animation07)
            .then(animation08)
            .then(animation09)
            .then(animation10)
            .then(animation11);
  });

  $animation.css('visibility', 'hidden');
  $skip.on('click', function(){
    onSkip();
    return false;
  });

  $(window).on('load', function(){
    $animation.css('visibility', 'visible');
    $deferred.resolve();
  });

}());