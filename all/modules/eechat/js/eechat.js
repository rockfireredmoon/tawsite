
/**
 * @file
 * Javascript related stuff for the Earth Eternal Who block.
 */

(function ($) {
  jQuery(document).ready(function($) {
	  //var objDiv = document.getElementById("block-eechat-eechat-chat");
	  //objDiv.scrollTop = objDiv.scrollHeight;
	  var sh = $("#block-eechat-eewho-who")[0].scrollHeight;
	  $("#block-eechat-eewho-chat").scrollTop(sh);
  });

})(jQuery);
