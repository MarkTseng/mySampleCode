/**
 * autoscle.js
 *
 * @author       Toshiya TSURU <t_tsuru@sunbi.co.jp>
 * 
 * @version      $Id: autoscale.js 20 2014-02-09 13:40:46Z t_tsuru@sunbi.co.jp $
 *
 * Last changed: $LastChangedDate: 2014-02-09 22:40:46 +0900 (日, 09 2 2014) $
 * Author:       $Author: t_tsuru@sunbi.co.jp $
 * 
 */
(function() {
	jQuery.fn.extend({
		/**
		 * autoscale
		 */
		autoscale: function() {
			var _this = this;
			// attach window resize event handler
			if (!_this._autoscale_window_resize_event_handler_attached) {
				$(window).resize(function(evt){ _this.autoscale(); });
				_this._autoscale_window_resize_event_handler_attached = true;
			}
			// calculate scale
			var scalew = _this.parent().width()  / _this.width();
			var scaleh = _this.parent().height() / _this.height();
			var scale  = (scalew < scaleh)       ? scalew : scaleh;
			// apply scale
			return this.css({
				"transform"                : 'scale(' + scale + ', ' + scale + ')',
				"transform-origin"         : 'center middle',
				"-webkit-transform"        : 'scale(' + scale + ', ' + scale + ')',
				"-webkit-transform-origin" : 'center middle',
				"-ms-transform"            : 'scale(' + scale + ', ' + scale + ')',
				"-ms-transform-origin"     : 'center middle',
			});
		}
	});
})();