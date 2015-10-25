
/**
 * @file
 * Javascript related stuff for the Earth Eternal Leaderboard.
 */

(function ($) {
	jQuery(document).ready(function() {
	    var al = $(".menu a.active");
	    al.closest("li.menu__item").addClass("is-active-trail");
	});

})(jQuery);
