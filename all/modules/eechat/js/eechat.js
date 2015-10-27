
/**
 * @file
 * Javascript related stuff for the Earth Eternal Who block.
 */

(function ($) {
  jQuery(document).ready(function($) {
	  //var objDiv = document.getElementById("block-eechat-eechat-chat");
	  //objDiv.scrollTop = objDiv.scrollHeight;
	  /*var sh = $("#block-eechat-eechat-chat")[0].scrollHeight;
	  $("#block-eechat-eechat-chat").scrollTop(sh);*/
	  var sh = $(".chat-area")[0].scrollHeight;
	  $(".chat-area").scrollTop(sh);
  }); 

})(jQuery);
