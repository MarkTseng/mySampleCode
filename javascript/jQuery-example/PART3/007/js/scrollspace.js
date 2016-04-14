/**
 * scrollspace.js
 * 
 * @copyright   RaNa design associates, inc.
 * @author      keisuke YAMAMOTO <keisukey@ranadesign.com>
 * @link        http://ranadesign.com/
 * @version     1.0
 * @date        2014/03/10
 *
 */
;(function($) {

	$.fn.scrollSpace = function(params) {
		var options = $.extend({
			z: 0,
			depth: 0.1
		}, params);
		var $this = this;
		var scrollSpace = function() {
			var scrollHeight = (document.body.scrollHeight - window.innerHeight) || 1;
			var y = window.scrollY || document.documentElement.scrollTop;
			var s = y / scrollHeight;
			s -= options.z - options.depth;
			s /= (options.z + options.depth) || 1;
			s = s < 0 ? 0 : s;
			var a = s;
			a = a > 0.8 ? 2 - a : a;
			$this.css({
				transform: "scale(" + s + ")",
				opacity: a
			}).toggle(a > 0);
		};
		$(window).scroll(scrollSpace).scroll();
	}
	
}(jQuery));