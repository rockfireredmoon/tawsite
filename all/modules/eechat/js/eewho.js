
/**
 * @file
 * Javascript related stuff for the Earth Eternal Chat block.
 */

(function ($) {
  jQuery(document).ready(function($) {
	  //var objDiv = document.getElementById("block-eechat-eechat-chat");
	  //objDiv.scrollTop = objDiv.scrollHeight;
	  var sh = $("#block-eechat-eewho-who")[0].scrollHeight;
	  $("#block-eechat-eewho-who").scrollTop(sh);
  }); 

})(jQuery);
