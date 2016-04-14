//rollover

(function(){

	function rollover(){
		$(".rollover").hover(function(){
			$(this).attr("src",$(this).attr("src").replace(/^(.+)(\.[a-z]+)$/, "$1_on$2"));
		},function(){
			$(this).attr("src",$(this).attr("src").replace(/^(.+)_on(\.[a-z]+)$/, "$1$2"));
		}).each(function(){
			$("<img>").attr("src",$(this).attr("src").replace(/^(.+)(\.[a-z]+)$/, "$1_on$2"));
		});
	};

	$(window).on('load', function(){
		rollover();
	});
	
}());